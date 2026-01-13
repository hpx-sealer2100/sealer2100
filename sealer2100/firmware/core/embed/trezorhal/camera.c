#include "camera.h"
#include STM32_HAL_H

#include "stm32h7xx_hal_gpio.h"
#include "stm32h7xx_hal_dcmi.h"
#include "stm32h7xx_hal_dma.h"
#include "stm32h7xx_hal_dma_ex.h"
#include "stm32h7xx_hal_mdma.h"
#include "stm32h7xx_hal_ltdc.h"

#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>

#include "sdram.h"
#include "secbool.h"
#include "gc0308.h"
#include "i2c.h"
#include "device.h"
#include "power_manager.h"
#include "zbar.h"

// clang-format off
/*
        ┌─────────────────────────────┐
        │                             │
        │         Camera              │
        │                             │
        └──────────────┬──────────────┘
                       │
                       │
                       │
                       ▼
        ┌──────────────────────────────┐
        │                              │
        │   SRam in D2 domain          │
        │                              │
        │   For speed up capturing     │
        │                              │
        └───────────────┬──────────────┘
                        │
           copy to image│ buffer
                        │
                        │
                        │
                        ▼
       ┌────────────────────────────────┐
       │                                │
       │    Image Buffer                │
       │                                │
       │    for lvgl canvas             │
       │                                │
       └───────────────┬────────────────┘
                       │
          rgb565 -> gray
                       │
      ┌────────────────▼─────────────────┐
      │                                  │
      │   Gray image buffer              │
      │                                  │
      │   For QRCode decode              │
      │                                  │
      │                                  │
      └──────────────────────────────────┘

*/
// clang-format on
// debug for qrcode scan
#define CAMERA_SHOW_GRAY 0

#define LINES_PER_DMA_FETCH 1

#define CAMERA_POWER_ON() do {                                                    \
  pm_power_up(POWER_MODULE_CAMERA);                                               \
}while(0)
#define CAMERA_POWER_OFF() do {                                                     \
  pm_power_down(POWER_MODULE_CAMERA);                                               \
}while(0)
// camera back work state pin PJ14
#define CAMERA_BACK_WORK_STATE_GPIO_CLK_ENABLE() __HAL_RCC_GPIOJ_CLK_ENABLE()
#define CAMERA_BACK_WORK_STATE_GPIO_PORT GPIOJ
#define CAMERA_BACK_WORK_STATE_GPIO_PIN GPIO_PIN_14

// camera front work state pin PD5
#define CAMERA_FRONT_WORK_STATE_GPIO_CLK_ENABLE() __HAL_RCC_GPIOD_CLK_ENABLE()
#define CAMERA_FRONT_WORK_STATE_GPIO_PORT GPIOD
#define CAMERA_FRONT_WORK_STATE_GPIO_PIN GPIO_PIN_5

#define CAMERA_BACK_ON()   do{                                                                             \
  HAL_GPIO_WritePin(CAMERA_BACK_WORK_STATE_GPIO_PORT, CAMERA_BACK_WORK_STATE_GPIO_PIN, GPIO_PIN_RESET);    \
  HAL_GPIO_WritePin(CAMERA_FRONT_WORK_STATE_GPIO_PORT, CAMERA_FRONT_WORK_STATE_GPIO_PIN, GPIO_PIN_SET);    \
}while(0)

#define CAMERA_FRONT_ON()  do{                                                                             \
  HAL_GPIO_WritePin(CAMERA_FRONT_WORK_STATE_GPIO_PORT, CAMERA_FRONT_WORK_STATE_GPIO_PIN, GPIO_PIN_RESET);  \
  HAL_GPIO_WritePin(CAMERA_BACK_WORK_STATE_GPIO_PORT, CAMERA_BACK_WORK_STATE_GPIO_PIN, GPIO_PIN_SET);      \
}while(0)

// pcb v1.1 camera hard reset pin PE2
#define CAMERA_HW_RESET_GPIO_CLK_ENABLE() __HAL_RCC_GPIOE_CLK_ENABLE()
#define CAMERA_HW_RESET_GPIO_PORT GPIOE
#define CAMERA_HW_RESET_GPIO_PIN GPIO_PIN_2

