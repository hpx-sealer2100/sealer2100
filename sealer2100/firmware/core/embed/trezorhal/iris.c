#include "iris.h"
#include <stdbool.h>

#include STM32_HAL_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "stm32h7xx_hal_def.h"
#include "stm32h7xx_hal_dma.h"
#include "stm32h7xx_hal_gpio.h"
#include "stm32h7xx_hal_rcc.h"
#include "stm32h7xx_hal_uart.h"
#include "stm32h7xx_hal_uart_ex.h"

#include "alignment.h"
#include "common.h"
#include "irq.h"
#include "log.h"
#include "trans_fifo.h"
#include "ed25519-donna/ed25519.h"

#define MODULE          "iris"

#define ENABLE_IRIS_LOG 1

#if !ENABLE_IRIS_LOG
  // disable log
  #undef LOG_DEBUG
  #undef LOG_HEXDUMP_DEBUG
  #define LOG_DEBUG(module, fmt, ...)                log_dummy(module, fmt, ##__VA_ARGS__)
  #define LOG_HEXDUMP_DEBUG(module, label, buf, len) log_dummy(module, label, buf, len)
#endif

#include "iris_sec_channel.c"

#define USART_TIMEOUT    1000
#define UART_BUF_MAX_LEN 256
// plain packet sync
#define SYNC_H0          0xFE
#define SYNC_H1          0xFD
// encrypted packet sync
#define SYNC_SEC_H0      0xFA
#define SYNC_SEC_H1      0xFB

/**
 * the packet overhead length
 * packet: 0xFE, 0xFD, msg_id, msg_type, N:2 bytes, <N bytes data>, crc
 * encrypted packet: 0xFE, 0xFC, N: 2 bytes, <N bytes ciphertext>, crc
 */
#define SYNC_COUNT                2
#define PLAIN_PACKET_OVERHEAD     (6 + 1)
#define ENCRYPTED_PACKET_OVERHEAD (4 + 1)
#define RESPONSE_HEADER_SIZE      4
#define UART_TX_BUFFER_SIZE       (4 * 1024)
#define UART_RX_BUFFER_SIZE       (256)

static UART_HandleTypeDef uart;
static DMA_HandleTypeDef hdma_tx;

static uint8_t xor (uint8_t init, const uint8_t* buf, size_t len);
static uint8_t uart_tx_doing = 0;

// buffer in sram3, see um0399 section 2.3.2
// next ble buffer
static uint8_t* recv_buf = (uint8_t*)0x30040200; // 256 bytes
static uint8_t* send_buf = (uint8_t*)0x30040300; // 4K bytes

static uint8_t __fifo_buf__[UART_BUF_MAX_LEN];
static trans_fifo uart_fifo_in = {
    .p_buf = __fifo_buf__,
    .buf_size = UART_BUF_MAX_LEN,
    .over_pre = false,
    .read_pos = 0,
    .write_pos = 0,
    .lock_pos = 0,
};

static void iris_uart_tx_complete(UART_HandleTypeDef* huart) {
    LOG_DEBUG(MODULE, "uart tx complete");
    uart_tx_doing = 0;
}
static void iris_uart_rx_event(UART_HandleTypeDef* huart, uint16_t Size) {
    // LOG_DEBUG(MODULE, "uart rx event: %d, size: %d", huart->RxEventType, Size);
    if (huart->RxEventType == HAL_UART_RXEVENT_IDLE) {
        // LOG_HEXDUMP_DEBUG(MODULE, "uart rx", recv_buf, Size);
        fifo_write_no_overflow(&uart_fifo_in, recv_buf, Size);
        // receive again
        HAL_UARTEx_ReceiveToIdle_IT(&uart, recv_buf, UART_RX_BUFFER_SIZE);
    }
}

