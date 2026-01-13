#include "device.h"
#include STM32_HAL_H

#include "stm32h7xx_hal_gpio.h"
#include "stm32h7xx_hal_spi.h"
#include "stm32h7xx_hal_dma.h"

#include <stdio.h>
#include <string.h>

#include "common.h"
#include "irq.h"
#include "spi.h"
#include "timer.h"

SPI_HandleTypeDef spi;
static DMA_HandleTypeDef hdma_tx;
static DMA_HandleTypeDef hdma_rx;

static uint8_t *recv_buf = (uint8_t *)0x30040000;
static uint8_t *send_buf = (uint8_t *)0x30040100;
static int32_t volatile spi_rx_event = 0;
static int32_t volatile spi_tx_event = 0;
static int32_t volatile spi_abort_event = 0;
static secbool volatile spi_rx_complete = secfalse;
ChannelType host_channel = CHANNEL_NULL;

uint8_t spi_data_in[SPI_BUF_MAX_IN_LEN];
uint8_t spi_data_out[SPI_BUF_MAX_OUT_LEN];

#define SPI_LOG_DATA 0

#if SPI_LOG_DATA
static void log_data(char* tag, uint8_t *data, size_t len) {
  printf("%s : \n", tag);
  for (int i = 0; i < len; i++) {
    printf(" %02x", data[i]);
    if ((i+1) % 16 == 0) {
      printf("\n");
    }
  }
  printf("\n");
}
#else
#define log_data(...)
#endif

trans_fifo spi_fifo_in = {0};

void spi_fifo_buffer_init(void) {
  fifo_init(&spi_fifo_in, spi_data_in, SPI_BUF_MAX_IN_LEN);
}

secbool spi_can_write(void) {
  if (spi_tx_event == 0)
    return sectrue;
  else
    return secfalse;
}

int32_t wait_spi_rx_event(int32_t timeout) {
  int32_t tickstart = HAL_GetTick();

  while (spi_rx_event == 1) {
    if ((HAL_GetTick() - tickstart) > timeout) {
      return -1;
    }
  }
  return 0;
}

int32_t wait_spi_tx_event(int32_t timeout) {
  int32_t tickstart = HAL_GetTick();

  while (spi_tx_event == 1) {
    if ((HAL_GetTick() - tickstart) > timeout) {
      return -1;
    }
  }
  return 0;
}

void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi) {
  spi_rx_event = 0;
  spi_rx_complete = sectrue;
}

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi) {
  spi_tx_event = 0;
  memset(recv_buf, 0, SPI_PKG_SIZE);
  HAL_SPI_Receive_DMA(&spi, recv_buf, SPI_PKG_SIZE);
}

void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi) {
  ensure(secfalse, "spi ovr err");
}

void HAL_SPI_AbortCpltCallback(SPI_HandleTypeDef *hspi) { spi_abort_event = 0; }

void SPI1_IRQHandler(void) { HAL_SPI_IRQHandler(&spi); }

void control_pin_init(void) {
  GPIO_InitTypeDef gpio;
  GPIO_TypeDef* port = NULL;
  uint32_t pin = 0;
  BLE_CTRL_PIN_CLK_ENABLE();
  port = BLE_CTRL_PIN_GPIO_PORT;
  pin = BLE_CTRL_PIN_GPIO_PIN;

  // BLE SHAKE PIN
  gpio.Mode = GPIO_MODE_OUTPUT_PP;
  gpio.Pull = GPIO_PULLUP;
  gpio.Speed = GPIO_SPEED_FREQ_HIGH;
  gpio.Pin = pin;
  HAL_GPIO_Init(port, &gpio);
  HAL_GPIO_WritePin(port, pin, GPIO_PIN_SET);
}

