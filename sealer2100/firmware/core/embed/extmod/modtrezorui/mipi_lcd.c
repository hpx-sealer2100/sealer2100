#include "display.h"
#include <stdbool.h>
#include STM32_HAL_H

#include "mipi_lcd.h"
#include "display_defs.h"
#include "stddef.h"
#include "device.h"
#include "gt911.h"

static int DISPLAY_BACKLIGHT = -1;
static int DISPLAY_ORIENTATION = -1;

#define HSE 25000 // MHz

// DSI_REQ 62.5 MHz HSE/IDF*N/ODF/8
#define DSI_N        36
#define DSI_IDF      1 // DSI_PLL_IN_DIV1
#define DSI_ODF      2 // DSI_PLL_OUT_DIV2

LCD_PARAMS lcd_params;
#if defined(LCD_USE_ST7701S)

#define LCD_HSYNC    ST7701S_480X800_HSYNC
#define LCD_HFP      ST7701S_480X800_HFP
#define LCD_HBP      ST7701S_480X800_HBP
#define LCD_VSYNC    ST7701S_480X800_VSYNC
#define LCD_VFP      ST7701S_480X800_VFP
#define LCD_VBP      ST7701S_480X800_VBP
// 26.97 MHz for st7701s 60fps
// LTDC_FREQ  27 HSE/M*N/R
#define LTDC_PLL_M   5
#define LTDC_PLL_N   108
#define LTDC_PLL_P   2
#define LTDC_PLL_Q   2
#define LTDC_PLL_R   20
#elif defined(LCD_USE_GC9503V)
#define LCD_HSYNC    GC9503V_480X800_HSYNC
#define LCD_HFP      GC9503V_480X800_HFP
#define LCD_HBP      GC9503V_480X800_HBP
#define LCD_VSYNC    GC9503V_480X800_VSYNC
#define LCD_VFP      GC9503V_480X800_VFP
#define LCD_VBP      GC9503V_480X800_VBP
// 26.84 MHz for gc9503v 60fps
// LTDC_FREQ  26.75 HSE/M*N/R
#define LTDC_PLL_M   5
#define LTDC_PLL_N   132
#define LTDC_PLL_P   2
#define LTDC_PLL_Q   2
#define LTDC_PLL_R   20
#else
#error "unknown lcd type"
#endif

DSI_HandleTypeDef hlcd_dsi;
DMA2D_HandleTypeDef hlcd_dma2d;
LTDC_HandleTypeDef hlcd_ltdc;
void lcd_enable_frame_it(void);

#define LTDC_BUFFER_1 (DISPLAY_MEMORY_BASE)
#define LTDC_BUFFER_2 (DISPLAY_MEMORY_BASE + DISPLAY_RESX*DISPLAY_RESY*2)

struct
{
  volatile uint32_t rendered;
  volatile uint32_t rendering;
  volatile bool updated;
} ltdc_buffer = {0};

static void ltdc_buffer_init(void) {
  ltdc_buffer.rendered = LTDC_BUFFER_1;
  ltdc_buffer.rendering = LTDC_BUFFER_2;
  ltdc_buffer.updated = false;
  memset((void*)ltdc_buffer.rendered, 0, DISPLAY_RESX*DISPLAY_RESY*2);
}

static void ltdc_msp_init(LTDC_HandleTypeDef *hltdc) {
  if (hltdc->Instance == LTDC) {
    /** Enable the LTDC clock */
    __HAL_RCC_LTDC_CLK_ENABLE();

    /** Toggle Sw reset of LTDC IP */
    __HAL_RCC_LTDC_FORCE_RESET();
    __HAL_RCC_LTDC_RELEASE_RESET();
  }
}

static void dma2d_msp_init(DMA2D_HandleTypeDef *hdma2d) {
  if (hdma2d->Instance == DMA2D) {
    /** Enable the DMA2D clock */
    __HAL_RCC_DMA2D_CLK_ENABLE();

    /** Toggle Sw reset of DMA2D IP */
    __HAL_RCC_DMA2D_FORCE_RESET();
    __HAL_RCC_DMA2D_RELEASE_RESET();
  }
}

static void dsi_msp_init(DSI_HandleTypeDef *hdsi) {
  if (hdsi->Instance == DSI) {
    /** Enable DSI Host and wrapper clocks */
    __HAL_RCC_DSI_CLK_ENABLE();

    /** Soft Reset the DSI Host and wrapper */
    __HAL_RCC_DSI_FORCE_RESET();
    __HAL_RCC_DSI_RELEASE_RESET();
  }
}

static void ltdc_program_line(uint32_t line) {
  /* Disable the Line interrupt */
  LTDC->IER &= ~(LTDC_IT_LI);

  /* Set the Line Interrupt position */
  LTDC->LIPCR = (uint32_t)line;

  /* Enable the Line interrupt */
  LTDC->IER |= LTDC_IT_LI;
}

static void ltdc_set_address(uint32_t address) {
 LTDC_Layer_TypeDef *layer = ((LTDC_Layer_TypeDef *)((uint32_t)(((uint32_t)(LTDC)) + 0x84U + (0x80U*(0)))));
 layer->CFBAR = address;
 LTDC->SRCR = LTDC_SRCR_IMR;
}

#define LTDC_FPS (60)
// total pixels in horizontal
#define LCD_H_TOTAL_PIXELS (DISPLAY_RESX + LCD_HSYNC + LCD_HFP + LCD_HBP)
// total pixels in vertical
#define LCD_V_TOTAL_PIXELS (DISPLAY_RESY + LCD_VSYNC + LCD_VFP + LCD_VBP)
// totoal pixels in frame
#define LCD_TOTAL_PIXELS (LCD_H_TOTAL_PIXELS * LCD_V_TOTAL_PIXELS)

// xxx KHz
#define LTDC_FREQ ((HSE/LTDC_PLL_M*LTDC_PLL_N/LTDC_PLL_R))

// DSI_FREQ >= (LCD_TOTAL_PIXELS × FPS × BPP) / (NumberOfLanes × 8)
// 62 MHz limit
#define DSI_FREQ (HSE/DSI_IDF*DSI_N/DSI_ODF/8)

