#include "iris.h"

#include STM32_HAL_H

#include <stdio.h>

#include "stm32h7xx_hal_gpio.h"
#include "stm32h7xx_hal_rcc.h"
#include "stm32h7xx_hal_uart.h"

#include "trans_fifo.h"
#include "common.h"
#include "irq.h"

#define IRIS_LOG_ENABLED 0

#define USART_TIMEOUT 1000
#define UART_BUF_MAX_LEN 256
#define PACKET_H0 0xFE
#define PACKET_H1 0xFD

/**
 * the packet overhead length
 * packet: 0xFE, 0xFD, msg_id, msg_type, N:2 bytes, <N bytes data>, crc
 */
#define PACKET_OVERHEAD (6+1)

static UART_HandleTypeDef uart;
static UART_HandleTypeDef *huart = &uart;
static uint8_t xor(const uint8_t *buf, size_t len);

static uint8_t __fifo_buf__[UART_BUF_MAX_LEN];
static trans_fifo uart_fifo_in =  {
    .p_buf = __fifo_buf__,
    .buf_size = UART_BUF_MAX_LEN,
    .over_pre = false,
    .read_pos = 0,
    .write_pos = 0,
    .lock_pos = 0,
};

#if IRIS_LOG_ENABLED
#define LOG printf

void log_data(const uint8_t* data, size_t data_size) {
  #define __FRAME_LINE__ 16
  uint8_t count = 0;
  while (data_size--) {
    count++;
    LOG("%02x ", *data++);
    if (count == __FRAME_LINE__) {
      LOG("\n");
      count = 0;
    }
  }
  LOG("\n");
}
#else
#define LOG(...)
#define log_data(...)
#endif

void iris_usart_init(void) {
    /**
     * USART2
     * TX : PA2
     * RX : PD6
     */

    __HAL_RCC_USART2_FORCE_RESET();
    __HAL_RCC_USART2_RELEASE_RESET();

    __HAL_RCC_USART2_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();

    // USART2: PA2(TX), PD6(RX)
    GPIO_InitTypeDef gpio = {0};
    gpio.Pin = GPIO_PIN_2;
    gpio.Mode = GPIO_MODE_AF_PP;
    gpio.Pull = GPIO_PULLUP;
    gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    gpio.Alternate = GPIO_AF7_USART2;
    HAL_GPIO_Init(GPIOA, &gpio);

    gpio.Pin = GPIO_PIN_6;
    HAL_GPIO_Init(GPIOD, &gpio);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_6, GPIO_PIN_SET);

    huart->Instance = USART2;
    huart->Init.BaudRate = 115200;
    huart->Init.WordLength = UART_WORDLENGTH_8B;
    huart->Init.StopBits = UART_STOPBITS_1;
    huart->Init.Parity = UART_PARITY_NONE;
    huart->Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart->Init.Mode = UART_MODE_TX_RX;
    huart->Init.OverSampling = UART_OVERSAMPLING_16;
    huart->Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    huart->Init.ClockPrescaler = UART_PRESCALER_DIV1;

    if (HAL_UART_Init(huart) != HAL_OK) {
        ensure(secfalse, "uart init failed");
    }

    NVIC_SetPriority(USART2_IRQn, IRQ_PRI_UART);
    HAL_NVIC_EnableIRQ(USART2_IRQn);

    __HAL_UART_ENABLE_IT(huart, UART_IT_RXNE);
}

void iris_usart_deinit(void) {
    HAL_NVIC_DisableIRQ(USART2_IRQn);
    HAL_UART_DeInit(huart);

    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2);
    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_6);
}

void iris_usart_send(const uint8_t *msg, size_t len) {
    LOG("iris send: ");
    log_data(msg, len);
    uint8_t crc = xor(msg, len);
    uint8_t header[2] = {PACKET_H0, PACKET_H1};
    HAL_UART_Transmit(huart, header, 2, USART_TIMEOUT);
    HAL_UART_Transmit(huart, msg, len, USART_TIMEOUT);
    HAL_UART_Transmit(huart, &crc, 1, USART_TIMEOUT);
    LOG("iris send done \n");
}

static uint8_t xor(const uint8_t *buf, size_t len) {
    uint8_t crc = 0;
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
        if ( header[0] != PACKET_H0) {
            // dirty byte, drop it
            fifo_read_lock(&uart_fifo_in, header, 1);
            continue;
        }

        // 0xFE <not 0xFD>
        if (header[1] != PACKET_H1) {
            // 2 dirty bytes, drop all
            fifo_read_lock(&uart_fifo_in, header, 2);
            continue;
        }
        // ok, have search
        break;
    }
}


size_t iris_usart_read(uint8_t *msg, size_t msg_buf_len) {
    // try drop dirty bytes
    iris_drop_dirty_bytes();
    // packet: 0xFE, 0xFD, msg_id, msg_type, N:2 bytes, <N bytes data>, crc
    if (fifo_lockdata_len(&uart_fifo_in) < PACKET_OVERHEAD) {
        // not one packet
        return 0;
    }

    uint8_t header[6];
    fifo_read_peek(&uart_fifo_in, header, sizeof(header));
    // little endian
    size_t N = header[4] + (header[5] << 8);
    if (fifo_lockdata_len(&uart_fifo_in) < PACKET_OVERHEAD + N) {
        // not one packet
        return 0;
    }

    if (N+4 > msg_buf_len) {
        // buffer small
        return 0;
    }

    // consume 0xfe, 0xfd
    fifo_read_lock(&uart_fifo_in, header, 2);
    fifo_read_lock(&uart_fifo_in, msg, N + 4);
    uint8_t crc = 0;
    fifo_read_lock(&uart_fifo_in, &crc, 1);
    if (crc != xor(msg, N+4)) {
        // crc not match
        LOG("crc not match\n");
        return 0;
    }
    return N+4;
}


static inline void usart_recv_packet(void) {
    uint8_t byte = (uint8_t)(huart->Instance->RDR);
    fifo_put_no_overflow(&uart_fifo_in, byte);
    fifo_lockpos_set(&uart_fifo_in);
}


void USART2_IRQHandler(void) {
    if (__HAL_UART_GET_FLAG(huart, UART_FLAG_ORE) != 0) {
        __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_OREF);
    }
    if (__HAL_UART_GET_FLAG(huart, UART_FLAG_RXNE) != 0) {
        // receive packget
        usart_recv_packet();
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
}

void iris_close(void) {
    iris_usart_deinit();
    iris_power_down();
}