void control_pin_deinit(void) {
  GPIO_InitTypeDef gpio;
  GPIO_TypeDef* port = NULL;
  uint32_t pin = 0;
  BLE_CTRL_PIN_CLK_ENABLE();
  port = BLE_CTRL_PIN_GPIO_PORT;
  pin = BLE_CTRL_PIN_GPIO_PIN;

  // BLE SHAKE PIN
  gpio.Mode = GPIO_MODE_INPUT;
  gpio.Pull = GPIO_NOPULL;
  gpio.Speed = GPIO_SPEED_FREQ_HIGH;
  gpio.Pin = pin;
  HAL_GPIO_Init(port, &gpio);
}

void control_pin_write(GPIO_PinState state) {
  HAL_GPIO_WritePin(BLE_CTRL_PIN_GPIO_PORT, BLE_CTRL_PIN_GPIO_PIN, state);
}

int32_t spi_slave_init() {
  spi_fifo_buffer_init();

  GPIO_InitTypeDef gpio;

  __HAL_RCC_SPI1_CLK_ENABLE();
  __HAL_RCC_DMA1_CLK_ENABLE();
  __HAL_RCC_DMA1_FORCE_RESET();
  __HAL_RCC_DMA1_RELEASE_RESET();
  __HAL_RCC_SPI1_FORCE_RESET();
  __HAL_RCC_SPI1_RELEASE_RESET();
  /*
   * MOSI: PD7
   * MISO: PB4
   * SCK:  PB3
   * NSS:  PA15
   * HS:   PE2
   */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();

  __HAL_RCC_SPI1_CLK_ENABLE();
  __HAL_RCC_DMA1_CLK_ENABLE();

  /// SPI1

  // PA15(NSS)
  gpio.Mode = GPIO_MODE_AF_PP;
  gpio.Pull = GPIO_NOPULL;
  gpio.Pin = GPIO_PIN_15;
  gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  gpio.Alternate = GPIO_AF5_SPI1;

  HAL_GPIO_Init(GPIOA, &gpio);

  // PB3(SCK)
  gpio.Pull = GPIO_PULLDOWN;
  gpio.Pin = GPIO_PIN_3;
  HAL_GPIO_Init(GPIOB, &gpio);

  gpio.Pull = GPIO_PULLUP;
  // PB4(MISO)
  gpio.Pin = GPIO_PIN_4;
  HAL_GPIO_Init(GPIOB, &gpio);

  // PD7(MOSI)
  gpio.Pin = GPIO_PIN_7;
  HAL_GPIO_Init(GPIOD, &gpio);

  spi.Instance = SPI1;
  spi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  spi.Init.Direction = SPI_DIRECTION_2LINES;
  spi.Init.CLKPhase = SPI_PHASE_1EDGE;
  spi.Init.CLKPolarity = SPI_POLARITY_LOW;
  spi.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  spi.Init.CRCPolynomial = 7;
  spi.Init.DataSize = SPI_DATASIZE_8BIT;
  spi.Init.FirstBit = SPI_FIRSTBIT_MSB;
  spi.Init.NSS = SPI_NSS_HARD_INPUT;
  spi.Init.TIMode = SPI_TIMODE_DISABLE;
  spi.Init.Mode = SPI_MODE_SLAVE;
  spi.Init.FifoThreshold = SPI_FIFO_THRESHOLD_08DATA;

  if (HAL_OK != HAL_SPI_Init(&spi)) {
    return -1;
  }


  /*##-3- Configure the DMA ##################################################*/
  /* Configure the DMA handler for Transmission process */
  hdma_tx.Instance = SPIx_TX_DMA_STREAM;
  hdma_tx.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
  hdma_tx.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
  hdma_tx.Init.MemBurst = DMA_MBURST_SINGLE;
  hdma_tx.Init.PeriphBurst = DMA_PBURST_SINGLE;
  hdma_tx.Init.Request = SPIx_TX_DMA_REQUEST;
  hdma_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
  hdma_tx.Init.PeriphInc = DMA_PINC_DISABLE;
  hdma_tx.Init.MemInc = DMA_MINC_ENABLE;
  hdma_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  hdma_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
  hdma_tx.Init.Mode = DMA_NORMAL;
  hdma_tx.Init.Priority = DMA_PRIORITY_LOW;

  HAL_DMA_DeInit(&hdma_tx);

  HAL_DMA_Init(&hdma_tx);

  /* Associate the initialized DMA handle to the the SPI handle */
  __HAL_LINKDMA(&spi, hdmatx, hdma_tx);

  /* Configure the DMA handler for Transmission process */
  hdma_rx.Instance = SPIx_RX_DMA_STREAM;

  hdma_rx.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
  hdma_rx.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
  hdma_rx.Init.MemBurst = DMA_MBURST_SINGLE;
  hdma_rx.Init.PeriphBurst = DMA_PBURST_SINGLE;
  hdma_rx.Init.Request = SPIx_RX_DMA_REQUEST;
  hdma_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
  hdma_rx.Init.PeriphInc = DMA_PINC_DISABLE;
  hdma_rx.Init.MemInc = DMA_MINC_ENABLE;
  hdma_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  hdma_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
  hdma_rx.Init.Mode = DMA_NORMAL;
  hdma_rx.Init.Priority = DMA_PRIORITY_HIGH;

  HAL_DMA_Init(&hdma_rx);

  /* Associate the initialized DMA handle to the the SPI handle */
  __HAL_LINKDMA(&spi, hdmarx, hdma_rx);

  /*##-4- Configure the NVIC for DMA #########################################*/
  /* NVIC configuration for DMA transfer complete interrupt (SPI1_TX) */
  NVIC_SetPriority(SPIx_DMA_TX_IRQn, IRQ_PRI_DMA);
  HAL_NVIC_EnableIRQ(SPIx_DMA_TX_IRQn);

  /* NVIC configuration for DMA transfer complete interrupt (SPI1_RX) */
  NVIC_SetPriority(SPIx_DMA_RX_IRQn, IRQ_PRI_DMA);
  HAL_NVIC_EnableIRQ(SPIx_DMA_RX_IRQn);

  NVIC_SetPriority(SPI1_IRQn, IRQ_PRI_SPI);
  HAL_NVIC_EnableIRQ(SPI1_IRQn);

  memset(recv_buf, 0, SPI_PKG_SIZE);
  spi_rx_event = 1;
  spi_rx_complete = secfalse;
  /* start SPI receive */
  if (HAL_SPI_Receive_DMA(&spi, recv_buf, SPI_PKG_SIZE) != HAL_OK) {
    return -1;
  }

  return 0;
}
int32_t spi_slave_deinit() {
  GPIO_InitTypeDef gpio;

  if (HAL_OK != HAL_SPI_DeInit(&spi)) {
    return -1;
  }
  /*
   * MOSI: PD7
   * MISO: PB4
   * SCK:  PB3
   * NSS:  PA15
   * HS:   PE2
   */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();

  // io config input no pull
  gpio.Mode = GPIO_MODE_INPUT;
  gpio.Pull = GPIO_NOPULL;
  gpio.Speed = GPIO_SPEED_FREQ_LOW;

  // use pe2 as software handshake pin
  // gpio.Pin = GPIO_PIN_2;
  // HAL_GPIO_Init(GPIOE, &gpio);

  // PA15(NSS)
  gpio.Pin = GPIO_PIN_15;
  HAL_GPIO_Init(GPIOA, &gpio);
  // SPI1
  // PB3(SCK)
  gpio.Pin = GPIO_PIN_3;
  HAL_GPIO_Init(GPIOB, &gpio);

  // PB4(MISO)
  gpio.Pin = GPIO_PIN_4;
  HAL_GPIO_Init(GPIOB, &gpio);

  // PD7(MOSI)
  gpio.Pin = GPIO_PIN_7;
  HAL_GPIO_Init(GPIOD, &gpio);
  return 0;
}