HAL_StatusTypeDef dsi_host_init(DSI_HandleTypeDef *hdsi, uint32_t PixelFormat) {
  DSI_PLLInitTypeDef PLLInit;
  DSI_VidCfgTypeDef VidCfg;

  hdsi->Instance = DSI;
  hdsi->Init.AutomaticClockLaneControl = DSI_AUTO_CLK_LANE_CTRL_DISABLE;
  hdsi->Init.TXEscapeCkdiv = 4;
  hdsi->Init.NumberOfLanes = DSI_TWO_DATA_LANES;

  PLLInit.PLLNDIV = DSI_N;
  PLLInit.PLLIDF = DSI_PLL_IN_DIV1;
  PLLInit.PLLODF = DSI_PLL_OUT_DIV2;
  if (HAL_DSI_Init(hdsi, &PLLInit) != HAL_OK) {
    return HAL_ERROR;
  }

  // // timeout
  // {
  //   DSI_HOST_TimeoutTypeDef HostTimeouts = {0};
  //   HostTimeouts.TimeoutCkdiv = 1;
  //   HostTimeouts.HighSpeedTransmissionTimeout = 0;
  //   HostTimeouts.LowPowerReceptionTimeout = 0;
  //   HostTimeouts.HighSpeedReadTimeout = 0;
  //   HostTimeouts.LowPowerReadTimeout = 0;
  //   HostTimeouts.HighSpeedWriteTimeout = 0;
  //   HostTimeouts.HighSpeedWritePrespMode = DSI_HS_PM_DISABLE;
  //   HostTimeouts.LowPowerWriteTimeout = 0;
  //   HostTimeouts.BTATimeout = 0;
  //   if (HAL_DSI_ConfigHostTimeouts(hdsi, &HostTimeouts) != HAL_OK) {
  //     return HAL_ERROR;
  //   }
  // }

  // // phy timing
  // {
  //   DSI_PHY_TimerTypeDef PhyTimings = {0};
  //   PhyTimings.ClockLaneHS2LPTime = 27;
  //   PhyTimings.ClockLaneLP2HSTime = 32;
  //   PhyTimings.DataLaneHS2LPTime = 15;
  //   PhyTimings.DataLaneLP2HSTime = 24;
  //   PhyTimings.DataLaneMaxReadTime = 0;
  //   PhyTimings.StopWaitTime = 0;
  //   if (HAL_DSI_ConfigPhyTimer(hdsi, &PhyTimings) != HAL_OK) {
  //     return HAL_ERROR;
  //   }
  // }

  // if (HAL_DSI_ConfigFlowControl(hdsi, DSI_FLOW_CONTROL_BTA) != HAL_OK) {
  //   return HAL_ERROR;
  // }
  // if (HAL_DSI_SetLowPowerRXFilter(hdsi, 10000) != HAL_OK) {
  //   return HAL_ERROR;
  // }
  // if (HAL_DSI_ConfigErrorMonitor(hdsi, HAL_DSI_ERROR_GEN) != HAL_OK) {
  //   return HAL_ERROR;
  // }


  VidCfg.VirtualChannelID = 0;
  VidCfg.ColorCoding = PixelFormat;
  VidCfg.LooselyPacked = DSI_LOOSELY_PACKED_DISABLE;
  VidCfg.Mode = DSI_VID_MODE_BURST;
  VidCfg.PacketSize = DISPLAY_RESX;
  VidCfg.NumberOfChunks = 0;
  VidCfg.NullPacketSize = 0;
  VidCfg.HSPolarity = DSI_HSYNC_ACTIVE_HIGH;
  VidCfg.VSPolarity = DSI_VSYNC_ACTIVE_HIGH;
  VidCfg.DEPolarity = DSI_DATA_ENABLE_ACTIVE_HIGH;
  VidCfg.HorizontalSyncActive = LCD_HSYNC * DSI_FREQ / LTDC_FREQ;
  VidCfg.HorizontalBackPorch = LCD_HBP * DSI_FREQ / LTDC_FREQ;
  VidCfg.HorizontalLine =  LCD_H_TOTAL_PIXELS * DSI_FREQ / LTDC_FREQ;
  VidCfg.VerticalSyncActive = LCD_VSYNC;
  VidCfg.VerticalBackPorch = LCD_VBP;
  VidCfg.VerticalFrontPorch = LCD_VFP;
  VidCfg.VerticalActive = DISPLAY_RESY;
  VidCfg.LPCommandEnable = DSI_LP_COMMAND_DISABLE;
  VidCfg.LPLargestPacketSize = 0;
  VidCfg.LPVACTLargestPacketSize = 0;

  VidCfg.LPHorizontalFrontPorchEnable = DSI_LP_HFP_ENABLE;
  VidCfg.LPHorizontalBackPorchEnable = DSI_LP_HBP_ENABLE;
  VidCfg.LPVerticalActiveEnable = DSI_LP_VACT_ENABLE;
  VidCfg.LPVerticalFrontPorchEnable = DSI_LP_VFP_ENABLE;
  VidCfg.LPVerticalBackPorchEnable = DSI_LP_VBP_ENABLE;
  VidCfg.LPVerticalSyncActiveEnable = DSI_LP_VSYNC_ENABLE;
  VidCfg.FrameBTAAcknowledgeEnable = DSI_FBTAA_DISABLE;

  if (HAL_DSI_ConfigVideoMode(hdsi, &VidCfg) != HAL_OK) {
    return HAL_ERROR;
  }

  return HAL_OK;
}

HAL_StatusTypeDef ltdc_clock_config(LTDC_HandleTypeDef *hltdc) {
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;

  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
  PeriphClkInitStruct.PLL3.PLL3M = LTDC_PLL_M;
  PeriphClkInitStruct.PLL3.PLL3N = LTDC_PLL_N;
  PeriphClkInitStruct.PLL3.PLL3P = LTDC_PLL_P;
  PeriphClkInitStruct.PLL3.PLL3Q = LTDC_PLL_Q;
  PeriphClkInitStruct.PLL3.PLL3R = LTDC_PLL_R;
  PeriphClkInitStruct.PLL3.PLL3RGE = RCC_PLLCFGR_PLL3RGE_2;
  PeriphClkInitStruct.PLL3.PLL3VCOSEL = RCC_PLL3VCOWIDE;
  PeriphClkInitStruct.PLL3.PLL3FRACN = 0U;
  return HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);
}

HAL_StatusTypeDef ltdc_init(LTDC_HandleTypeDef *hltdc) {
  hltdc->Instance = LTDC;
  hltdc->Init.HSPolarity = LTDC_HSPOLARITY_AL;
  hltdc->Init.VSPolarity = LTDC_VSPOLARITY_AL;
  hltdc->Init.DEPolarity = LTDC_DEPOLARITY_AL;
  hltdc->Init.PCPolarity = LTDC_PCPOLARITY_IPC;

  hltdc->Init.HorizontalSync = LCD_HSYNC- 1;
  hltdc->Init.AccumulatedHBP = LCD_HSYNC + LCD_HBP - 1;
  hltdc->Init.AccumulatedActiveW = LCD_HSYNC + DISPLAY_RESX + LCD_HBP - 1;
  hltdc->Init.TotalWidth = LCD_H_TOTAL_PIXELS - 1;
  hltdc->Init.VerticalSync = LCD_VSYNC - 1;
  hltdc->Init.AccumulatedVBP = LCD_VSYNC + LCD_VBP - 1;
  hltdc->Init.AccumulatedActiveH = LCD_VSYNC + DISPLAY_RESY +LCD_VBP - 1;
  hltdc->Init.TotalHeigh = LCD_V_TOTAL_PIXELS - 1;

  hltdc->Init.Backcolor.Blue = 0x00;
  hltdc->Init.Backcolor.Green = 0x00;
  hltdc->Init.Backcolor.Red = 0x00;
  // HAL_LTDC_DeInit(hltdc);
  HAL_LTDC_Init(hltdc);

  lcd_enable_frame_it();
  return 0;
}

void LTDC_IRQHandler(void) {
  HAL_LTDC_IRQHandler(&hlcd_ltdc);
}

void HAL_LTDC_LineEventCallback(LTDC_HandleTypeDef *hltdc) {
  if (ltdc_buffer.updated) {
    ltdc_set_address(ltdc_buffer.rendered);
    // comsumed
    ltdc_buffer.updated = false;
  }

  ltdc_program_line(0);
}

