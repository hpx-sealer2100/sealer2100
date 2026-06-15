#include "uart_log.h"
#include STM32_HAL_H
#include "stm32h7xx_hal_gpio.h"
#include "stm32h7xx_hal_spi.h"
#include <stdio.h>
#include <string.h>
#include "se_spi.h"
#include "sec_trans.h"
#include "device.h"
#include "log.h"

#define MODULE "SE"

#define ENABLE_SE_LOG 0

#if !ENABLE_SE_LOG
// disable log
#undef LOG_DEBUG
#undef LOG_HEXDUMP_DEBUG
#define LOG_DEBUG(module, fmt, ...)                log_dummy(module, fmt, ##__VA_ARGS__)
#define LOG_HEXDUMP_DEBUG(module, label, buf, len) log_dummy(module, label, buf, len)
#define log_frame(frame) log_dummy(MODULE, frame)
#else
static void log_frame(uint8_t frame[SEC_MAX_FRAME_SIZE]) {
  uint8_t fctr = frame[0];
  (void)fctr;
  size_t len = frame[1];
  len <<= 8;
  len |= frame[2];

  size_t frame_size = len + 5;
  (void)frame_size;
  uint8_t *p = frame + 3;
  if (frame_size > SEC_MAX_FRAME_SIZE) {
    LOG_DEBUG(MODULE, "SE spi maybe read failed\n");
    return;
  }
  LOG_DEBUG(MODULE, "frame: %02x, len: %d(%02x%02x), crc: %02x%02x", fctr, len, frame[1], frame[2],
             frame[frame_size - 2], frame[frame_size - 1]);
  LOG_HEXDUMP_DEBUG(MODULE, "payload", p, len);
  uart_log_flush();
}

#endif

static SPI_HandleTypeDef hspi5 = {0};
#define SE_TRANS_TIMEOUT 500
int se_spi_init(void) {
  GPIO_InitTypeDef gpio = {0};

  /* Peripheral clock enable */
  __HAL_RCC_SPI5_CLK_ENABLE();
  __HAL_RCC_SPI5_FORCE_RESET();
  __HAL_RCC_SPI5_RELEASE_RESET();

  // enable gpio clock
  SPI_GPIO_CLK_ENABLE();

  // CLK
  gpio.Pin = SPI_CLK_GPIO_PIN;
  gpio.Mode = GPIO_MODE_AF_PP;
  gpio.Pull = GPIO_PULLUP;
  gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  gpio.Alternate = GPIO_AF5_SPI5;
  HAL_GPIO_Init(SPI_CLK_GPIO_PORT, &gpio);

  // NSS
  gpio.Pin = SPI_NSS_GPIO_PIN;
  gpio.Mode = GPIO_MODE_AF_PP;
  gpio.Pull = GPIO_PULLUP;
  gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  gpio.Alternate = GPIO_AF5_SPI5;
  HAL_GPIO_Init(SPI_NSS_GPIO_PORT, &gpio);

  // MOSI
  gpio.Pin = SPI_MOSI_GPIO_PIN;
  gpio.Mode = GPIO_MODE_AF_PP;
  gpio.Pull = GPIO_PULLUP;
  gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  gpio.Alternate = GPIO_AF5_SPI5;
  HAL_GPIO_Init(SPI_MOSI_GPIO_PORT, &gpio);

  // MISO
  gpio.Pin = SPI_MISO_GPIO_PIN;
  gpio.Mode = GPIO_MODE_AF_PP;
  gpio.Pull = GPIO_NOPULL;
  gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  gpio.Alternate = GPIO_AF5_SPI5;
  HAL_GPIO_Init(SPI_MISO_GPIO_PORT, &gpio);

  gpio.Pin = SE_COMBUS_GPIO_PIN;
  gpio.Mode = GPIO_MODE_INPUT;
  gpio.Pull = GPIO_NOPULL;
  gpio.Speed = GPIO_SPEED_FREQ_HIGH;
  gpio.Alternate = 0;
  HAL_GPIO_Init(SE_COMBUS_GPIO_PORT, &gpio);


  /* SPI5 parameter configuration*/
  hspi5.Instance = SPI5;
  hspi5.Init.Mode = SPI_MODE_MASTER;
  hspi5.Init.Direction = SPI_DIRECTION_2LINES;
  hspi5.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi5.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi5.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi5.Init.NSS = SPI_NSS_HARD_OUTPUT;
  hspi5.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128;
  hspi5.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi5.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi5.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi5.Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
  hspi5.Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
  hspi5.Init.IOSwap = SPI_IO_SWAP_DISABLE;
  hspi5.Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_08CYCLE;
  HAL_SPI_DeInit(&hspi5);
  if (HAL_SPI_Init(&hspi5) != HAL_OK) {
    return -1;
  }

  return 0;
}