int32_t spi_slave_send(uint8_t *buf, uint32_t size, int32_t timeout) {
  int32_t ret = 0;
  if (size != TREZOR_PKG_SIZE) {
    printf("sending unpack trezor packet, size: %ld!!! \n", size);
  }
  log_data("sending", buf, size);

  memset(send_buf, 0, SPI_PKG_SIZE);
  memcpy(&send_buf[1], buf, size);

  size = ((size + 63)/64)*64;
  send_buf[0] = size;

  spi_abort_event = 1;
  if (HAL_SPI_Abort_IT(&spi) != HAL_OK) {
    memset(send_buf, 0, SPI_PKG_SIZE);
    printf("spi abort error!!\n");
    return 0;
  }
  while (spi_abort_event){}

  spi_tx_event = 1;
  if (HAL_SPI_Transmit_DMA(&spi, send_buf, SPI_PKG_SIZE) != HAL_OK) {
    printf("spi dma start send error!!\n");
    goto END;
  }

  SET_COMBUS_LOW();
  if (wait_spi_tx_event(timeout) != 0) {
    printf("spi wait send timeout!!\n");
    goto END;
  }
  ret = size;
END:
  HAL_SPI_Abort_IT(&spi);
  memset(send_buf, 0, SPI_PKG_SIZE);
  HAL_SPI_Receive_DMA(&spi, recv_buf, SPI_PKG_SIZE);
  // 无论成功与失败，都应该让spi再次可以写。
  spi_tx_event = 0;
  SET_COMBUS_HIGH();

  return ret;
}
static void try_cache_packet(void) {
  // not read data from spi
  if (spi_rx_complete != sectrue) {
    return;
  }
  spi_rx_complete = secfalse;

  // packet: L|V
  volatile size_t block = recv_buf[0];

  volatile size_t free = fifo_free_len(&spi_fifo_in);
  // not enough buffer for write spi data
  // Wait for consumers to consume enough data
  if (block > free) {
    spi_rx_complete = sectrue;
    return;
  }
  log_data("caching", recv_buf+1, recv_buf[0]);
  // ok, we can cache the packet to fifo buffer
  // copy data
  fifo_write_no_overflow(&spi_fifo_in, &recv_buf[1], block);
  memset(recv_buf, 0, SPI_PKG_SIZE);
  HAL_SPI_Receive_DMA(&spi, recv_buf, SPI_PKG_SIZE);
  SET_RX_BUS_BUSY();
  hal_delay(1);
  SET_RX_BUS_IDEL();
}