void ltdc_buffer_switch(void) {
  if (ltdc_buffer.rendered == LTDC_BUFFER_1) {
    ltdc_buffer.rendered = LTDC_BUFFER_2;
    ltdc_buffer.rendering = LTDC_BUFFER_1;
  } else {
    ltdc_buffer.rendered = LTDC_BUFFER_1;
    ltdc_buffer.rendering = LTDC_BUFFER_2;
  }
  ltdc_buffer.updated = true;

  // wait ltdc switch buffer done
  while (ltdc_buffer.updated);

  // lvgl not render the full frame, copy the rendered as base view
  dma2d_copy_buffer((void*)ltdc_buffer.rendered, (void*)ltdc_buffer.rendering, 0, 0, DISPLAY_RESX, DISPLAY_RESY);

}

void* ltdc_buffer_rendered(void) {
  return (void*)ltdc_buffer.rendered;
}

void* ltdc_buffer_rendering(void) {
  return (void*)ltdc_buffer.rendering;
}

bool ltdc_buffer_idle(void) {
  return !ltdc_buffer.updated;
}

HAL_StatusTypeDef ltdc_layer_config(LTDC_HandleTypeDef *hltdc,
                                    uint32_t layer_index,
                                    LTDC_LAYERCONFIG *config) {
  LTDC_LayerCfgTypeDef pLayerCfg;

  pLayerCfg.WindowX0 = config->x0;
  pLayerCfg.WindowX1 = config->x1;
  pLayerCfg.WindowY0 = config->y0;
  pLayerCfg.WindowY1 = config->y1;
  pLayerCfg.PixelFormat = config->pixel_format;
  pLayerCfg.Alpha = 255;
  pLayerCfg.Alpha0 = 0;
  pLayerCfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_PAxCA;
  pLayerCfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;
  pLayerCfg.FBStartAdress = config->address;
  pLayerCfg.ImageWidth = (config->x1 - config->x0);
  pLayerCfg.ImageHeight = (config->y1 - config->y0);
  pLayerCfg.Backcolor.Blue = 0;
  pLayerCfg.Backcolor.Green = 0;
  pLayerCfg.Backcolor.Red = 0;
  return HAL_LTDC_ConfigLayer(hltdc, &pLayerCfg, layer_index);
}

int32_t bsp_get_tick(void) { return (int32_t)HAL_GetTick(); }

#define CONVERTRGB5652ARGB8888(Color)                                   \
  ((((((((Color) >> (11U)) & 0x1FU) * 527U) + 23U) >> (6U)) << (16U)) | \
   (((((((Color) >> (5U)) & 0x3FU) * 259U) + 33U) >> (6U)) << (8U)) |   \
   (((((Color)&0x1FU) * 527U) + 23U) >> (6U)) | (0xFF000000U))

void fb_read_pixel(uint32_t x_pos, uint32_t y_pos, uint32_t *color) {
  if (lcd_params.pixel_format == LTDC_PIXEL_FORMAT_ARGB8888) {
    /* Read data value from SDRAM memory */
    *color =
        *(uint32_t *)(lcd_params.fb_base +
                      (lcd_params.bbp * (lcd_params.xres * y_pos + x_pos)));

  } else {
    /*LTDC_PIXEL_FORMAT_RGB565 */
    *color = *(uint16_t *)(lcd_params.fb_base +
                      (lcd_params.bbp * (lcd_params.xres * y_pos + x_pos)));
  }
}

void fb_write_pixel(uint32_t x_pos, uint32_t y_pos, uint32_t color) {
  if (lcd_params.pixel_format == LTDC_PIXEL_FORMAT_ARGB8888) {
    *(uint32_t *)(lcd_params.fb_base +
                  (lcd_params.bbp * (lcd_params.xres * y_pos + x_pos))) = color;

  } else {
    /*LTDC_PIXEL_FORMAT_RGB565 */
    *(uint16_t *)(lcd_params.fb_base +
                  (lcd_params.bbp * (lcd_params.xres * y_pos + x_pos))) = color;
  }
}

static void fb_fill_buffer(uint32_t *dest, uint32_t x_size, uint32_t y_size,
                           uint32_t offset, uint32_t color) {
  uint32_t output_color_mode, input_color = color;

  switch (lcd_params.pixel_format) {
    case LTDC_PIXEL_FORMAT_RGB565:
      output_color_mode = DMA2D_OUTPUT_RGB565; /* RGB565 */
      input_color = CONVERTRGB5652ARGB8888(color);
      break;
    case LTDC_PIXEL_FORMAT_RGB888:
    default:
      output_color_mode = DMA2D_OUTPUT_ARGB8888; /* ARGB8888 */
      break;
  }

  /* Register to memory mode with ARGB8888 as color Mode */
  hlcd_dma2d.Init.Mode = DMA2D_R2M;
  hlcd_dma2d.Init.ColorMode = output_color_mode;
  hlcd_dma2d.Init.OutputOffset = offset;

  hlcd_dma2d.Instance = DMA2D;

  /* DMA2D Initialization */
  if (HAL_DMA2D_Init(&hlcd_dma2d) == HAL_OK) {
    if (HAL_DMA2D_ConfigLayer(&hlcd_dma2d, 1) == HAL_OK) {
      if (HAL_DMA2D_Start(&hlcd_dma2d, input_color, (uint32_t)dest, x_size,
                          y_size) == HAL_OK) {
        /* Polling For DMA transfer */
        (void)HAL_DMA2D_PollForTransfer(&hlcd_dma2d, HAL_MAX_DELAY);
      }
    }
  }
}

void fb_fill_rect(uint32_t x_pos, uint32_t y_pos, uint32_t width,
                  uint32_t height, uint32_t color) {
  /* Get the rectangle start address */
  uint32_t address = lcd_params.fb_base +
                     ((lcd_params.bbp) * (lcd_params.xres * y_pos + x_pos));

  /* Fill the rectangle */
  fb_fill_buffer((uint32_t *)address, width, height, (lcd_params.xres - width),
                 color);
}

void fb_draw_hline(uint32_t x_pos, uint32_t y_pos, uint32_t len,
                   uint32_t color) {
  uint32_t address = lcd_params.fb_base +
                     ((lcd_params.bbp) * (lcd_params.xres * y_pos + x_pos));
  fb_fill_buffer((uint32_t *)address, len, 1, 0, color);
}

void fb_draw_vline(uint32_t x_pos, uint32_t y_pos, uint32_t len,
                   uint32_t color) {
  uint32_t address = lcd_params.fb_base +
                     ((lcd_params.bbp) * (lcd_params.xres * y_pos + x_pos));
  fb_fill_buffer((uint32_t *)address, 1, len, lcd_params.xres - 1, color);
}