void iris_usart_init(void) {
    /**
     * USART2
     * TX : PA2
     * RX : PD6
     */

    __HAL_RCC_USART2_CLK_ENABLE();
    __HAL_RCC_USART2_FORCE_RESET();
    __HAL_RCC_USART2_RELEASE_RESET();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();

    __HAL_RCC_DMA1_CLK_ENABLE();

    // USART2: PA2(TX), PD6(RX)
    GPIO_InitTypeDef gpio = {0};
    gpio.Mode = GPIO_MODE_AF_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    gpio.Alternate = GPIO_AF7_USART2;

    gpio.Pin = GPIO_PIN_2;
    HAL_GPIO_Init(GPIOA, &gpio);

    gpio.Pin = GPIO_PIN_6;
    HAL_GPIO_Init(GPIOD, &gpio);

    uart.Instance = USART2;
    uart.Init.BaudRate = 115200;
    uart.Init.WordLength = UART_WORDLENGTH_8B;
    uart.Init.StopBits = UART_STOPBITS_1;
    uart.Init.Parity = UART_PARITY_NONE;
    uart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    uart.Init.Mode = UART_MODE_TX_RX;
    uart.Init.OverSampling = UART_OVERSAMPLING_16;
    uart.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    uart.Init.ClockPrescaler = UART_PRESCALER_DIV1;

    if (HAL_UART_Init(&uart) != HAL_OK) {
        ensure(secfalse, "uart init failed");
    }

    HAL_UART_RegisterRxEventCallback(&uart, iris_uart_rx_event);
    HAL_UART_RegisterCallback(&uart, HAL_UART_TX_COMPLETE_CB_ID, iris_uart_tx_complete);

    NVIC_SetPriority(USART2_IRQn, IRQ_PRI_UART);
    HAL_NVIC_EnableIRQ(USART2_IRQn);

    hdma_tx.Instance = DMA1_Stream4;
    hdma_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    hdma_tx.Init.Request = DMA_REQUEST_USART2_TX;
    hdma_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_tx.Init.Mode = DMA_NORMAL;
    hdma_tx.Init.Priority = DMA_PRIORITY_LOW;

    HAL_DMA_DeInit(&hdma_tx);

    HAL_DMA_Init(&hdma_tx);
    NVIC_SetPriority(DMA1_Stream4_IRQn, IRQ_PRI_UART);
    HAL_NVIC_EnableIRQ(DMA1_Stream4_IRQn);

    __HAL_LINKDMA(&uart, hdmatx, hdma_tx);

    // receive first powerup moudle status packet
    uart_tx_doing = 0;
    HAL_UARTEx_ReceiveToIdle_IT(&uart, recv_buf, UART_RX_BUFFER_SIZE);
}

void DMA1_Stream4_IRQHandler(void) {
    HAL_DMA_IRQHandler(&hdma_tx);
}

void USART2_IRQHandler(void) {
    HAL_UART_IRQHandler(&uart);
}

void iris_usart_deinit(void) {
    HAL_NVIC_DisableIRQ(DMA1_Stream4_IRQn);
    HAL_DMA_DeInit(&hdma_tx);

    HAL_NVIC_DisableIRQ(USART2_IRQn);
    HAL_UART_DeInit(&uart);

    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2);
    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_6);
}

static inline void iris_resp_cache_reset(void) {
    fifo_flush(&uart_fifo_in);
}
static inline bool iris_cmd_need_encrypted(iris_msg_id_t id) {
    // all ota command no need encrypt
    if (id >= IRIS_MSG_ID_PACKET_OTA_START && id <= IRIS_MSG_ID_PACKET_OTA_STATUS) {
        return false;
    }
    // all handshake command no need encrypt
    if (id >= IRIS_MSG_ID_HANDSHAKE_HELLO && id <= IRIS_MSG_ID_HANDSHAKE_FINISHED) {
        return false;
    }

    return true;
}

// used to send builded msg, used by firmware
int iris_usart_send(const uint8_t* msg, size_t len) {
    iris_resp_cache_reset();
    // msg: <msg_id><payload_size><payload>
    LOG_HEXDUMP_DEBUG(MODULE, "sending(msg)", msg, len);
    iris_msg_id_t id = msg[0];
    uint8_t* p = send_buf;
    if (iris_cmd_need_encrypted(id) && iris_sec_ctx.sec_chan_established) {
        // `sync` 2 bytes
        *p++ = SYNC_SEC_H0;
        *p++ = SYNC_SEC_H1;
        // skip `encrypted_size` 2 bytes

        // encrypt cmd
        int record_size = iris_sec_channel_encrypt(msg, len, p+2, UART_TX_BUFFER_SIZE - 4);
        if (record_size <= 0) {
            return IRIS_ERROR_SEC_FAILED;
        }
        // `encrypted_size` 2 bytes
        PUT_UINT16_LE(record_size, p, 0);

        // move over
        p += 2;
        p += record_size;

        // `crc` 1 byte
        *p++ = xor(0, send_buf + 2, p - send_buf - 2);
    } else {
        // `sync` 2 bytes
        *p++ = SYNC_H0;
        *p++ = SYNC_H1;
        // `msg` N bytes, user have build msg
        memcpy(p, msg, len);
        p += len;
        // `crc` 1 byte
        *p++ = xor(0, msg, len);
    }
    LOG_HEXDUMP_DEBUG(MODULE, "dma sending", send_buf, p - send_buf);
    uart_tx_doing = 1;
    HAL_UART_Transmit_DMA(&uart, send_buf, p - send_buf);
    return IRIS_ERROR_OK;
}

