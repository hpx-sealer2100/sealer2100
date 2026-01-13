#include STM32_HAL_H
#include "stm32h7xx_hal_gpio.h"
#include "stm32h7xx_hal_spi.h"
#include <stdio.h>
#include <string.h>
#include "se_spi.h"
#include "sec_trans.h"
#include "device.h"

#if 1
#define SE_LOG(...)
#define log_data(...)
#define log_frame(...)
#else
#define SE_LOG printf
void log_data(uint8_t* data, size_t data_size) {
  #define __FRAME_LINE__ 16
  uint8_t count = 0;
  while (data_size--) {
    count++;
    SE_LOG("%02x ", *data++);
    if (count == __FRAME_LINE__) {
      SE_LOG("\n");
      count = 0;
    }
  }
  SE_LOG("\n");
}
static void log_frame(uint8_t frame[SEC_MAX_FRAME_SIZE]) {
  uint8_t fctr = frame[0];
  (void)fctr;
  size_t len = frame[1];
  len <<= 8;
  len |= frame[2];

  size_t frame_size = len + 5;
  (void)frame_size;
  uint8_t* p = frame + 3;
  if (frame_size > SEC_MAX_FRAME_SIZE) {
    printf("SE spi maybe read failed\n");
    return;
  }
  SE_LOG("frame: %02x,  len: %d(%02x%02x), crc: %02x%02x\n", fctr, len,
          frame[1], frame[2], frame[frame_size - 2], frame[frame_size - 1]);

  log_data(p, len);
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
  gpio.Pull = GPIO_PULLDOWN;
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
  SE_LOG("APP ==> SE\n");
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
  SE_LOG("SE ==> APP\n");
  log_frame((uint8_t*)frame);
  return SEC_TRANS_SUCCESS;
}
int se_send(uint8_t *buf, size_t size, uint32_t timeout) {
  // wait combus pull up, SE is IDLE
  while (SE_COMBUS_IS_LOW());
  HAL_Delay(1);
  SE_LOG("SE sending: \n");
  log_data(buf, size);
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
  SE_LOG("SE received: \n");
  log_data(buf, size);
  return 0;
}