void dma2d_copy_buffer(void *pSrc, void* dst,  uint16_t x, uint16_t y,
                       uint16_t xsize, uint16_t ysize) {
  uint32_t destination = ((uint32_t) dst) + (y * lcd_params.xres + x) * (lcd_params.bbp);
  uint32_t source = (uint32_t)pSrc;

  /*##-1- Configure the DMA2D Mode, Color Mode and output offset #############*/
  hlcd_dma2d.Init.Mode = DMA2D_M2M;
  hlcd_dma2d.Init.ColorMode = DMA2D_OUTPUT_RGB565;
  hlcd_dma2d.Init.OutputOffset = lcd_params.xres - xsize;
  hlcd_dma2d.Init.AlphaInverted =
      DMA2D_REGULAR_ALPHA; /* No Output Alpha Inversion*/
  hlcd_dma2d.Init.RedBlueSwap =
      DMA2D_RB_REGULAR; /* No Output Red & Blue swap */

  /*##-2- DMA2D Callbacks Configuration ######################################*/
  hlcd_dma2d.XferCpltCallback = NULL;

  /*##-3- Foreground Configuration ###########################################*/
  hlcd_dma2d.LayerCfg[1].AlphaMode = DMA2D_NO_MODIF_ALPHA;
  hlcd_dma2d.LayerCfg[1].InputAlpha = 0xFF;
  hlcd_dma2d.LayerCfg[1].InputColorMode = DMA2D_INPUT_RGB565;
  hlcd_dma2d.LayerCfg[1].InputOffset = 0;
  hlcd_dma2d.LayerCfg[1].RedBlueSwap =
      DMA2D_RB_REGULAR; /* No ForeGround Red/Blue swap */
  hlcd_dma2d.LayerCfg[1].AlphaInverted =
      DMA2D_REGULAR_ALPHA; /* No ForeGround Alpha inversion */

  hlcd_dma2d.Instance = DMA2D;

  /* DMA2D Initialization */
  if (HAL_DMA2D_Init(&hlcd_dma2d) == HAL_OK) {
    if (HAL_DMA2D_ConfigLayer(&hlcd_dma2d, 1) == HAL_OK) {
      if (HAL_DMA2D_Start(&hlcd_dma2d, source, destination, xsize, ysize) ==
          HAL_OK) {
        /* Polling For DMA transfer */
        HAL_DMA2D_PollForTransfer(&hlcd_dma2d, HAL_MAX_DELAY);
      }
    }
  }
}

void dma2d_copy_ycbcr_to_rgb(uint32_t *pSrc, uint32_t *pDst, uint16_t xsize,
                             uint16_t ysize, uint32_t ChromaSampling) {
  uint32_t cssMode = DMA2D_CSS_420, inputLineOffset = 0;

  if (ChromaSampling == JPEG_420_SUBSAMPLING) {
    cssMode = DMA2D_CSS_420;

    inputLineOffset = xsize % 16;
    if (inputLineOffset != 0) {
      inputLineOffset = 16 - inputLineOffset;
    }
  } else if (ChromaSampling == JPEG_444_SUBSAMPLING) {
    cssMode = DMA2D_NO_CSS;

    inputLineOffset = xsize % 8;
    if (inputLineOffset != 0) {
      inputLineOffset = 8 - inputLineOffset;
    }
  } else if (ChromaSampling == JPEG_422_SUBSAMPLING) {
    cssMode = DMA2D_CSS_422;

    inputLineOffset = xsize % 16;
    if (inputLineOffset != 0) {
      inputLineOffset = 16 - inputLineOffset;
    }
  }

  /*##-1- Configure the DMA2D Mode, Color Mode and output offset #############*/
  hlcd_dma2d.Init.Mode = DMA2D_M2M_PFC;
  hlcd_dma2d.Init.ColorMode = DMA2D_OUTPUT_RGB565;
  hlcd_dma2d.Init.OutputOffset = 0;
  hlcd_dma2d.Init.AlphaInverted =
      DMA2D_REGULAR_ALPHA; /* No Output Alpha Inversion*/
  hlcd_dma2d.Init.RedBlueSwap =
      DMA2D_RB_REGULAR; /* No Output Red & Blue swap */

  /*##-2- DMA2D Callbacks Configuration ######################################*/
  hlcd_dma2d.XferCpltCallback = NULL;

  /*##-3- Foreground Configuration ###########################################*/
  hlcd_dma2d.LayerCfg[1].AlphaMode = DMA2D_REPLACE_ALPHA;
  hlcd_dma2d.LayerCfg[1].InputAlpha = 0xFF;
  hlcd_dma2d.LayerCfg[1].InputColorMode = DMA2D_INPUT_YCBCR;
  hlcd_dma2d.LayerCfg[1].ChromaSubSampling = cssMode;
  hlcd_dma2d.LayerCfg[1].InputOffset = inputLineOffset;
  hlcd_dma2d.LayerCfg[1].RedBlueSwap =
      DMA2D_RB_REGULAR; /* No ForeGround Red/Blue swap */
  hlcd_dma2d.LayerCfg[1].AlphaInverted =
      DMA2D_REGULAR_ALPHA; /* No ForeGround Alpha inversion */

  hlcd_dma2d.Instance = DMA2D;

  /*##-4- DMA2D Initialization     ###########################################*/
  HAL_DMA2D_Init(&hlcd_dma2d);
  HAL_DMA2D_ConfigLayer(&hlcd_dma2d, 1);

  HAL_DMA2D_Start(&hlcd_dma2d, (uint32_t)pSrc, (uint32_t)pDst, xsize, ysize);
  HAL_DMA2D_PollForTransfer(
      &hlcd_dma2d, 25); /* wait for the previous DMA2D transfer to ends */
}

void gc9503v_dsi_write(uint16_t reg, uint8_t *seq, uint16_t len) {
  if (len == 0) {
    HAL_DSI_ShortWrite(&hlcd_dsi, 0, DSI_DCS_SHORT_PKT_WRITE_P0, reg, 0);
  }
  if (len <= 1) {
    HAL_DSI_ShortWrite(&hlcd_dsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, reg,
                       (uint32_t)seq[0]);
  } else {
    HAL_DSI_LongWrite(&hlcd_dsi, 0, DSI_DCS_LONG_PKT_WRITE, len, reg, seq);
  }
}

void st7701s_dsi_write(uint16_t reg, uint8_t *seq, uint16_t len) {
  if (len <= 1) {
    HAL_DSI_ShortWrite(&hlcd_dsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, reg,
                       (uint32_t)seq[0]);
  } else {
    HAL_DSI_LongWrite(&hlcd_dsi, 0, DSI_DCS_LONG_PKT_WRITE, len, reg, seq);
  }
}

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

#define st7701s_dsi(reg, seq...)              \
  {                                          \
    uint8_t d[] = {seq};                     \
    st7701s_dsi_write(reg, d, ARRAY_SIZE(d)); \
  }