static iris_error_t iris_send_encrypted_request(const iris_request_t* req) {
    uint8_t cmd[256] = {0};
    uint8_t* p = cmd;
    // encode request
    // `id` 1 byte
    *p++ = req->id;
    // `type` 1 byte
    *p++ = IRIS_MSG_TYPE_REQUEST;
    // `payload_size` 2 bytes
    PUT_UINT16_LE(req->payload_size, p, 0);
    p += 2;
    // `payload` N bytes
    if (req->payload_size > 0) {
        memcpy(p, req->payload, req->payload_size);
        p += req->payload_size;
    }
    // `crc` 1 byte
    *p++ = xor(0, cmd, p - cmd);
    size_t cmd_size = p - cmd;
    LOG_HEXDUMP_DEBUG(MODULE, "sending(msg)", cmd, cmd_size);

    // encode encrypted cmd
    p = send_buf;
    // `sync` 2 bytes
    *p++ = SYNC_SEC_H0;
    *p++ = SYNC_SEC_H1;
    // skip `encrypted_size` 2 bytes

    // encrypt cmd
    int record_size = iris_sec_channel_encrypt(cmd, cmd_size, p+2, UART_TX_BUFFER_SIZE - 4);
    if (record_size <= 0) {
        return IRIS_ERROR_SEC_FAILED;
    }
    // `encrypted_size` 2 bytes
    PUT_UINT16_LE(record_size, p, 0);

    // move over
    p += 2;
    p += record_size;

    // `crc` 1 byte
    *p++ = xor(0, send_buf + 2, p - send_buf - 2);
    LOG_HEXDUMP_DEBUG(MODULE, "dma sending", send_buf, p - send_buf);
    // send encrypted cmd
    uart_tx_doing = 1;
    HAL_UART_Transmit_DMA(&uart, send_buf, p - send_buf);
    return IRIS_ERROR_OK;
}

static iris_error_t iris_send_plain_request(const iris_request_t* req) {
    uint8_t* p = send_buf;
    // `sync` 2 bytes
    *p++ = SYNC_H0;
    *p++ = SYNC_H1;
    // `id` 1 byte
    *p++ = req->id;
    // `type` 1 byte
    *p++ = IRIS_MSG_TYPE_REQUEST;
    // `payload_size` 2 bytes
    PUT_UINT16_LE(req->payload_size, p, 0);
    p += 2;

    // `payload` N bytes
    if (req->payload_size > 0) {
        memcpy(p, req->payload, req->payload_size);
        p += req->payload_size;
    }
    // `crc` 1 byte
    *p++ = xor(0, send_buf + 2, p - send_buf - 2);

    LOG_HEXDUMP_DEBUG(MODULE, "dma sending", send_buf, p - send_buf);
    uart_tx_doing = 1;
    HAL_UART_Transmit_DMA(&uart, send_buf, p - send_buf);
    return IRIS_ERROR_OK;
}

iris_error_t iris_send_request(const iris_request_t* req) {
    iris_resp_cache_reset();
    // use dma send request
    LOG_DEBUG(MODULE, "sending request id: %d", req->id);
    LOG_HEXDUMP_DEBUG(MODULE, "payload ", req->payload, req->payload_size);

    if (iris_cmd_need_encrypted(req->id) && iris_sec_ctx.sec_chan_established) {
        // encrypted cmd, need sec channel established
        return iris_send_encrypted_request(req);
    } else {
        return iris_send_plain_request(req);
    }
}