// pcb v1.1 camera led pin PD5
#define CAMERA_LED_GPIO_CLK_ENABLE() __HAL_RCC_GPIOD_CLK_ENABLE()
#define CAMERA_LED_GPIO_PORT GPIOD
#define CAMERA_LED_GPIO_PIN GPIO_PIN_5

#define CAMERA_GRAY_BUFFER FMC_SDRAM_GRAY_IMAGE_BUFFER_ADDRESS
#define CAMERA_CAPTURE_BUFFER 0x30020000 // sram2 in D2

#define GC0308_RGB565 1 // rgb565 bytes in gc0308 is reversed

static volatile uint32_t line = 0;
static DCMI_HandleTypeDef hdcmi = {0};
static MDMA_HandleTypeDef hmdma;

// local functions
/// camera functions
static void camera_led_init(void);
static void camera_hardware_reset(void);
static void camera_mdma_init(void);
static HAL_StatusTypeDef camera_start_dma(DCMI_HandleTypeDef *hdcmi,
                                   uint32_t DCMI_Mode, uint32_t pData);
/// DCMI functions
static void DCMI_MspInit(DCMI_HandleTypeDef *hdcmi);
static HAL_StatusTypeDef MX_DCMI_Init(DCMI_HandleTypeDef* hdcmi);
static void DCMI_MspDeInit(DCMI_HandleTypeDef *hdcmi);

#if CAMERA_SHOW_GRAY
static void gray_to_rgb565(void);
// reuse this buffer
#define CAMERA_TEST_BUFFER FMC_SDRAM_JPEG_OUTPUT_DATA_BUFFER_ADDRESS
#endif

enum {
  QR_NONE,
  QR_MAKING_GRAYSCALE,
  QR_GRAYSCALE,
  QR_CODE,
};

enum {
  CAMERA_NONE,
  CAMERA_STOPED,
  CAMERA_POWRED,
  CAMERA_CAPTURING,
  CAMERA_CAPTURED,
};

typedef struct {
  // capture window(w, h)
  uint32_t width;
  uint32_t height;
  uint8_t camera_state;
  uint8_t qr_state;
} camera_context_t;

static camera_context_t __camera__ = {0};

#define CAMERA_FRAME_WIDTH 640
#define CAMERA_FRAME_HEIGHT 480

/// exported functions
bool camera_init(int width, int height) {
  __camera__.width = width;
  __camera__.height = height;
  line = 0;

  // clip 640x480 to 400x400
  camera_mdma_init();

  // make camera in normal mode

  CAMERA_FRONT_WORK_STATE_GPIO_CLK_ENABLE();
  CAMERA_BACK_WORK_STATE_GPIO_CLK_ENABLE();

  GPIO_InitTypeDef gpio;
  gpio.Pin = CAMERA_FRONT_WORK_STATE_GPIO_PIN;
  gpio.Mode = GPIO_MODE_OUTPUT_PP;
  gpio.Pull = GPIO_PULLDOWN;
  gpio.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(CAMERA_FRONT_WORK_STATE_GPIO_PORT, &gpio);

  gpio.Pin = CAMERA_BACK_WORK_STATE_GPIO_PIN;
  gpio.Mode = GPIO_MODE_OUTPUT_PP;
  gpio.Pull = GPIO_PULLDOWN;
  gpio.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(CAMERA_BACK_WORK_STATE_GPIO_PORT, &gpio);

  // power on camera module
  CAMERA_POWER_ON();
  // hardware reset
  camera_hardware_reset();

  // gc0308 initialize
  CAMERA_FRONT_ON();
  int id_front = 0;
  printf("gc0308 front probing ...\n");
  i2c4_lock();
  if (gc0308_read_id(&id_front) == 0 && id_front == GC0308_ID) {
    printf("gc0308 front found\n");
    gc0308_init();
  }
  i2c4_unlock();

  CAMERA_BACK_ON();
  int id_back = 0;
  printf("gc0308 back probing ...\n");
  i2c4_lock();
  if (gc0308_read_id(&id_back) == 0 && id_back == GC0308_ID) {
    printf("gc0308 back found\n");
    gc0308_init();
    gc0308_set_window(width, height);
  }
  i2c4_unlock();

  // dcmi initialize
  DCMI_MspInit(&hdcmi);

  MX_DCMI_Init(&hdcmi);
  __camera__.camera_state = CAMERA_POWRED;
  __camera__.qr_state = QR_NONE;

  return true;
}