void st7701s_init_sequence(void){
st7701s_dsi(0xFF, 0x77, 0x01, 0x00, 0x00, 0x13);
st7701s_dsi(0xEF, 0x08);
st7701s_dsi(0xFF, 0x77, 0x01, 0x00, 0x00, 0x10);
st7701s_dsi(0xC0, 0x63, 0x00);
st7701s_dsi(0xC1, 0x14, 0x0C);
st7701s_dsi(0xC2, 0x37, 0x02);
st7701s_dsi(0xCC, 0x10);
st7701s_dsi(0xB0, 0x06, 0x10, 0x16, 0x0D, 0x11, 0x06, 0x08, 0x07, 0x08, 0x22, 0x04, 0x14, 0x0F, 0x29, 0x2F, 0x1F);
st7701s_dsi(0xB1, 0x0F, 0x18, 0x1E, 0x0C, 0x0F, 0x06, 0x08, 0x0A, 0x09, 0x24, 0x05, 0x10, 0x11, 0x2A, 0x34, 0x1F);
st7701s_dsi(0xFF, 0x77, 0x01, 0x00, 0x00, 0x11);
st7701s_dsi(0xB0, 0x4D);
st7701s_dsi(0xB1, 0x4D);
st7701s_dsi(0xB2, 0x81);
st7701s_dsi(0xB3, 0x80);
st7701s_dsi(0xB5, 0x4E);
st7701s_dsi(0xB7, 0x85);
st7701s_dsi(0xB8, 0x32);
st7701s_dsi(0xBB, 0x03);
st7701s_dsi(0xC1, 0x08);
st7701s_dsi(0xC2, 0x08);
st7701s_dsi(0xD0, 0x88);
st7701s_dsi(0xE0, 0x00, 0x00, 0x02);
st7701s_dsi(0xE1, 0x06, 0x28, 0x08, 0x28, 0x05, 0x28, 0x07, 0x28, 0x0E, 0x33, 0x33);
st7701s_dsi(0xE2, 0x30, 0x30, 0x33, 0x33, 0x34, 0x00, 0x00, 0x00, 0x34, 0x00, 0x00, 0x00);
st7701s_dsi(0xE3, 0x00, 0x00, 0x33, 0x33);
st7701s_dsi(0xE4, 0x44, 0x44);
st7701s_dsi(0xE5, 0x09, 0x2F, 0x2C, 0x8C, 0x0B, 0x31, 0x2C, 0x8C, 0x0D, 0x33, 0x2C, 0x8C, 0x0F, 0x35, 0x2C, 0x8C);
st7701s_dsi(0xE6, 0x00, 0x00, 0x33, 0x33);
st7701s_dsi(0xE7, 0x44, 0x44);
st7701s_dsi(0xE8, 0x08, 0x2E, 0x2C, 0x8C, 0x0A, 0x30, 0x2C, 0x8C, 0x0C, 0x32, 0x2C, 0x8C, 0x0E, 0x34, 0x2C, 0x8C);
st7701s_dsi(0xE9, 0x36, 0x00);
st7701s_dsi(0xEB, 0x00, 0x01, 0xE4, 0xE4, 0x44, 0x88, 0x40);
st7701s_dsi(0xED, 0xFF, 0xFC, 0xB2, 0x45, 0x67, 0xFA, 0x01, 0xFF, 0xFF, 0x10, 0xAF, 0x76, 0x54, 0x2B, 0xCF, 0xFF);
st7701s_dsi(0xEF, 0x08, 0x08, 0x08, 0x45, 0x3F, 0x54);
st7701s_dsi(0xFF, 0x77, 0x01, 0x00, 0x00, 0x13);
st7701s_dsi(0xE8, 0x00, 0x0E);
st7701s_dsi(0x11);
HAL_Delay(120);
st7701s_dsi(0xE8, 0x00, 0x0C);
HAL_Delay(10);
st7701s_dsi(0xE8, 0x00, 0x00);
st7701s_dsi(0xFF, 0x77, 0x01, 0x00, 0x00, 0x00);
st7701s_dsi(0x36, 0x00);
st7701s_dsi(0x29);
HAL_Delay(20);
st7701s_dsi(0xFF, 0x77, 0x01, 0x00, 0x00, 0x10);
st7701s_dsi(0xE5, 0x00, 0x00);
}