static uint8_t xor (uint8_t init, const uint8_t* buf, size_t len) {
    uint8_t crc = init;
    while (len--) {
        crc ^= *buf++;
    }
    return crc;
}

static void iris_drop_dirty_bytes(void) {
    // the header 0xFE FD
    while (fifo_lockdata_len(&uart_fifo_in) >= 2) {
        uint8_t header[2] = {0};
        fifo_read_peek(&uart_fifo_in, header, 2);
        if (header[0] != SYNC_H0 && header[0] != SYNC_SEC_H0) {
            // dirty byte, drop it
            fifo_read_lock(&uart_fifo_in, header, 1);
            continue;
        }

        // FE FD or FA FB
        if (header[0] == SYNC_H0 && header[1] != SYNC_H1) {
            // 2 dirty bytes, drop all
            fifo_read_lock(&uart_fifo_in, header, 2);
            continue;
        }
        if (header[0] == SYNC_SEC_H0 && header[1] != SYNC_SEC_H1) {
            // 2 dirty bytes, drop all
            fifo_read_lock(&uart_fifo_in, header, 2);
            continue;
        }
        // ok, have search
        break;
    }
}

static int iris_usart_plain_read(uint8_t* msg, size_t msg_buf_len) {
    // plain packet: 0xFE, 0xFD, msg_id, msg_type, N:2 bytes, <N bytes data>, crc
    uint8_t header[6];
    fifo_read_peek(&uart_fifo_in, header, sizeof(header));
    // little endian
    size_t N = header[4] + (header[5] << 8);
    if (fifo_lockdata_len(&uart_fifo_in) < PLAIN_PACKET_OVERHEAD + N) {
        LOG_WARN(MODULE, "have not received one packet");
        // not one packet
        return 0;
    }

    if (N + 4 > msg_buf_len) {
        LOG_ERROR(MODULE, "buffer size too small");
        // buffer small
        return 0;
    }

    // consume 0xfe, 0xfd
    fifo_read_lock(&uart_fifo_in, header, 2);
    fifo_read_lock(&uart_fifo_in, msg, N + 4);
    uint8_t crc = 0;
    fifo_read_lock(&uart_fifo_in, &crc, 1);
    if (crc != xor(0, msg, N + 4)) {
        // crc not match
        LOG_ERROR(MODULE, "crc not match\n");
        return 0;
    }
    LOG_HEXDUMP_DEBUG(MODULE, "reading(msg)", msg, N + 4);
    return N + 4;
}

static int iris_usart_encrypt_read(uint8_t* msg, size_t msg_buf_len) {
    // encrypted packet: 0xFE, 0xFC, N: 2 bytes, <N bytes record>, crc
    uint8_t header[4] = {0};
    fifo_read_peek(&uart_fifo_in, header, sizeof(header));
    // little endian
    int N = header[2] + (header[3] << 8);
    if (fifo_lockdata_len(&uart_fifo_in) < ENCRYPTED_PACKET_OVERHEAD + N) {
        LOG_WARN(MODULE, "have not received one packet");
        // not one packet
        return 0;
    }

    if (1 && ENABLE_IRIS_LOG) {
        uint8_t packet[256] = {0};
        fifo_read_peek(&uart_fifo_in, packet, N + ENCRYPTED_PACKET_OVERHEAD);
        LOG_HEXDUMP_DEBUG(MODULE, "encrypted packet", packet, N + ENCRYPTED_PACKET_OVERHEAD);
    }

    uint8_t record[UART_RX_BUFFER_SIZE] = {0};
    // consume 0xfe, 0xfc, N: 2bytes
    fifo_read_lock(&uart_fifo_in, header, 4);
    // consume ciphertext
    fifo_read_lock(&uart_fifo_in, record, N);
    uint8_t crc = 0;
    fifo_read_lock(&uart_fifo_in, &crc, 1);
    // crc(N | ciphertext)
    uint8_t crc2 = xor(0, &header[2], 2);
    if (crc != xor(crc2, record, N)) {
        // crc not match
        LOG_ERROR(MODULE, "crc not match\n");
        return 0;
    }
    LOG_HEXDUMP_DEBUG(MODULE, "reading(record)", record, N);

    N = iris_sec_channel_decrypt(record, N, msg, msg_buf_len);
    if (N < 0) {
        // decrypt failed
        LOG_ERROR(MODULE, "decrypt failed");
        return 0;
    }
    LOG_HEXDUMP_DEBUG(MODULE, "reading(msg)", msg, N);
    return N;
}