void camera_deinit(void) {
  if (__camera__.camera_state == CAMERA_NONE) {
    return;
  }
  HAL_DCMI_DeInit(&hdcmi);
  DCMI_MspDeInit(&hdcmi);
  HAL_MDMA_DeInit(&hmdma);
  memset(&__camera__, 0, sizeof(__camera__));
  CAMERA_POWER_OFF();
}

void camera_start(void) {
  camera_start_dma(&hdcmi, DCMI_MODE_CONTINUOUS, CAMERA_CAPTURE_BUFFER);
  __camera__.camera_state = CAMERA_CAPTURING;
}

void camera_suspend(void) {
  HAL_DCMI_Suspend(&hdcmi);
}

void camera_resume(void) {
  __camera__.camera_state = CAMERA_CAPTURING;
  HAL_DCMI_Resume(&hdcmi);
}

void camera_stop(void) {
  __camera__.camera_state = CAMERA_STOPED;
  HAL_DCMI_Stop(&hdcmi);
}

void camera_led_on(void) {
    // camera led on
  camera_led_init();
  HAL_GPIO_WritePin(CAMERA_LED_GPIO_PORT, CAMERA_LED_GPIO_PIN, GPIO_PIN_SET);
}

void camera_led_off(void) {
  // camera led off
  HAL_GPIO_WritePin(CAMERA_LED_GPIO_PORT, CAMERA_LED_GPIO_PIN, GPIO_PIN_RESET);
}

bool camera_is_power_on(void) { return __camera__.camera_state >= CAMERA_POWRED; }
bool camera_is_captured(void) { return __camera__.camera_state == CAMERA_CAPTURED; }

#if CAMERA_SHOW_GRAY
bool camera_is_grayscale_done(void) { return false;}
#else
bool camera_is_grayscale_done(void) { return __camera__.qr_state == QR_GRAYSCALE; }
#endif


/// local functions
void camera_hardware_reset(void) {
  GPIO_InitTypeDef gpio;
  GPIO_TypeDef* port = NULL;
  uint32_t pin = 0;
  CAMERA_HW_RESET_GPIO_CLK_ENABLE();
  port = CAMERA_HW_RESET_GPIO_PORT;
  pin = CAMERA_HW_RESET_GPIO_PIN;

  gpio.Mode = GPIO_MODE_OUTPUT_PP;
  gpio.Pull = GPIO_PULLUP;
  gpio.Speed = GPIO_SPEED_FREQ_LOW;
  gpio.Pin = pin;
  HAL_GPIO_Init(port, &gpio);

  // hard reset camera
  HAL_GPIO_WritePin(port, pin, GPIO_PIN_RESET);
  HAL_Delay(10);
  // release reset
  HAL_GPIO_WritePin(port, pin, GPIO_PIN_SET);
  HAL_Delay(10);
}