void gc9503v_init_sequence(void) {
  typedef struct
  {
    int cmd;             /*<! The specific LCD command */
    size_t data_bytes;   /*<! Size of `data` in memory, in bytes */
    uint8_t* data; /*<! Buffer that holds the command specific data */
  } gc9503_lcd_init_cmd_t;

  // clang-format off
  gc9503_lcd_init_cmd_t init_cmds[] = {
    /*
      {0xF0, 5, (uint8_t[]){0x55, 0xaa, 0x52, 0x08, 0x00}},
      {0xF6, 2, (uint8_t[]){0x5A, 0x87}},
      {0xc1, 1, (uint8_t[]){0x3f}},
      {0xcd, 1, (uint8_t[]){0x25}},
      {0xc9, 1, (uint8_t[]){0x10}},
      {0xf8, 1, (uint8_t[]){0x8a}},
      {0xac, 1, (uint8_t[]){0x45}},
      {0xa7, 1, (uint8_t[]){0x47}},
      {0xa0, 1, (uint8_t[]){0x77}},
      {0x87, 3, (uint8_t[]){0x04, 0x03, 0x66}},
      {0x86, 4, (uint8_t[]){0x99, 0xa3, 0xa3, 0x31}},
      {0xfa, 4, (uint8_t[]){0x08, 0x08, 0x00, 0x04}},
      {0xA3, 1, (uint8_t[]){0x6E}},
      {0xFD, 3, (uint8_t[]){0x28, 0x3C, 0x00}},
      {0xb1, 1, (uint8_t[]){0x00}},
      {0x6d, 32, (uint8_t[]){0x1E, 0x1E, 0x1E, 0x0C, 0x0B, 0x0A, 0x09, 0x1e, 0x01, 0x1F, 0x00,
                             0x07, 0x0D, 0x0F, 0x1E, 0x1E, 0x1E, 0x1E, 0x10, 0x0E, 0x08, 0x00,
                             0x1F, 0x02, 0x1e, 0x11, 0x12, 0x13, 0x14, 0x1e, 0x1E, 0x1E}},
      {0x64, 16, (uint8_t[]){0x38, 0x09, 0x03, 0x1e, 0x03, 0x03, 0x38, 0x07, 0x03, 0x20, 0x03, 0x03, 0x7a, 0x7a, 0x7a, 0x7a}},
      {0x65, 16, (uint8_t[]){0x38, 0x05, 0x03, 0x22, 0x03, 0x03, 0x38, 0x03, 0x03, 0x24, 0x03, 0x03, 0x7a, 0x7a, 0x7a, 0x7a}},
      {0x66, 16, (uint8_t[]){0x98, 0x11, 0x03, 0x37, 0x03, 0x03, 0x98, 0x11, 0x03, 0x37, 0x03, 0x03, 0x7a, 0x7a, 0x7a, 0x7a}},
      {0x67, 16, (uint8_t[]){0x93, 0x37, 0x08, 0x11, 0x03, 0x03, 0x93, 0x37, 0x08, 0x11, 0x03, 0x03, 0x7a, 0x7a, 0x7a, 0x7a}},
      {0x60, 8, (uint8_t[]){0x38, 0x0d, 0x7a, 0x7a, 0x38, 0x0c, 0x7a, 0x7a}},
      {0x63, 8, (uint8_t[]){0x38, 0x0b, 0x7a, 0x7a, 0x38, 0x0a, 0x7a, 0x7a}},
      {0x69, 7, (uint8_t[]){0x14, 0x22, 0x14, 0x22, 0x44, 0x22, 0x08}},
      {0x6b, 1, (uint8_t[]){0x07}},
      {0x7A, 2, (uint8_t[]){0x0F, 0x13}},
      {0x7B, 2, (uint8_t[]){0x0F, 0x13}},
      {0xD1, 52, (uint8_t[]){0x00, 0x00, 0x00, 0x10, 0x00, 0x2C, 0x00, 0x45, 0x00, 0x58, 0x00, 0x7A, 0x00,
                             0x98, 0x00, 0xC3, 0x00, 0xE6, 0x01, 0x22, 0x01, 0x58, 0x01, 0xAE, 0x01, 0xf6,
                             0x01, 0xf8, 0x02, 0x3A, 0x02, 0x8A, 0x02, 0xBD, 0x03, 0x0B, 0x03, 0x40, 0x03,
                             0x9C, 0x03, 0xD1, 0x03, 0xE1, 0x03, 0xed, 0x03, 0xf0, 0x03, 0xff, 0x03, 0xFF}},
      {0xD2, 52, (uint8_t[]){0x00, 0x00, 0x00, 0x10, 0x00, 0x2C, 0x00, 0x45, 0x00, 0x58, 0x00, 0x7A, 0x00,
                             0x98, 0x00, 0xC3, 0x00, 0xE6, 0x01, 0x22, 0x01, 0x58, 0x01, 0xAE, 0x01, 0xf6,
                             0x01, 0xf8, 0x02, 0x3A, 0x02, 0x8A, 0x02, 0xBD, 0x03, 0x0B, 0x03, 0x40, 0x03,
                             0x9C, 0x03, 0xD1, 0x03, 0xE1, 0x03, 0xed, 0x03, 0xf0, 0x03, 0xff, 0x03, 0xFF}},
      {0xD3, 52, (uint8_t[]){0x00, 0x00, 0x00, 0x10, 0x00, 0x2C, 0x00, 0x45, 0x00, 0x58, 0x00, 0x7A, 0x00,
                             0x98, 0x00, 0xC3, 0x00, 0xE6, 0x01, 0x22, 0x01, 0x58, 0x01, 0xAE, 0x01, 0xf6,
                             0x01, 0xf8, 0x02, 0x3A, 0x02, 0x8A, 0x02, 0xBD, 0x03, 0x0B, 0x03, 0x40, 0x03,
                             0x9C, 0x03, 0xD1, 0x03, 0xE1, 0x03, 0xed, 0x03, 0xf0, 0x03, 0xff, 0x03, 0xFF}},
      {0xD4, 52, (uint8_t[]){0x00, 0x00, 0x00, 0x10, 0x00, 0x2C, 0x00, 0x45, 0x00, 0x58, 0x00, 0x7A, 0x00,
                             0x98, 0x00, 0xC3, 0x00, 0xE6, 0x01, 0x22, 0x01, 0x58, 0x01, 0xAE, 0x01, 0xf6,
                             0x01, 0xf8, 0x02, 0x3A, 0x02, 0x8A, 0x02, 0xBD, 0x03, 0x0B, 0x03, 0x40, 0x03,
                             0x9C, 0x03, 0xD1, 0x03, 0xE1, 0x03, 0xed, 0x03, 0xf0, 0x03, 0xff, 0x03, 0xFF}},
      {0xD5, 52, (uint8_t[]){0x00, 0x00, 0x00, 0x10, 0x00, 0x2C, 0x00, 0x45, 0x00, 0x58, 0x00, 0x7A, 0x00,
                             0x98, 0x00, 0xC3, 0x00, 0xE6, 0x01, 0x22, 0x01, 0x58, 0x01, 0xAE, 0x01, 0xf6,
                             0x01, 0xf8, 0x02, 0x3A, 0x02, 0x8A, 0x02, 0xBD, 0x03, 0x0B, 0x03, 0x40, 0x03,
                             0x9C, 0x03, 0xD1, 0x03, 0xE1, 0x03, 0xed, 0x03, 0xf0, 0x03, 0xff, 0x03, 0xFF}},
      {0xD6, 52, (uint8_t[]){0x00, 0x00, 0x00, 0x10, 0x00, 0x2C, 0x00, 0x45, 0x00, 0x58, 0x00, 0x7A, 0x00,
                             0x98, 0x00, 0xC3, 0x00, 0xE6, 0x01, 0x22, 0x01, 0x58, 0x01, 0xAE, 0x01, 0xf6,
                             0x01, 0xf8, 0x02, 0x3A, 0x02, 0x8A, 0x02, 0xBD, 0x03, 0x0B, 0x03, 0x40, 0x03,
                             0x9C, 0x03, 0xD1, 0x03, 0xE1, 0x03, 0xed, 0x03, 0xf0, 0x03, 0xff, 0x03, 0xFF}},
                             */
    {0xF0,5,(uint8_t[]){0x55,0xaa,0x52,0x08,0x00}},
    {0xF6,2,(uint8_t[]){0x5a,0x87}},
    {0xC1,1,(uint8_t[]){0x3F}},
    {0xCD,1,(uint8_t[]){0x25}},
    {0xC9,1,(uint8_t[]){0x10}},
    {0xF8,1,(uint8_t[]){0x8A}},
    {0xAC,1,(uint8_t[]){0x45}},
    {0xA7,1,(uint8_t[]){0x47}},
    {0xA0,1,(uint8_t[]){0x77}},
    {0x86,4,(uint8_t[]){0x99,0xA3,0xA3,0x41}},
    {0xFA,4,(uint8_t[]){0x08,0x08,0x00,0x04}},
    {0xA3,1,(uint8_t[]){0x6E}},
    {0xFD,3,(uint8_t[]){0x28,0x3C,0x00}},
    {0x9A,1,(uint8_t[]){0x70}},
    {0x9B,1,(uint8_t[]){0x20}},
    {0x82,2,(uint8_t[]){0x22,0x22}},
    {0x80,1,(uint8_t[]){0x18}},
    {0xB1,1,(uint8_t[]){0x10}},
    {0x7A,2,(uint8_t[]){0x0F,0x13}},
    {0x7B,2,(uint8_t[]){0x0F,0x13}},
    {0x69,7,(uint8_t[]){0x14,0x22,0x14,0x22,0x44,0x22,0x08}},
    {0x6B,1,(uint8_t[]){0x07}},
    {0x6D,32, (uint8_t[]){0x1e,0x1e,0x1f,0x00,0x05,0x06,0x13,0x14,0x11,0x12,0x1e,0x0e,0x02,0x08,0x1E,0x1E,0x1E,0x1E,0x07,0x01,0x0d,0x1e,0x0a,0x09,0x0c,0x0b,0x03,0x04,0x00,0x1f,0x1e,0x1e}},
    {0x60,8, (uint8_t[]){0x38,0x0C,0x1C,0x64,0x38,0x0B,0x1C,0x64}},
    {0x61,8, (uint8_t[]){0x33,0x22,0x64,0x1C,0x53,0x28,0x1C,0x64}},
    {0x62,8, (uint8_t[]){0x53,0x28,0x1C,0x64,0x33,0x22,0x64,0x1C}},
    {0x63,8, (uint8_t[]){0x38,0x0A,0x1C,0x64,0x38,0x09,0x1C,0x64}},
    {0x64,16, (uint8_t[]){0x38,0x08,0x03,0x20,0x03,0x03,0x38,0x06,0x03,0x22,0x03,0x03,0x1C,0x64,0x1C,0x64}},
    {0x65,16, (uint8_t[]){0x38,0x04,0x03,0x24,0x03,0x03,0x38,0x02,0x03,0x26,0x03,0x03,0x1C,0x64,0x1C,0x64}},
    {0x66,16, (uint8_t[]){0x83,0x2e,0x03,0x27,0x03,0x03,0x83,0x2e,0x03,0x27,0x03,0x03,0x1C,0x64,0x1C,0x64}},
    {0xD1,52, (uint8_t[]){0x00,0x00,0x00,0x2c,0x00,0x57,0x00,0x8f,0x00,0xa4,0x00,0xc3,0x00,0xdc,0x01,0x0a,0x01,0x35,0x01,0x7a,0x01,0xaf,0x02,0x05,0x02,0x4a,0x02,0x4d,0x02,0x8b,0x02,0xcc,0x02,0xfa,0x03,0x33,0x03,0x59,0x03,0x8f,0x03,0xbc,0x03,0xcc,0x03,0xdd,0x03,0xe8,0x03,0xf0,0x03,0xFF}},
    {0xD2,52, (uint8_t[]){0x00,0x00,0x00,0x2c,0x00,0x57,0x00,0x8f,0x00,0xa4,0x00,0xc3,0x00,0xdc,0x01,0x0a,0x01,0x35,0x01,0x7a,0x01,0xaf,0x02,0x05,0x02,0x4a,0x02,0x4d,0x02,0x8b,0x02,0xcc,0x02,0xfa,0x03,0x33,0x03,0x59,0x03,0x8f,0x03,0xbc,0x03,0xcc,0x03,0xdd,0x03,0xe8,0x03,0xf0,0x03,0xFF}},
    {0xD3,52, (uint8_t[]){0x00,0x00,0x00,0x2c,0x00,0x57,0x00,0x8f,0x00,0xa4,0x00,0xc3,0x00,0xdc,0x01,0x0a,0x01,0x35,0x01,0x7a,0x01,0xaf,0x02,0x05,0x02,0x4a,0x02,0x4d,0x02,0x8b,0x02,0xcc,0x02,0xfa,0x03,0x33,0x03,0x59,0x03,0x8f,0x03,0xbc,0x03,0xcc,0x03,0xdd,0x03,0xe8,0x03,0xf0,0x03,0xFF}},
    {0xD4,52, (uint8_t[]){0x00,0x00,0x00,0x2c,0x00,0x57,0x00,0x8f,0x00,0xa4,0x00,0xc3,0x00,0xdc,0x01,0x0a,0x01,0x35,0x01,0x7a,0x01,0xaf,0x02,0x05,0x02,0x4a,0x02,0x4d,0x02,0x8b,0x02,0xcc,0x02,0xfa,0x03,0x33,0x03,0x59,0x03,0x8f,0x03,0xbc,0x03,0xcc,0x03,0xdd,0x03,0xe8,0x03,0xf0,0x03,0xFF}},
    {0xD5,52, (uint8_t[]){0x00,0x00,0x00,0x2c,0x00,0x57,0x00,0x8f,0x00,0xa4,0x00,0xc3,0x00,0xdc,0x01,0x0a,0x01,0x35,0x01,0x7a,0x01,0xaf,0x02,0x05,0x02,0x4a,0x02,0x4d,0x02,0x8b,0x02,0xcc,0x02,0xfa,0x03,0x33,0x03,0x59,0x03,0x8f,0x03,0xbc,0x03,0xcc,0x03,0xdd,0x03,0xe8,0x03,0xf0,0x03,0xFF}},
    {0xD6,52, (uint8_t[]){0x00,0x00,0x00,0x2c,0x00,0x57,0x00,0x8f,0x00,0xa4,0x00,0xc3,0x00,0xdc,0x01,0x0a,0x01,0x35,0x01,0x7a,0x01,0xaf,0x02,0x05,0x02,0x4a,0x02,0x4d,0x02,0x8b,0x02,0xcc,0x02,0xfa,0x03,0x33,0x03,0x59,0x03,0x8f,0x03,0xbc,0x03,0xcc,0x03,0xdd,0x03,0xe8,0x03,0xf0,0x03,0xFF}},
  };
  // clang-format on

  gc9503_lcd_init_cmd_t *cmd = init_cmds;
  int cmds_count = sizeof(init_cmds) / sizeof(gc9503_lcd_init_cmd_t);

  // init commands
  for (int i = 0; i < cmds_count; i++) {
    cmd = init_cmds + i;
    gc9503v_dsi_write( cmd->cmd, cmd->data, cmd->data_bytes);
    HAL_Delay(1);
  };

  // pixel format
  uint8_t pf = GC9503V_FORMAT_RGB565;
  gc9503v_dsi_write(MIPI_DCS_SET_PIXEL_FORMAT, &pf, 1);

  // sleep out
  gc9503v_dsi_write(MIPI_DCS_EXIT_SLEEP_MODE, NULL, 0);
  // wait for sleep out exit
  HAL_Delay(120);

  // display on
  gc9503v_dsi_write(MIPI_DCS_SET_DISPLAY_ON, NULL, 0);

  HAL_Delay(120);
}