int iris_usart_read(uint8_t* msg, size_t msg_buf_len) {
    if (uart_tx_doing) {
        // uart tx is doing
        return 0;
    }
    // try drop dirty bytes
    iris_drop_dirty_bytes();
    if (fifo_lockdata_len(&uart_fifo_in) < SYNC_COUNT) {
        // not one packet
        return 0;
    }

    uint8_t sync[2] = {0};
    fifo_read_peek(&uart_fifo_in, sync, sizeof(sync));

    // `dirty` bytes have been dropped
    if (sync[0] == SYNC_H0 && sync[1] == SYNC_H1) {
        // plain packet
        return iris_usart_plain_read(msg, msg_buf_len);
    } else if (sync[0] == SYNC_SEC_H0 && sync[1] == SYNC_SEC_H1) {
        // encrypted packet
        return iris_usart_encrypt_read(msg, msg_buf_len);
    } else {
        // unreachable, `iris_drop_dirty_bytes` have been called
        LOG_ERROR(MODULE, "sync byte: %d", sync[1]);
        LOG_ERROR(MODULE, "invalid sync byte");
        return 0;
    }
}

static void iris_power_up(void) {
    // PE3
    GPIO_InitTypeDef gpio = {0};
    gpio.Pin = GPIO_PIN_3;
    gpio.Mode = GPIO_MODE_OUTPUT_PP;
    gpio.Pull = GPIO_PULLDOWN;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOE, &gpio);

    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, GPIO_PIN_SET);
}

static void iris_power_down(void) {
    // PE3
    GPIO_InitTypeDef gpio = {0};
    gpio.Pin = GPIO_PIN_3;
    gpio.Mode = GPIO_MODE_OUTPUT_PP;
    gpio.Pull = GPIO_PULLDOWN;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOE, &gpio);
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, GPIO_PIN_RESET);
}

void iris_open(void) {
    memset(__fifo_buf__, 0, UART_BUF_MAX_LEN);
    fifo_flush(&uart_fifo_in);
    iris_usart_init();
    iris_power_up();
    iris_sec_channel_init();
    iris_handshake_ctx_init();
}

void iris_close(void) {
    iris_usart_deinit();
    iris_power_down();
    iris_sec_channel_reset();
    iris_handshake_ctx_reset();
}
void iris_wait_power_down(bool opened) {
  if (!opened) {
    iris_close();
    return;
  }
  iris_async_send_powerdown();
  uint8_t resp_buf[256] = {0};
  iris_poll_wait(1000, resp_buf, sizeof(resp_buf));
  // no need to check response
  iris_close();
}

int iris_sec_channel_open(const uint8_t* secret, size_t secret_size) {
    return iris_sec_channel_handshake(secret, secret_size);
}

int iris_set_shared_secret(const uint8_t *secret, size_t secret_size) {
    uint8_t payload[256] = {0};
    PUT_UINT16_LE(secret_size, payload, 0);
    memcpy(payload + 2, secret, secret_size);
    iris_request_t req = {
        .id = IRIS_MSG_ID_SET_SHARED_SECRET,
        .payload_size = secret_size + 2,
        .payload = payload,
    };

    iris_send_request(&req);

    // 2. wait response
    uint8_t resp_buf[256] = {0};
    iris_poll_context_t ctx = iris_poll_wait(5000, resp_buf, sizeof(resp_buf));

    if (ctx.state == IRIS_POLL_STATE_TIMEOUT) {
        return IRIS_ERROR_TIMEOUT;
    }
    if (ctx.state != IRIS_POLL_STATE_DONE) {
        return IRIS_ERROR_INVALID_RESPONSE;
    }
    if (!iris_response_replay_is(&ctx.resp, IRIS_MSG_ID_SET_SHARED_SECRET)) {
        return IRIS_ERROR_INVALID_RESPONSE;
    }
    if (!iris_response_replay_is_ok(&ctx.resp)) {
        return IRIS_ERROR_INVALID_RESPONSE;
    }
    return IRIS_ERROR_OK;
}

