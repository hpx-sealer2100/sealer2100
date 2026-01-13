#include "uart_log.h"

#if !PRODUCTION
#include "stm32h7xx_hal_uart.h"
static UART_HandleTypeDef uart;

#if USE_DUNAN_BOARD
int uart_log_init(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_USART1_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  // log only use tx
  GPIO_InitStruct.Pin = GPIO_PIN_14; // | GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF4_USART1;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  uart.Instance = USART1;
  uart.Init.BaudRate = 230400;
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

  return HAL_UART_Init(&uart);
}
#elif USE_ZIAN_BOARD
void log_usart_init(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};


  __HAL_RCC_USART1_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  GPIO_InitStruct.Pin = GPIO_PIN_9 | GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  uart.Instance = USART1;
  uart.Init.BaudRate = 115200;
  uart.Init.WordLength = UART_WORDLENGTH_8B;
  uart.Init.StopBits = UART_STOPBITS_1;
  uart.Init.Parity = UART_PARITY_NONE;
  uart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  uart.Init.Mode = UART_MODE_TX_RX;
  uart.Init.OverSampling = UART_OVERSAMPLING_16;
  uart.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  uart.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  uart.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

  if (HAL_UART_Init(&uart) != HAL_OK) {
    ensure(secfalse, "uart init failed");
  }

  NVIC_SetPriority(USART1_IRQn, IRQ_PRI_UART);
  HAL_NVIC_EnableIRQ(USART1_IRQn);

  __HAL_UART_ENABLE_IT(huart, UART_IT_RXFNE);
}
#else
void log_usart_init(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};


  __HAL_RCC_USART1_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  GPIO_InitStruct.Pin = GPIO_PIN_9 | GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  uart.Instance = USART1;
  uart.Init.BaudRate = 115200;
  uart.Init.WordLength = UART_WORDLENGTH_8B;
  uart.Init.StopBits = UART_STOPBITS_1;
  uart.Init.Parity = UART_PARITY_NONE;
  uart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  uart.Init.Mode = UART_MODE_TX_RX;
  uart.Init.OverSampling = UART_OVERSAMPLING_16;
  uart.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  uart.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  uart.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

  if (HAL_UART_Init(&huart) != HAL_OK) {
    ensure(secfalse, "uart init failed");
  }

  NVIC_SetPriority(UART8_IRQn, IRQ_PRI_UART);
  HAL_NVIC_EnableIRQ(UART8_IRQn);

  __HAL_UART_ENABLE_IT(huart, UART_IT_RXFNE);
}
#endif

// in `firmware` printf function implement in micropython
// for micropython
size_t uart_print(const char *str, size_t len) {
  HAL_UART_Transmit(&uart, (uint8_t *)str, len, 0xFFFF);
  return len;
}

// for printf
#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
PUTCHAR_PROTOTYPE
{
    // 将字符发送到UART
    HAL_UART_Transmit(&uart, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
    return ch;
}
// libc-nano lib use `_write` to print
int _write(int file, char *ptr, int len) {
    HAL_UART_Transmit(&uart, (uint8_t *)ptr, len, HAL_MAX_DELAY);
    return len;
}

#else
int uart_log_init(void) {
  return 0;
}
size_t uart_print(const char *str, size_t len) {(void)str;return len;}

#endif // endif PRODUCTION
