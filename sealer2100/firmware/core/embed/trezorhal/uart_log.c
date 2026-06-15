#include "uart_log.h"

#include <stdint.h>
#include <string.h>

#if !PRODUCTION
#include "stm32h7xx_hal.h"
#include "stm32h7xx_hal_uart.h"
#include "stm32h7xx_hal_dma.h"
#include "stm32h7xx_hal_gpio.h"

#include "common.h"
#include "irq.h"
#include "sram.h"
#include "trans_fifo.h"

#define LOG_BUF_MAX_LEN (4 * 1024)
#define LOG_ITEM_MAX_SIZE 0x200 // 512 bytes

static uint8_t *tx_buf = (uint8_t*)SRAM_LOG_ADDRESS;
static uint8_t __fifo_buf__[LOG_BUF_MAX_LEN];
static trans_fifo log_fifo = {
    .p_buf = __fifo_buf__,
    .buf_size = LOG_BUF_MAX_LEN,
    .over_pre = false,
    .read_pos = 0,
    .write_pos = 0,
    .lock_pos = 0,
};

static UART_HandleTypeDef uart;
static DMA_HandleTypeDef hdma_tx;

static void uart_log_consume_items(void);

static void uart_log_tx_complete(UART_HandleTypeDef* huart) {
  uart_log_consume_items();
}

static void uart_log_error(UART_HandleTypeDef* huart) {
  volatile uint32_t err = HAL_DMA_GetError(&hdma_tx);
  (void)err;
}

int uart_log_init(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_USART1_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  __HAL_RCC_DMA1_CLK_ENABLE();

  // log only use tx
  GPIO_InitStruct.Pin = GPIO_PIN_14; // | GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF4_USART1;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  uart.Instance = USART1;
  uart.Init.BaudRate = 230400;
  // uart.Init.BaudRate = 460800; // no problem with this baud rate
  uart.Init.WordLength = UART_WORDLENGTH_8B;
  uart.Init.StopBits = UART_STOPBITS_1;
  uart.Init.Parity = UART_PARITY_NONE;
  uart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  // log only use tx
  uart.Init.Mode = UART_MODE_TX; // UART_MODE_TX_RX;
  uart.Init.OverSampling = UART_OVERSAMPLING_16;
  uart.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  uart.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  uart.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  HAL_UART_DeInit(&uart);
  HAL_UART_Init(&uart);
  HAL_UART_RegisterCallback(&uart, HAL_UART_TX_COMPLETE_CB_ID, uart_log_tx_complete);
  HAL_UART_RegisterCallback(&uart, HAL_UART_ERROR_CB_ID, uart_log_error);
  NVIC_SetPriority(USART1_IRQn, IRQ_PRI_UART);
  HAL_NVIC_EnableIRQ(USART1_IRQn);

  hdma_tx.Instance = DMA1_Stream0;
  hdma_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
  hdma_tx.Init.Request = DMA_REQUEST_USART1_TX;
  hdma_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
  hdma_tx.Init.PeriphInc = DMA_PINC_DISABLE;
  hdma_tx.Init.MemInc = DMA_MINC_ENABLE;
  hdma_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  hdma_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
  hdma_tx.Init.Mode = DMA_NORMAL;
  hdma_tx.Init.Priority = DMA_PRIORITY_LOW;

  HAL_DMA_DeInit(&hdma_tx);

  HAL_DMA_Init(&hdma_tx);
  NVIC_SetPriority(DMA1_Stream0_IRQn, IRQ_PRI_UART);
  HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);

  __HAL_LINKDMA(&uart, hdmatx, hdma_tx);

  memset(tx_buf, 0, LOG_ITEM_MAX_SIZE);
  fifo_flush(&log_fifo);
  return 0;
}

void uart_log_flush(void) {
  while (fifo_lockdata_len(&log_fifo) > 0) {
    HAL_Delay(1);
  }
  HAL_Delay(1);
  while (hdma_tx.State != HAL_DMA_STATE_READY) {
    HAL_Delay(1);
  }

}

void uart_log_deinit(void) {
  // aboart uart tx
  HAL_UART_AbortTransmit(&uart);
  HAL_Delay(1);
  // disable dma irq
  HAL_NVIC_DisableIRQ(DMA1_Stream0_IRQn);

  // disable uart irq
  HAL_NVIC_DisableIRQ(USART1_IRQn);

  // deinit dma
  HAL_DMA_DeInit(&hdma_tx);

  // deinit uart
  HAL_UART_DeInit(&uart);
}


void DMA1_Stream0_IRQHandler(void) {
    HAL_DMA_IRQHandler(&hdma_tx);
}

void USART1_IRQHandler(void) {
    HAL_UART_IRQHandler(&uart);
}

static void uart_log_consume_items(void) {
  volatile uint16_t len = 0;

  if (fifo_lockdata_len(&log_fifo) < sizeof(len)) {
    return;
  }

  fifo_read_peek(&log_fifo, (uint8_t*)&len, sizeof(len));

  // not enough data in log_fifo
  // L|V format, L: 2bytes LE format, V: `len` bytes
  if (len + 2 > fifo_lockdata_len(&log_fifo)) {
    return;
  }
  // read out `len`
  fifo_read_lock(&log_fifo, (uint8_t*)&len, sizeof(len));
  // read out `len` bytes
  fifo_read_lock(&log_fifo, tx_buf, len);
  // start dma transfer
  HAL_UART_Transmit_DMA(&uart, tx_buf, len);
}

size_t uart_log_write(const char *str, size_t len) {
  // HAL_UART_Transmit(&uart, (uint8_t *)str, len, HAL_MAX_DELAY);
  // return len;
  // does dma consume all the data in log_fifo
  // put L|V format to log_fifo
  // L: 2bytes LE format
  size_t N = len;
  while (N) {
    size_t L = MIN(N, LOG_ITEM_MAX_SIZE);
    size_t freed = fifo_free_len(&log_fifo);
    size_t lock = fifo_lockdata_len(&log_fifo);
    if (freed < L + 2) {
      // wait for log_fifo to be empty
      uart_log_flush();
    }
    fifo_put_no_overflow(&log_fifo, (uint8_t)(L & 0xFF));
    fifo_put_no_overflow(&log_fifo, (uint8_t)(L >> 8));
    fifo_write_no_overflow(&log_fifo, (uint8_t *)str, L);

    if (hdma_tx.State == HAL_DMA_STATE_READY && lock == 0) {
      uart_log_consume_items();
    }
    N -= L;
    str += L;
  }
  return len;
}

// in `firmware` printf function implement in micropython
// for micropython
size_t uart_print(const char *str, size_t len) {
  return uart_log_write(str, len);
}

// for printf
// #ifdef __GNUC__
// #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
// #else
// #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
// #endif /* __GNUC__ */
// PUTCHAR_PROTOTYPE
// {
//     // 将字符发送到UART
//     HAL_UART_Transmit(&uart, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
//     return ch;
// }
// libc-nano lib use `_write` to print
int _write(int file, char *ptr, int len) {
  (void)file;
  return uart_log_write(ptr, len);
}

#else
int uart_log_init(void) {
  return 0;
}
void uart_log_deinit(void) {
}

void uart_log_flush(void) {
}

// in `firmware` printf function implement in micropython
// for micropython
size_t uart_print(const char *str, size_t len) {
  return len;
}
#endif // endif PRODUCTION