iris_error_t iris_resp_parser(const uint8_t* msg, size_t msg_len, iris_response_t* resp) {
    if (msg_len < RESPONSE_HEADER_SIZE) {
        return IRIS_ERROR_INVALID_RESPONSE;
    }
    /* packet: 0xFE, 0xFD, msg_id, msg_type, N:2 bytes, <N bytes data>, crc*/
    // <0xFE, 0xFD> have been consumed, crc have been checked
    resp->id = *msg++;
    resp->type = *msg++;
    size_t N = GET_UINT16_LE(msg, 0);
    // move over N
    msg += 2;

    if (resp->type == IRIS_MSG_TYPE_NOTE) {
        // status: 1 byte
        if (N != 1) {
            return IRIS_ERROR_INVALID_MSG_LEN;
        }
        resp->note.value = *msg++;
    } else if (resp->type == IRIS_MSG_TYPE_REPLY) {
        // code: 1 byte, data: N-1 bytes
        if (N < 1) {
            return IRIS_ERROR_INVALID_MSG_LEN;
        }
        resp->reply.code = *msg++;
        resp->reply.size = N - 1;
        resp->reply.data = msg;
    } else {
        return IRIS_ERROR_INVALID_MSG_TYPE;
    }

    return IRIS_ERROR_OK;
}
iris_error_t iris_resp_ota_status_parser(const iris_response_t* resp, iris_reply_ota_status_t* ota_status) {
    if (resp->type != IRIS_MSG_TYPE_REPLY) {
        return IRIS_ERROR_INVALID_MSG_TYPE;
    }
    if (resp->reply.code != IRIS_RESPONSE_CODE_OK) {
        return IRIS_ERROR_INVALID_RESPONSE;
    }
    if (resp->reply.size != 5) {
        return IRIS_ERROR_INVALID_MSG_LEN;
    }
    const uint8_t* p = resp->reply.data;
    ota_status->status = *p++;
    ota_status->next_pkt_id = GET_UINT32_LE(p, 0);
    return IRIS_ERROR_OK;
}

void iris_async_send_powerdown(void) {
    iris_request_t req = {
        .id = IRIS_MSG_ID_DEVICE_POWERDOWN,
        .payload_size = 0,
        .payload = NULL,
    };
    iris_send_request(&req);
}

void iris_async_send_reboot(void) {
    iris_request_t req = {
        .id = IRIS_MSG_ID_DEVICE_REBOOT,
        .payload_size = 0,
        .payload = NULL,
    };
    iris_send_request(&req);
}

iris_error_t iris_async_get_version(void) {
    iris_request_t req = {
        .id = IRIS_MSG_ID_DEVICE_GET_VERSION,
        .payload_size = 0,
        .payload = NULL,
    };
    return iris_send_request(&req);
}

iris_error_t iris_async_start_ota(void) {
    iris_request_t req = {
        .id = IRIS_MSG_ID_PACKET_OTA_START,
        .payload_size = 0,
        .payload = NULL,
    };

    return iris_send_request(&req);
}

iris_error_t iris_async_stop_ota(void) {
    iris_request_t req = {
        .id = IRIS_MSG_ID_PACKET_OTA_STOP,
        .payload_size = 0,
        .payload = NULL,
    };

    return iris_send_request(&req);
}

iris_error_t iris_async_send_ota_header(const iris_packet_ota_header_t* header) {
    uint8_t msg[4 + 42] = {0};
    uint8_t* p = msg + 4;
    // build ota header request
    // `fw_size`, 4 bytes
    PUT_UINT32_LE(header->fw_size, p, 0);
    p += 4;
    // `fw_pkt_count`, 4 bytes
    PUT_UINT32_LE(header->fw_pkt_count, p, 0);
    p += 4;
    // `fw_pkt_size`, 2 bytes
    PUT_UINT32_LE(header->fw_pkt_size, p, 0);
    p += 2;
    // `fw_md5_sum`
    memcpy(p, header->fw_md5_sum, 32);
    p += 32;

    p = msg + 4;
    iris_request_t req = {
        .id = IRIS_MSG_ID_PACKET_OTA_HEADER,
        .payload_size = 42,
        .payload = p,
    };

    return iris_send_request(&req);
}