void camera_work_normal(const char* name) {
  if(strcmp(name, "front") == 0) {
    CAMERA_FRONT_ON();
    HAL_Delay(10);
    gc0308_set_window(__camera__.width, __camera__.height);
    gc0308_vertical_mirror(true);
  } else if(strcmp(name, "back") == 0) {
    CAMERA_BACK_ON();
    HAL_Delay(10);
    gc0308_set_window(__camera__.width, __camera__.height);
    gc0308_vertical_mirror(false);
  }else if (strcmp(name,"none") == 0){
    HAL_GPIO_WritePin(CAMERA_BACK_WORK_STATE_GPIO_PORT, CAMERA_BACK_WORK_STATE_GPIO_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(CAMERA_FRONT_WORK_STATE_GPIO_PORT, CAMERA_FRONT_WORK_STATE_GPIO_PIN, GPIO_PIN_SET);
  }
}

static void camera_mdma_init(void) {
  __HAL_RCC_MDMA_CLK_ENABLE();
  hmdma.Instance = MDMA_Channel1;
  hmdma.Init.Request = MDMA_REQUEST_SW;
  hmdma.Init.TransferTriggerMode = MDMA_BLOCK_TRANSFER;
  hmdma.Init.Priority = MDMA_PRIORITY_HIGH;
#if GC0308_RGB565
  hmdma.Init.Endianness = MDMA_LITTLE_BYTE_ENDIANNESS_EXCHANGE;
#else
  hmdma.Init.Endianness = MDMA_LITTLE_ENDIANNESS_PRESERVE;
#endif
  hmdma.Init.SourceInc = MDMA_SRC_INC_WORD;
  hmdma.Init.DestinationInc = MDMA_DEST_INC_WORD;
  hmdma.Init.SourceDataSize = MDMA_SRC_DATASIZE_WORD;
  hmdma.Init.DestDataSize = MDMA_DEST_DATASIZE_WORD;
  hmdma.Init.DataAlignment = MDMA_DATAALIGN_PACKENABLE;
  hmdma.Init.SourceBurst = MDMA_SOURCE_BURST_4BEATS;
  hmdma.Init.DestBurst = MDMA_DEST_BURST_4BEATS;
  hmdma.Init.BufferTransferLength = 128;
  hmdma.Init.SourceBlockAddressOffset = 0;
  hmdma.Init.DestBlockAddressOffset = 0;

  HAL_MDMA_Init(&hmdma);
}

static void DCMI_DMAXferCplt(DMA_HandleTypeDef *hdma)
{
  uint32_t tmp ;
  DCMI_HandleTypeDef *hdcmi = (DCMI_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;
  uint32_t src = 0;
  uint32_t dst = 0;

  if (hdcmi->XferCount != 0U)
  {
    /* Update memory 0 address location */
    tmp = ((((DMA_Stream_TypeDef *)(hdcmi->DMA_Handle->Instance))->CR) & DMA_SxCR_CT);
    if (((hdcmi->XferCount % 2U) == 0U) && (tmp != 0U))
    {
      src = tmp = ((DMA_Stream_TypeDef *)(hdcmi->DMA_Handle->Instance))->M0AR;
      (void)HAL_DMAEx_ChangeMemory(hdcmi->DMA_Handle, (tmp + (8U * hdcmi->XferSize)), MEMORY0);
      hdcmi->XferCount--;
    }
    /* Update memory 1 address location */
    else if ((((DMA_Stream_TypeDef *)(hdcmi->DMA_Handle->Instance))->CR & DMA_SxCR_CT) == 0U)
    {
      src = tmp = ((DMA_Stream_TypeDef *)(hdcmi->DMA_Handle->Instance))->M1AR;
      (void)HAL_DMAEx_ChangeMemory(hdcmi->DMA_Handle, (tmp + (8U * hdcmi->XferSize)), MEMORY1);
      hdcmi->XferCount--;
    }
    else
    {
      /* Nothing to do */
    }
  }
  /* Update memory 0 address location */
  else if ((((DMA_Stream_TypeDef *)(hdcmi->DMA_Handle->Instance))->CR & DMA_SxCR_CT) != 0U)
  {
    src = ((DMA_Stream_TypeDef *)(hdcmi->DMA_Handle->Instance))->M0AR;
    ((DMA_Stream_TypeDef *)(hdcmi->DMA_Handle->Instance))->M0AR = hdcmi->pBuffPtr;
  }
  /* Update memory 1 address location */
  else if ((((DMA_Stream_TypeDef *)(hdcmi->DMA_Handle->Instance))->CR & DMA_SxCR_CT) == 0U)
  {
    src = ((DMA_Stream_TypeDef *)(hdcmi->DMA_Handle->Instance))->M1AR;
    tmp = hdcmi->pBuffPtr;
    ((DMA_Stream_TypeDef *)(hdcmi->DMA_Handle->Instance))->M1AR = (tmp + (4U * hdcmi->XferSize));
    hdcmi->XferCount = hdcmi->XferTransferNumber;
  }

  dst = FMC_SDRAM_IMAGE_BUFFER_ADDRESS + line * __camera__.width * 2;
  HAL_MDMA_Start(&hmdma, src, dst, __camera__.width * 2, LINES_PER_DMA_FETCH);
  HAL_MDMA_PollForTransfer(&hmdma, HAL_MDMA_FULL_TRANSFER, 1000);
  // update lines
  line += LINES_PER_DMA_FETCH;
  if (line == __camera__.height) {
    line = 0;
    __HAL_DCMI_ENABLE_IT(hdcmi, DCMI_IT_FRAME);
  }
  return ;
}
static void DCMI_DMAError(DMA_HandleTypeDef *hdma) {
  DCMI_HandleTypeDef *hdcmi =
      (DCMI_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;

  if (hdcmi->DMA_Handle->ErrorCode != HAL_DMA_ERROR_FE) {
    /* Initialize the DCMI state*/
    hdcmi->State = HAL_DCMI_STATE_READY;

    /* Set DCMI Error Code */
    hdcmi->ErrorCode |= HAL_DCMI_ERROR_DMA;
  }

  /* DCMI error Callback */
#if (USE_HAL_DCMI_REGISTER_CALLBACKS == 1)
  /*Call registered DCMI error callback*/
  hdcmi->ErrorCallback(hdcmi);
#else
  HAL_DCMI_ErrorCallback(hdcmi);
#endif /* USE_HAL_DCMI_REGISTER_CALLBACKS */
}

static void DCMI_Abort(DMA_HandleTypeDef *hdma) {
  printf("DCMI abort\n");
}

HAL_StatusTypeDef camera_start_dma(DCMI_HandleTypeDef *hdcmi,
                                   uint32_t DCMI_Mode, uint32_t pData) {
  /* Initialize the second memory address */
  uint32_t SecondMemAddress;

  /* Check function parameters */
  assert_param(IS_DCMI_CAPTURE_MODE(DCMI_Mode));

  /* Process Locked */
  __HAL_LOCK(hdcmi);

  /* Lock the DCMI peripheral state */
  hdcmi->State = HAL_DCMI_STATE_BUSY;

  /* Enable DCMI by setting DCMIEN bit */
  __HAL_DCMI_ENABLE(hdcmi);

  /* Configure the DCMI Mode */
  hdcmi->Instance->CR &= ~(DCMI_CR_CM);
  hdcmi->Instance->CR |= (uint32_t)(DCMI_Mode);

  /* Set the DMA memory0 conversion complete callback */
  hdcmi->DMA_Handle->XferCpltCallback = DCMI_DMAXferCplt;

  /* Set the DMA error callback */
  hdcmi->DMA_Handle->XferErrorCallback = DCMI_DMAError;

  /* Set the dma abort callback */
  hdcmi->DMA_Handle->XferAbortCallback = DCMI_Abort;

  /* Set the DMA memory1 conversion complete callback */
  hdcmi->DMA_Handle->XferM1CpltCallback = DCMI_DMAXferCplt;

  /* Initialize transfer parameters */
  hdcmi->XferCount = 2; // lines
  hdcmi->XferSize = __camera__.width / 2 * LINES_PER_DMA_FETCH; // words per line * LINES_PER_DMA_FETCH
  hdcmi->pBuffPtr = pData;

  /* Update DCMI counter  and transfer number*/
  hdcmi->XferCount = (hdcmi->XferCount - 2U);
  hdcmi->XferTransferNumber = hdcmi->XferCount;

  /* Update second memory address */
  SecondMemAddress = (uint32_t)(pData + (4U * hdcmi->XferSize));

  /* Start DMA multi buffer transfer */
  if (HAL_DMAEx_MultiBufferStart_IT(
          hdcmi->DMA_Handle, (uint32_t)&hdcmi->Instance->DR, (uint32_t)pData,
          SecondMemAddress, hdcmi->XferSize) != HAL_OK) {
    /* Set Error Code */
    hdcmi->ErrorCode = HAL_DCMI_ERROR_DMA;
    /* Change DCMI state */
    hdcmi->State = HAL_DCMI_STATE_READY;
    /* Release Lock */
    __HAL_UNLOCK(hdcmi);
    /* Return function status */
    return HAL_ERROR;
  }

  /* Enable Capture */
  hdcmi->Instance->CR |= DCMI_CR_CAPTURE;

  /* Release Lock */
  __HAL_UNLOCK(hdcmi);

  /* Return function status */
  return HAL_OK;
}

void camera_led_init(void) {
  GPIO_InitTypeDef gpio;
  gpio.Mode = GPIO_MODE_OUTPUT_PP;
  gpio.Pull = GPIO_PULLDOWN;
  gpio.Speed = GPIO_SPEED_FREQ_LOW;

  CAMERA_LED_GPIO_CLK_ENABLE();
  gpio.Pin = CAMERA_LED_GPIO_PIN;
  HAL_GPIO_Init(CAMERA_LED_GPIO_PORT, &gpio);
}


void convert_rgb565_to_grayscale(void) {
  uint8_t *p = (uint8_t *)FMC_SDRAM_IMAGE_BUFFER_ADDRESS;
  uint8_t *gray = (uint8_t *)CAMERA_GRAY_BUFFER;
  for (int i = 0; i < __camera__.width * __camera__.height; i++) {
    uint16_t rgb565 = p[1] << 8 | p[0];
    uint8_t r = (rgb565 >> 11) & 0x1F;
    uint8_t g = (rgb565 >> 5) & 0x3F;
    uint8_t b = rgb565 & 0x1F;
    *gray++ = (r * 77 + g * 151 + b * 28) >> 8;
    p += 2;
  }
}

void histogram_equalization() {
    uint32_t histogram[256] = {0};
    uint32_t cdf[256] = {0};
    int count = __camera__.width * __camera__.height;
    uint8_t *gray = (uint8_t *)CAMERA_GRAY_BUFFER;

    // 1. 计算直方图
    for (int i = 0; i < count; i++) {
        histogram[gray[i]]++;
    }

    // 2. 计算累积分布函数 (CDF)
    cdf[0] = histogram[0];
    for (int i = 1; i < 256; i++) {
        cdf[i] = cdf[i - 1] + histogram[i];
    }

    // 3. 归一化 CDF
    int cdf_min = cdf[0];
    for (int i = 0; i < 256; i++) {
        cdf[i] = ((cdf[i] - cdf_min) * 255) / (count - cdf_min);
    }

    // 4. 映射像素值
    for (int i = 0; i < count; i++) {
        gray[i] = cdf[gray[i]];
    }
}

#if CAMERA_SHOW_GRAY
static void gray_to_rgb565(void) {
  uint8_t *src = (uint8_t *)CAMERA_GRAY_BUFFER;
  uint16_t *dst = (uint16_t *)CAMERA_TEST_BUFFER;
  for (int i = 0; i < __camera__.width * __camera__.height; i++) {
    uint8_t gray = *src++;
    uint8_t r = (gray >> 3) & 0x1F;  // 5 bits for red
    uint8_t g = (gray >> 2) & 0x3F;  // 6 bits for green
    uint8_t b = (gray >> 3) & 0x1F;  // 5 bits for blue

    // Combine into RGB565 format
    uint16_t rgb565 = (r << 11) | (g << 5) | b;
    *dst++ = rgb565;
  }
}
#endif

int camera_scan_qrcode(uint8_t qrcode[1024 + 1], int *type) {
  if (__camera__.qr_state != QR_GRAYSCALE) {
    return -1;
  }
  int qr_code_len = 0;
  zbar_image_scanner_t *scanner = NULL;
  /* create a reader */
  scanner = zbar_image_scanner_create();

  /* configure the reader */
  zbar_image_scanner_set_config(scanner, ZBAR_QRCODE, ZBAR_CFG_ENABLE, 1);
  zbar_image_scanner_set_config(scanner, ZBAR_QRCODE, ZBAR_CFG_X_DENSITY, 4);
  zbar_image_scanner_set_config(scanner, ZBAR_QRCODE, ZBAR_CFG_Y_DENSITY, 4);

  /* wrap image data */
  zbar_image_t *image = zbar_image_create();
  zbar_image_set_format(image, *(int *)"Y800");

  zbar_image_set_size(image, __camera__.width, __camera__.height);
  uint8_t *gray = (uint8_t *)CAMERA_GRAY_BUFFER;
  zbar_image_set_data(image, gray, __camera__.width * __camera__.height, NULL);

  /* scan the image for barcodes */
  int n = zbar_scan_image(scanner, image);
  if (n != 1) {
    __camera__.qr_state = QR_NONE;
    zbar_image_destroy(image);
    zbar_image_scanner_destroy(scanner);
    return -2;
  }

  /* extract results */
  const zbar_symbol_t *symbol = zbar_image_first_symbol(image);

  // zbar_symbol_get_type(symbol); // ZBAR_QRCODE, only enabled QRCODE

  // only one symbol
  const char *data = zbar_symbol_get_data(symbol);
  qr_code_len = strlen(data);
  strcpy((char*)qrcode, data);

  if (qr_code_len > 0) {
    __camera__.qr_state = QR_CODE;
    *type = CAMERA_QR_TYPE_STRING;
    printf("qrcode : %s", (char*)qrcode);
  } else {
    __camera__.qr_state = QR_NONE;
  }
  /* clean up */
  zbar_image_destroy(image);
  zbar_image_scanner_destroy(scanner);

  return qr_code_len;
}

void camera_scan_reset(void) {
  __camera__.qr_state = QR_NONE;
}

uint8_t* camera_buffer(void) {
#if CAMERA_SHOW_GRAY
  return (uint8_t*)CAMERA_TEST_BUFFER;
#else
  return (uint8_t*)FMC_SDRAM_IMAGE_BUFFER_ADDRESS;
#endif
}

/// dcmi callbacks
void HAL_DCMI_VsyncEventCallback(DCMI_HandleTypeDef *hdcmi) {
  (void)hdcmi;
  line = 0;
}
void HAL_DCMI_ErrorCallback(DCMI_HandleTypeDef *hdcmi) {
  if (hdcmi->ErrorCode == 0) {
    return;
  }
  camera_deinit();
}
void HAL_DCMI_FrameEventCallback(DCMI_HandleTypeDef *hdcmi) {
  HAL_DCMI_Suspend(hdcmi);
  __camera__.camera_state = CAMERA_CAPTURED;
  // no need convert gray while COVERING or already gray
  if (__camera__.qr_state != QR_GRAYSCALE && __camera__.qr_state != QR_MAKING_GRAYSCALE) {
    __camera__.qr_state = QR_MAKING_GRAYSCALE;
    convert_rgb565_to_grayscale();
    // 增加对比度
    histogram_equalization();
    __camera__.qr_state = QR_GRAYSCALE;
  }
#if CAMERA_SHOW_GRAY
  convert_rgb565_to_grayscale();
  histogram_equalization();
  gray_to_rgb565();
  char value[1024+1] = {0};
  int type = 0;
  int len = camera_scan_qrcode((uint8_t*)value, &type);
  if (len > 0) {
    printf("camera scan qrcode: %s\n", value);
  } else {
    printf("camera scan qrcode failed: %d\n", len);
  }
#endif
  // not resume, resume by caller
  // camera_resume();
}

/// DCMI initialization
static void DCMI_MspInit(DCMI_HandleTypeDef *hdcmi) {
  static DMA_HandleTypeDef hdma_handler;
  GPIO_InitTypeDef gpio_init_structure;

  /*** Enable peripherals and GPIO clocks ***/
  /* Enable DCMI clock */
  __HAL_RCC_DCMI_CLK_ENABLE();

  /* Enable DMA2 clock */
  __HAL_RCC_DMA2_CLK_ENABLE();

  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();

   // HSYNC PCLK  D0 D1
  gpio_init_structure.Pin       = GPIO_PIN_4 | GPIO_PIN_6 | GPIO_PIN_9 | GPIO_PIN_10;
  gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
  gpio_init_structure.Pull      = GPIO_PULLUP;
  gpio_init_structure.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
  gpio_init_structure.Alternate = GPIO_AF13_DCMI;
  HAL_GPIO_Init(GPIOA, &gpio_init_structure);

  // D5
  gpio_init_structure.Pin       = GPIO_PIN_3;
  gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
  gpio_init_structure.Pull      = GPIO_PULLUP;
  gpio_init_structure.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
  gpio_init_structure.Alternate = GPIO_AF13_DCMI;
  HAL_GPIO_Init(GPIOD, &gpio_init_structure);

  //  D4 D6 D7
  gpio_init_structure.Pin       = GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6;
  gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
  gpio_init_structure.Pull      = GPIO_PULLUP;
  gpio_init_structure.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
  gpio_init_structure.Alternate = GPIO_AF13_DCMI;
  HAL_GPIO_Init(GPIOE, &gpio_init_structure);

  // VSYNC D2 D3
  gpio_init_structure.Pin       = GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11;
  gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
  gpio_init_structure.Pull      = GPIO_PULLUP;
  gpio_init_structure.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
  gpio_init_structure.Alternate = GPIO_AF13_DCMI;
  HAL_GPIO_Init(GPIOG, &gpio_init_structure);

  /*** Configure the DMA ***/
  /* Set the parameters to be configured */
  hdma_handler.Init.Request             = DMA_REQUEST_DCMI;
  hdma_handler.Init.Direction           = DMA_PERIPH_TO_MEMORY;
  hdma_handler.Init.PeriphInc           = DMA_PINC_DISABLE;
  hdma_handler.Init.MemInc              = DMA_MINC_ENABLE;
  hdma_handler.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
  hdma_handler.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
  hdma_handler.Init.Mode                = DMA_CIRCULAR;
  hdma_handler.Init.Priority            = DMA_PRIORITY_HIGH;
  hdma_handler.Init.FIFOMode            = DMA_FIFOMODE_ENABLE;
  hdma_handler.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
  hdma_handler.Init.MemBurst            = DMA_MBURST_SINGLE;
  hdma_handler.Init.PeriphBurst         = DMA_PBURST_SINGLE;
  hdma_handler.Instance                 = DMA2_Stream3;

  /* Associate the initialized DMA handle to the DCMI handle */
  __HAL_LINKDMA(hdcmi, DMA_Handle, hdma_handler);

  /*** Configure the NVIC for DCMI and DMA ***/
  /* NVIC configuration for DCMI transfer complete interrupt */
  HAL_NVIC_SetPriority(DCMI_IRQn, 8, 0);
  HAL_NVIC_EnableIRQ(DCMI_IRQn);

  /* NVIC configuration for DMA2D transfer complete interrupt */
  HAL_NVIC_SetPriority(DMA2_Stream3_IRQn, 9, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream3_IRQn);

  // 启用 VSYNC 中断
  __HAL_DCMI_ENABLE_IT(hdcmi, DCMI_IT_VSYNC);

  /* Configure the DMA stream */
  (void)HAL_DMA_Init(hdcmi->DMA_Handle);
}

HAL_StatusTypeDef MX_DCMI_Init(DCMI_HandleTypeDef* hdcmi)
{
  /*** Configures the DCMI to interface with the camera module ***/
  /* DCMI configuration */
  hdcmi->Instance              = DCMI;
  hdcmi->Init.CaptureRate      = DCMI_CR_ALL_FRAME;
  hdcmi->Init.HSPolarity       = DCMI_HSPOLARITY_HIGH;
  hdcmi->Init.SynchroMode      = DCMI_SYNCHRO_HARDWARE;
  hdcmi->Init.VSPolarity       = DCMI_VSPOLARITY_HIGH;
  hdcmi->Init.ExtendedDataMode = DCMI_EXTEND_DATA_8B;
  hdcmi->Init.PCKPolarity      = DCMI_PCKPOLARITY_RISING;

  if(HAL_DCMI_Init(hdcmi) != HAL_OK)
  {
    return HAL_ERROR;
  }
  return HAL_OK;
}

static void DCMI_MspDeInit(DCMI_HandleTypeDef *hdcmi)
{
  GPIO_InitTypeDef gpio_init_structure;

  /* Disable NVIC  for DCMI transfer complete interrupt */
  HAL_NVIC_DisableIRQ(DCMI_IRQn);

  /* Disable NVIC for DMA2 transfer complete interrupt */
  HAL_NVIC_DisableIRQ(DMA2_Stream3_IRQn);

  /* Configure the DMA stream */
  (void)HAL_DMA_DeInit(hdcmi->DMA_Handle);

  /* DeInit DCMI GPIOs */
  // HSYNC PCLK  D0 D1
  gpio_init_structure.Pin       = GPIO_PIN_4 | GPIO_PIN_6 | GPIO_PIN_9 | GPIO_PIN_10;
  HAL_GPIO_DeInit(GPIOA, gpio_init_structure.Pin);

  // D5
  gpio_init_structure.Pin       = GPIO_PIN_3;
  HAL_GPIO_DeInit(GPIOD, gpio_init_structure.Pin);

  // D4 D6 D7
  gpio_init_structure.Pin       = GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6;
  HAL_GPIO_DeInit(GPIOE, gpio_init_structure.Pin);

  // VSYNC D2 D3
  gpio_init_structure.Pin       = GPIO_PIN_3 | GPIO_PIN_10 | GPIO_PIN_11;
  HAL_GPIO_DeInit(GPIOG, gpio_init_structure.Pin);

  /* Disable DCMI clock */
  __HAL_RCC_DCMI_CLK_DISABLE();
}

/// DCMI IRQ
void DCMI_IRQHandler(void) {
  HAL_DCMI_IRQHandler(&hdcmi);
}

/// DCMI DMA IRQ
void DMA2_Stream3_IRQHandler(void) {
  HAL_DMA_IRQHandler(hdcmi.DMA_Handle);
}
