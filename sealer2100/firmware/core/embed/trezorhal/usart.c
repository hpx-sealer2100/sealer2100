#include STM32_HAL_H

#include <stdio.h>
#include <string.h>

#include "ble.h"
#include "common.h"
#include "irq.h"
#include "usart.h"

#define BLE_USART_LOG_ENABLED 0

#define USART_TIMEOUT 0x100000
#define PACKET_H0 0xA5
#define PACKET_H1 0x5A

// package: 0xA5, 0x5A, len:2 bytes, data, xor
#define PACKET_OVERHEAD (4+1)

UART_HandleTypeDef uart;
UART_HandleTypeDef *huart = &uart;

static uint8_t __uart_data_in__[UART_BUF_MAX_LEN];

static trans_fifo uart_fifo_in = {.p_buf = __uart_data_in__,
                           .buf_size = UART_BUF_MAX_LEN,
                           .over_pre = false,
                           .read_pos = 0,
                           .write_pos = 0,
                           .lock_pos = 0};

#if BLE_USART_LOG_ENABLED
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

void ble_usart_init(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  HAL_SYSCFG_AnalogSwitchConfig(SYSCFG_SWITCH_PA0, SYSCFG_SWITCH_PA0_CLOSE);
  HAL_SYSCFG_AnalogSwitchConfig(SYSCFG_SWITCH_PA1, SYSCFG_SWITCH_PA1_CLOSE);

  __HAL_RCC_UART4_FORCE_RESET();
  __HAL_RCC_UART4_RELEASE_RESET();

  __HAL_RCC_UART4_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  // UART4: PA0_C(TX), PA1_C(RX)
  GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF8_UART4;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  huart->Instance = UART4;
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

  NVIC_SetPriority(UART4_IRQn, IRQ_PRI_UART);
  HAL_NVIC_EnableIRQ(UART4_IRQn);

  __HAL_UART_ENABLE_IT(huart, UART_IT_RXNE);
}

void ble_usart_deinit(void) {

  HAL_NVIC_DisableIRQ(UART4_IRQn);
  if (HAL_UART_DeInit(huart) != HAL_OK) {
    ensure(secfalse, "uart deinit failed");
  }

  HAL_GPIO_DeInit(GPIOA, GPIO_PIN_0 | GPIO_PIN_1);
}
void ble_usart_send_byte(uint8_t data) {
  HAL_UART_Transmit(huart, &data, 1, 0xFFFF);
}

void ble_usart_send(uint8_t *buf, uint32_t len) {
  LOG("ble usart send: ");
  log_data(buf, len);
  HAL_UART_Transmit(huart, buf, len, 0xFFFF);
}

bool ble_read_byte(uint8_t *buf) {
  if (HAL_UART_Receive(huart, buf, 1, 100) == HAL_OK) {
    return true;
  }
  return false;
}


void ble_usart_irq_disable(void) { HAL_NVIC_DisableIRQ(UART4_IRQn); }

static uint8_t calXor(uint8_t *buf, uint32_t len) {
  uint8_t crc = 0;
  while(len--) {
    crc ^= *buf++;
  }
  return crc;
}

static inline size_t ble_usart_packet_length(uint8_t header[4]) {
  size_t N = (header[2] << 8) + header[3];
  return PACKET_OVERHEAD + N - 1;
}

static void ble_usart_drop_dirty_bytes(void) {
    // the header 0xA5 0x5A
    while (fifo_lockdata_len(&uart_fifo_in) >= 2) {
        uint8_t header[2] = {0};
        fifo_read_peek(&uart_fifo_in, header, 2);
        if ( header[0] != PACKET_H0) {
            // dirty byte, drop it
            fifo_read_lock(&uart_fifo_in, header, 1);
            continue;
        }

        // 0xA5 <not 0x5A>
        if (header[1] != PACKET_H1) {
            // 2 dirty bytes, drop all
            fifo_read_lock(&uart_fifo_in, header, 2);
            continue;
        }
        // ok, have search
        break;
    }
}

// package: 0xA5, 0x5A, len, data, xor
secbool ble_usart_can_read(void) {
  // try drop dirty bytes
  ble_usart_drop_dirty_bytes();
  if (fifo_lockdata_len(&uart_fifo_in) < PACKET_OVERHEAD) {
    return secfalse;
  }

  uint8_t header[4] = {0};
  fifo_read_peek(&uart_fifo_in, header, sizeof(header));

  if (fifo_lockdata_len(&uart_fifo_in) < ble_usart_packet_length(header)) {
    // not one packet
    return secfalse;
  }

  return sectrue;
}

uint32_t ble_usart_read(uint8_t *buf, uint32_t lenth) {
  // try drop dirty bytes
  ble_usart_drop_dirty_bytes();
  if (fifo_lockdata_len(&uart_fifo_in) < PACKET_OVERHEAD) {
    return 0;
  }

  uint8_t header[4] = {0};
  fifo_read_peek(&uart_fifo_in, header, sizeof(header));

  // <len>: 2 bytes, big endian, the length of data and xor
  size_t N = ble_usart_packet_length(header);

  // OVERHEAD contain the xor
  if (fifo_lockdata_len(&uart_fifo_in) < N) {
    // not one packet
    LOG("not one packet\n");
    return 0;
  }

  if (lenth < N) {
    return 0;
  }

  // consume the packet
  fifo_read_lock(&uart_fifo_in, buf, N);
  LOG("ble usart read: ");
  log_data(buf, N);

  uint8_t crc = buf[N-1];
  if (crc != calXor(buf, N-1)) {
    // crc not match
    LOG("crc not match\n");
    return 0;
  }
  return N;
}

// package: 0xA5, 0x5A, len, data, xor
static void usart_rev_package(void) {
  uint8_t byte = (uint8_t)(huart->Instance->RDR);
  fifo_put_no_overflow(&uart_fifo_in, byte);
  fifo_lockpos_set(&uart_fifo_in);
}

void UART4_IRQHandler(void) {
  if (__HAL_UART_GET_FLAG(huart, UART_FLAG_ORE) != 0) {
    __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_OREF);
  }
  if (__HAL_UART_GET_FLAG(huart, UART_FLAG_RXNE) != 0) {
    usart_rev_package();
  }
}