iris_error_t iris_async_send_ota_packet(const iris_packet_ota_firmware_t* pkt) {
    iris_resp_cache_reset();
    LOG_DEBUG(MODULE, "sending ota packet");
    LOG_DEBUG(MODULE, "packet id: %d", pkt->pkt_id);
    LOG_DEBUG(MODULE, "packet size: %#4x", pkt->pkt_size);

    if (pkt->pkt_size == 0 || pkt->pkt_size > UART_TX_BUFFER_SIZE - 7) {
        return IRIS_ERROR_INVALID_MSG_LEN;
    }

    // build ota packet request in send buffer
    uint8_t* p = send_buf;
    // `sync` 2 bytes
    *p++ = SYNC_H0;
    *p++ = SYNC_H1;
    // `msg_id` 1 byte
    *p++ = IRIS_MSG_ID_PACKET_OTA_FIRMWARE;
    // `msg_type` 1 byte
    *p++ = IRIS_MSG_TYPE_REQUEST;
    // `N` 2 bytes, pkt_id: 4, pkt_size: 2
    PUT_UINT16_LE(pkt->pkt_size + 6, p, 0);
    p += 2;

    // payload: `iris_packet_ota_firmware_t`
    // `pkt_id` 4 bytes
    PUT_UINT32_LE(pkt->pkt_id, p, 0);
    p += 4;
    // `pkt_size` 2 bytes
    PUT_UINT16_LE(pkt->pkt_size, p, 0);
    p += 2;
    // `data` pkt->pkt_size bytes
    memcpy(p, pkt->data, pkt->pkt_size);
    p += pkt->pkt_size;

    // `crc` 1 byte
    *p++ = xor(0, send_buf + 2, p - send_buf - 2);

    HAL_UART_Transmit_DMA(&uart, send_buf, p - send_buf);
    return IRIS_ERROR_OK;
}
iris_error_t iris_async_send_ota_status(void) {
    iris_request_t req = {
        .id = IRIS_MSG_ID_PACKET_OTA_STATUS,
        .payload_size = 0,
        .payload = NULL,
    };

    return iris_send_request(&req);
}

void iris_poll_context_init(
    iris_poll_context_t* ctx, uint32_t timeout, uint8_t* resp_buf, size_t resp_buf_len
) {
    memset(ctx, 0, sizeof(iris_poll_context_t));
    ctx->operation = IRIS_MSG_ID_NONE;
    ctx->state = IRIS_POLL_STATE_PENDDING;
    ctx->until = HAL_GetTick() + timeout;
    ctx->resp_buf = resp_buf;
    ctx->resp_buf_len = resp_buf_len;
}

void iris_poll_context_set_timeout(iris_poll_context_t* ctx, uint32_t timeout) {
    ctx->until = HAL_GetTick() + timeout;
}

void iris_async_response_poll(iris_poll_context_t* ctx) {
    // not pending, no need to poll
    if (ctx->state != IRIS_POLL_STATE_PENDDING) {
        return;
    }

    // pending
    // check timeout
    if (HAL_GetTick() > ctx->until) {
        if (ctx->operation == IRIS_MSG_ID_WAIT) {
            ctx->state = IRIS_POLL_STATE_DONE;
            return;
        }
        ctx->state = IRIS_POLL_STATE_TIMEOUT;
        return;
    }

    // not have response yet
    ctx->resp_raw_size = iris_usart_read(ctx->resp_buf, ctx->resp_buf_len);
    if (ctx->resp_raw_size <= 0) {
        return;
    }
    // parse response
    if (iris_resp_parser(ctx->resp_buf, ctx->resp_raw_size, &ctx->resp) == IRIS_ERROR_OK) {
        ctx->state = IRIS_POLL_STATE_DONE;
    } else {
        ctx->state = IRIS_POLL_STATE_ERROR;
    }
}

iris_poll_context_t iris_poll_wait(uint32_t timeout, uint8_t* resp_buf, size_t resp_buf_len) {
    iris_poll_context_t ctx;
    iris_poll_context_init(&ctx, timeout, resp_buf, resp_buf_len);
    while (ctx.state == IRIS_POLL_STATE_PENDDING) {
        iris_async_response_poll(&ctx);
        hal_delay(10);
    }
    return ctx;
}