// valid values: 0~50%, the brightness will not change when greater than 50%
// (0~100)/200 = 0~50%
#define LED_PWM_TIM_PERIOD (200)

int display_backlight(int val) {
  if (DISPLAY_BACKLIGHT != val && val >= 0 && val <= 100) {
    DISPLAY_BACKLIGHT = val;
    TIM1->CCR2 = val;
  }
  return DISPLAY_BACKLIGHT;
}

int display_orientation(int degrees) {
  if (degrees != DISPLAY_ORIENTATION) {
    if (degrees == 0 || degrees == 90 || degrees == 180 || degrees == 270) {
      DISPLAY_ORIENTATION = degrees;
    }
  }
  return DISPLAY_ORIENTATION;
}

void lcd_reset(void) {
  //Hypermate的Touch和LCD共用一个RST，在Touch中RST了，这里面就不用了

  // // RESET PIN
  // GPIO_InitTypeDef gpio_init_structure;
  // GPIO_TypeDef* port = NULL;
  // uint32_t pin = 0;

  // not used, used `lcd_touch_reset`
  // LCD_RESET_CLK_ENABLE();
  // port = LCD_RESET_GPIO_PORT;
  // pin = LCD_RESET_PIN;

  // /* Configure the GPIO Reset pin */
  // gpio_init_structure.Pin = pin;
  // gpio_init_structure.Mode = GPIO_MODE_OUTPUT_PP;
  // gpio_init_structure.Pull = GPIO_PULLUP;
  // gpio_init_structure.Speed = GPIO_SPEED_FREQ_LOW;
  // HAL_GPIO_Init(port, &gpio_init_structure);

  // /* Activate XRES active low */
  // HAL_GPIO_WritePin(port, pin, GPIO_PIN_RESET);
  // HAL_Delay(20);
  // /* Deactivate XRES */
  // HAL_GPIO_WritePin(port, pin, GPIO_PIN_SET);
  // HAL_Delay(10);
}