int sec_trans_write(const uint8_t *frame, size_t frame_size, uint32_t timeout) {
  // wait combus pull up, SE is IDLE
  uint32_t until = HAL_GetTick() + timeout;
  while(SE_COMBUS_IS_LOW()) {
    if (HAL_GetTick() > until) {
      return SEC_TRANS_ERR_TIMEOUT;
    }
  }
  // delay a short time
  HAL_Delay(1);
  SCB_DisableICache();
  SCB_DisableDCache();
  int ret = HAL_SPI_Transmit(&hspi5, frame, SEC_MAX_FRAME_SIZE, timeout);
  SCB_EnableDCache();
  SCB_EnableICache();
  if (ret == HAL_TIMEOUT) {
    return SEC_TRANS_ERR_TIMEOUT;
  } else if (ret != HAL_OK) {
    return SEC_TRANS_ERR_FAILED;
  }
  LOG_DEBUG(MODULE, "APP ==> SE");
  log_frame((uint8_t*)frame);
  return SEC_TRANS_SUCCESS;
}

int sec_trans_read(uint8_t *frame, size_t frame_buf_size, uint32_t timeout) {
  // wait combus pull down, SE have processed data
  uint32_t until = HAL_GetTick() + timeout;
  while(SE_COMBUS_IS_HIGH()) {
    if (HAL_GetTick() > until) {
      return SEC_TRANS_ERR_TIMEOUT;
    }
  };
  // delay a short time
  HAL_Delay(1);
  SCB_DisableICache();
  SCB_DisableDCache();
  int ret = HAL_SPI_Receive(&hspi5, frame, SEC_MAX_FRAME_SIZE, timeout);
  SCB_EnableDCache();
  SCB_EnableICache();
  if (ret == HAL_TIMEOUT) {
    return SEC_TRANS_ERR_TIMEOUT;
  } else if (ret != HAL_OK) {
    return SEC_TRANS_ERR_FAILED;
  }
  HAL_Delay(1);
  LOG_DEBUG(MODULE, "SE ==> APP");
  log_frame((uint8_t*)frame);
  return SEC_TRANS_SUCCESS;
}
int se_send(uint8_t *buf, size_t size, uint32_t timeout) {
  // wait combus pull up, SE is IDLE
  while (SE_COMBUS_IS_LOW());
  HAL_Delay(1);
  LOG_HEXDUMP_DEBUG(MODULE, "sending", buf, size);
  if (HAL_SPI_Transmit(&hspi5, buf, size, timeout) != HAL_OK) {
    return -1;
  }
  return 0;
}

int se_recv(uint8_t *buf, size_t size, uint32_t timeout) {
  // wait combus pull down, SE have processed data
  while (SE_COMBUS_IS_HIGH());
  HAL_Delay(1);
  if (HAL_SPI_Receive(&hspi5, buf, size, timeout) != HAL_OK) {
    return -1;
  }
  LOG_HEXDUMP_DEBUG(MODULE, "received", buf, size);
  return 0;
}