int iris_version_parse(const char* s, iris_version_t* ver) {
    uint32_t major, minor, patch;
    if (memcmp(s, "None", 4) == 0) {
        ver->major = 0;
        ver->minor = 0;
        ver->patch = 0;
        return IRIS_ERROR_OK;
    }
    int ret = sscanf(s, "%lu.%lu.%lu", &major, &minor, &patch);
    if (ret != 3) {
        return IRIS_ERROR_INVALID_MSG_LEN;
    }
    ver->major = major;
    ver->minor = minor;
    ver->patch = patch;
    ver->build = 0;
    return IRIS_ERROR_OK;
}

void iris_version_format(iris_version_t version, char str[33]) {
    sprintf(str, "%ld.%ld.%ld", version.major, version.minor, version.patch);
}

int iris_version_compare(const iris_version_t* a, const iris_version_t* b) {
    if (a->major < b->major) return -1;
    if (a->major > b->major) return 1;

    if (a->minor < b->minor) return -1;
    if (a->minor > b->minor) return 1;

    if (a->patch < b->patch) return -1;
    if (a->patch > b->patch) return 1;

    // unused `build` field
    return 0;
}

void iris_print_header(const iris_header_t *const header) {
  (void)header;
  LOG_DEBUG(MODULE, "magic: 0x%08x", header->magic);
  LOG_DEBUG(MODULE, "header_size: 0x%08x", header->header_size);
  LOG_DEBUG(MODULE, "code_size: 0x%08x", header->code_size);
  LOG_DEBUG(MODULE, "version: %d %d %d %d",
    header->version.build,
    header->version.patch,
    header->version.minor,
    header->version.major
  );
  LOG_DEBUG(MODULE, "required_sig_count: 0x%02x", header->required_sig_count);
  LOG_HEXDUMP_DEBUG(MODULE, "digest", header->digest, sizeof(header->digest));
  LOG_DEBUG(MODULE, "sigmask: 0x%08x", header->sig_mask);
  uart_log_flush();
  for (int i = 0; i < sizeof(header->sig_mask)*8; i++) {
    bool present = (header->sig_mask & (1 << i)) != 0;
    if (present) {
      char label[17] = {0};
      snprintf(label, sizeof(label), "[%d] sig", i);
      LOG_HEXDUMP_DEBUG(MODULE, label, header->sigs[i], IMAGE_SIG_SIZE);
      uart_log_flush();
    }
  }
}

void iris_header_hash_update(SHA256_CTX *ctx, const iris_header_t *const header) {
  // `header`: `magic` `version` `header_size` `code_size` `required_sig_count` is be protected
  size_t protected = offsetof(iris_header_t, digest);
  sha256_Update(ctx, (const uint8_t *)header, protected);

  uint8_t filled[sizeof(iris_header_t) - protected];
  memset(filled, 0x00, sizeof(filled));
  sha256_Update(ctx, filled, sizeof(filled));
}

secbool __wur iris_header_verify(const iris_header_t* const header) {
  if (header->required_sig_count < IMAGE_MIN_REQUIRED_SIG_COUNT) {
    if (PRODUCTION) {
      return secfalse;
    } else {
      LOG_DEBUG(MODULE, "Not config `required_sig_count` in develop mode, skip...");
      return sectrue;
    }
  }
  int count = 0;
  for (int i = 0; i < sizeof(header->sig_mask)*8; i++) {
    bool present = (header->sig_mask & (1 << i)) != 0;
    if (present) {
      if (i >= KEY_N) {
        LOG_DEBUG(MODULE, "pubkey index %d is out of range", i);
        return secfalse;
      }
      LOG_DEBUG(MODULE, "use %d pubkey verify sig", i);
      LOG_HEXDUMP_DEBUG(MODULE, "pubkey", KEYS[i], KEY_SIZE);
      LOG_HEXDUMP_DEBUG(MODULE, "sig", header->sigs[i], IMAGE_SIG_SIZE);
      uart_log_flush();
      if (0 != ed25519_sign_open(header->digest, sizeof(header->digest), KEYS[i], header->sigs[i])) {
        LOG_ERROR(MODULE, "verify sig %d failed", i);
        return secfalse;
      }
      LOG_DEBUG(MODULE, "verify sig %d success", i);
      count++;
    }
  }
  if (count >= header->required_sig_count) {
    return sectrue;
  } else if (PRODUCTION){
    LOG_DEBUG(MODULE, "sign count %d < required %d in develop mode, skip...", count, header->required_sig_count);
    return sectrue;
  }
  return secfalse;
}