void lcd_init(uint32_t lcd_width, uint32_t lcd_height, uint32_t pixel_format) {

  lcd_reset();
  ltdc_buffer_init();
  lcd_para_init(DISPLAY_RESX, DISPLAY_RESY, LCD_PIXEL_FORMAT_RGB565);

  /* LCD_TE_CTRL GPIO configuration */
  __HAL_RCC_GPIOJ_CLK_ENABLE();

  GPIO_InitTypeDef gpio_init_structure;

  gpio_init_structure.Pin       = LCD_BL_CTRL_PIN;
  gpio_init_structure.Mode      = GPIO_MODE_OUTPUT_PP;
  gpio_init_structure.Speed     = GPIO_SPEED_FREQ_HIGH;
  gpio_init_structure.Pull      = GPIO_NOPULL;

  gpio_init_structure.Speed     = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LCD_BL_CTRL_GPIO_PORT, &gpio_init_structure);
  /* Assert back-light LCD_BL_CTRL pin */
  HAL_GPIO_WritePin(LCD_BL_CTRL_GPIO_PORT, LCD_BL_CTRL_PIN, GPIO_PIN_RESET);

  gpio_init_structure.Pin = LCD_TE_PIN;
  gpio_init_structure.Mode = GPIO_MODE_INPUT;
  gpio_init_structure.Speed = GPIO_SPEED_FREQ_HIGH;

  HAL_GPIO_Init(LCD_TE_GPIO_PORT, &gpio_init_structure);
  /* Assert back-light LCD_BL_CTRL pin */
  HAL_GPIO_WritePin(LCD_TE_GPIO_PORT, LCD_TE_PIN, GPIO_PIN_SET);

  /* Initializes peripherals instance value */
  hlcd_ltdc.Instance = LTDC;
  hlcd_dma2d.Instance = DMA2D;
  hlcd_dsi.Instance = DSI;

  uint32_t ltdc_pixel_format, dsi_pixel_format;

  if (pixel_format == LCD_PIXEL_FORMAT_RGB565) {
    ltdc_pixel_format = LTDC_PIXEL_FORMAT_RGB565;
    dsi_pixel_format = DSI_RGB565;
    lcd_params.bbp = 2;
  } else {
    ltdc_pixel_format = LCD_PIXEL_FORMAT_ARGB8888;
    dsi_pixel_format = DSI_RGB888;
    lcd_params.bbp = 4;
  }

  lcd_params.pixel_format = ltdc_pixel_format;
  lcd_params.xres = lcd_width;
  lcd_params.yres = lcd_height;
  lcd_params.fb_base = ltdc_buffer.rendered;

  ltdc_msp_init(&hlcd_ltdc);

  dma2d_msp_init(&hlcd_dma2d);

  dsi_msp_init(&hlcd_dsi);

  dsi_host_init(&hlcd_dsi, dsi_pixel_format);

  ltdc_clock_config(&hlcd_ltdc);

  ltdc_init(&hlcd_ltdc);

  LTDC_LAYERCONFIG config;

  config.x0 = 0;
  config.x1 = lcd_width;
  config.y0 = 0;
  config.y1 = lcd_height;
  config.pixel_format = ltdc_pixel_format;
  config.address = ltdc_buffer.rendered;

  ltdc_layer_config(&hlcd_ltdc, 0, &config);

  /* Enable the DSI host and wrapper after the LTDC initialization
        To avoid any synchronization issue, the DSI shall be started after
     enabling the LTDC */
  (void)HAL_DSI_Start(&hlcd_dsi);

  /* Enable the DSI BTW for read operations */
  (void)HAL_DSI_ConfigFlowControl(&hlcd_dsi, DSI_FLOW_CONTROL_BTA);

#if defined (LCD_USE_ST7701S)
  st7701s_init_sequence();
#elif defined (LCD_USE_GC9503V)
  gc9503v_init_sequence();
#endif
}

void lcd_pwm_init(void) {
  __HAL_RCC_GPIOJ_CLK_ENABLE();
  __HAL_RCC_TIM1_CLK_ENABLE();

  GPIO_InitTypeDef gpio = {0};
  /* LCD_BL_CTRL GPIO configuration */
  // LCD_PWM/PJ11 (backlight control)
  gpio.Mode = GPIO_MODE_AF_PP;
  gpio.Pull = GPIO_NOPULL;
  gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  gpio.Alternate = GPIO_AF1_TIM1;
  gpio.Pin = LCD_BL_CTRL_PIN;
  HAL_GPIO_DeInit(LCD_BL_CTRL_GPIO_PORT, LCD_BL_CTRL_PIN);
  HAL_GPIO_Init(LCD_BL_CTRL_GPIO_PORT, &gpio);

  // enable PWM timer
  TIM_HandleTypeDef tim1 = {0};
  tim1.Instance = TIM1;
  tim1.Init.Period = LED_PWM_TIM_PERIOD - 1;
  // 10 KHz
  tim1.Init.Prescaler = SystemCoreClock / 2 / 10000000 - 1;
  tim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  tim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  tim1.Init.RepetitionCounter = 0;
  HAL_TIM_PWM_Init(&tim1);

  TIM_OC_InitTypeDef TIM_OC_InitStructure = {0};
  TIM_OC_InitStructure.Pulse = LED_PWM_TIM_PERIOD;
  TIM_OC_InitStructure.OCMode = TIM_OCMODE_PWM1;
  TIM_OC_InitStructure.OCPolarity = TIM_OCPOLARITY_HIGH;
  TIM_OC_InitStructure.OCFastMode = TIM_OCFAST_DISABLE;
  TIM_OC_InitStructure.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  TIM_OC_InitStructure.OCIdleState = TIM_OCIDLESTATE_SET;
  TIM_OC_InitStructure.OCNIdleState = TIM_OCNIDLESTATE_SET;
  HAL_TIM_PWM_ConfigChannel(&tim1, &TIM_OC_InitStructure, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&tim1, TIM_CHANNEL_2);
}

void lcd_para_init(uint32_t lcd_width, uint32_t lcd_height,
                   uint32_t pixel_format) {
  uint32_t ltdc_pixel_format;

  if (pixel_format == LCD_PIXEL_FORMAT_RGB565) {
    ltdc_pixel_format = LTDC_PIXEL_FORMAT_RGB565;
    lcd_params.bbp = 2;

  } else {
    ltdc_pixel_format = LCD_PIXEL_FORMAT_ARGB8888;
    lcd_params.bbp = 4;
  }

  lcd_params.pixel_format = ltdc_pixel_format;
  lcd_params.xres = lcd_width;
  lcd_params.yres = lcd_height;
  lcd_params.fb_base = ltdc_buffer.rendered;
}

void lcd_enable_frame_it(void) {
  __HAL_LTDC_ENABLE_IT(&hlcd_ltdc, LTDC_IT_LI);
  HAL_NVIC_SetPriority(LTDC_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(LTDC_IRQn);
  ltdc_program_line(0);
}

void display_set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {}

int display_get_orientation(void) { return DISPLAY_ORIENTATION; }
void display_reset_state() {}
void display_clear_save(void) {}
const char *display_save(const char *prefix) { return NULL; }
void display_refresh(void) {}