uint32_t spi_slave_poll(uint8_t *buf) {
  volatile uint32_t total_len, len, ret;

  // poll spi data to fifo buffer
  try_cache_packet();

  if (buf == NULL) return 0;

  total_len = fifo_lockdata_len(&spi_fifo_in);
  if (total_len == 0) {
    return 0;
  }
  len = total_len > TREZOR_PKG_SIZE ? TREZOR_PKG_SIZE : total_len;
  ret = fifo_read_lock(&spi_fifo_in, buf, len);
  log_data("reading", buf, ret);
  return ret;
}

uint32_t spi_read_retry(uint8_t *buf) {
  spi_rx_event = 1;

  for (int retry = 0;; retry++) {
    int r = wait_spi_rx_event(500);
    if (r == -1) {  // reading failed
      if (r == -1 && retry < 10) {
        // only timeout => let's try again
      } else {
        // error
        error_shutdown("Error reading", "from SPI.", "Try to", "reset.");
      }
    }

    if (r == 0) {
      return spi_slave_poll(buf);
    }
  }
}

uint32_t spi_read_blocking(uint8_t *buf, int timeout) {
  spi_rx_event = 1;

  // check if there already some data
  int r = spi_slave_poll(buf);

  // yes, retrun
  if (r != 0) {
    return r;
  }

  // no, try read with timeout
  switch (wait_spi_rx_event(timeout)) {
    case 0:
      return spi_slave_poll(buf);
      break;
    case -1:
    default:
      return 0;
      break;
  }
}

void SPIx_DMA_RX_IRQHandler(void) { HAL_DMA_IRQHandler(spi.hdmarx); }

void SPIx_DMA_TX_IRQHandler(void) { HAL_DMA_IRQHandler(spi.hdmatx); }
