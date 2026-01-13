
#include STM32_HAL_H

#include "gt911.h"
#include "common.h"
#include "i2c.h"

// static uint8_t gt911_data[256];

void gt911_io_init(void) {
  TOUCH_LCD_RESET_CLK_ENABLE();
  TOUCH_INT_CLK_ENABLE();

  GPIO_InitTypeDef GPIO_InitStructure;

  /* Configure the GPIO Reset pin */
  GPIO_InitStructure.Pin = TOUCH_LCD_RESET_PIN;
  GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStructure.Pull = GPIO_PULLUP;
  GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(TOUCH_LCD_RESET_GPIO_PORT, &GPIO_InitStructure);

  /* Configure the GPIO Interrupt pin */
  GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
  GPIO_InitStructure.Pull = GPIO_PULLUP;
  GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStructure.Pin = TOUCH_INT_PIN;
  HAL_GPIO_Init(TOUCH_INT_GPIO_PORT, &GPIO_InitStructure);

  HAL_GPIO_WritePin(TOUCH_INT_GPIO_PORT, TOUCH_INT_PIN, GPIO_PIN_SET);
}
void gt911_reset(void) {
  GPIO_InitTypeDef GPIO_InitStructure;

  GPIO_InitStructure.Pin = TOUCH_INT_PIN;
  GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStructure.Pull = GPIO_PULLUP;
  GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(TOUCH_INT_GPIO_PORT, &GPIO_InitStructure);

  HAL_GPIO_WritePin(TOUCH_INT_GPIO_PORT, TOUCH_INT_PIN, GPIO_PIN_RESET);

  HAL_GPIO_WritePin(TOUCH_LCD_RESET_GPIO_PORT, TOUCH_LCD_RESET_PIN, GPIO_PIN_RESET);
  HAL_Delay(10);
  HAL_GPIO_WritePin(TOUCH_LCD_RESET_GPIO_PORT, TOUCH_LCD_RESET_PIN, GPIO_PIN_SET);
  HAL_Delay(100);

  GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
  GPIO_InitStructure.Pull = GPIO_NOPULL;
  GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStructure.Pin = TOUCH_INT_PIN;
  HAL_GPIO_Init(TOUCH_INT_GPIO_PORT, &GPIO_InitStructure);
}

void gt911_read(uint16_t reg_addr, uint8_t *buf, uint16_t len) {
  if (HAL_I2C_Mem_Read(&hi2c4, GT911_ADDR, reg_addr, 2, buf, len,
                       1000) != HAL_OK) {
    // ensure(secfalse, "gt911 read failed");
  }
}

void gt911_write(uint16_t reg_addr, uint8_t *buf, uint16_t len) {
  if (HAL_I2C_Mem_Write(&hi2c4, GT911_ADDR, reg_addr, 2, buf,
                        len, 1000) != HAL_OK) {
    // ensure(secfalse, "gt911 write failed");
  }
}

// return one point data only
uint32_t gt911_read_location(void) {
  uint8_t point_data[10] = {0};
  uint8_t point_num;
  uint16_t x = 0, y = 0;
  static uint32_t xy = 0;

  static uint8_t last_point_num = 0;

  gt911_read(GTP_READ_COOR_ADDR, point_data, 10);
  if (point_data[0] == 0x00) {
    return xy;
  }

  if (point_data[0] == 0x80) {
    point_data[0] = 0;
    gt911_write(GTP_READ_COOR_ADDR, point_data, 1);
    last_point_num = 0;
    xy = 0;
    return 0;
  }
  point_num = point_data[0] & 0x0f;

  if (last_point_num == 0 && point_num == 1) {
    last_point_num = point_num;
  }

  if (point_num && last_point_num == 1) {
    x = point_data[2] | (point_data[3] << 8);
    y = point_data[4] | (point_data[5] << 8);
  }

  point_data[0] = 0;
  gt911_write(GTP_READ_COOR_ADDR, point_data, 1);

  xy = x << 16 | y;

  return xy;
}

void gt911_enter_sleep(void) {
  uint8_t data[1] = {0x05};
  gt911_write(GTP_REG_COMMAND, data, 1);
}

void gt911_enable_irq(void) {
}

void gt911_disable_irq(void) { }

void gt911_test(void) {
  while (1) {
    gt911_read_location();
  }
}

static uint8_t default_config[] = {0x43,0xE0,0x01,0x20,0x03,0x05,0xC4,0x00,0x01,0x0F,0x28,0x0F,0x5A,0x46,0x03,0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x87,0x27,0x09,0x3C,0x3E,0xEB,0x04,0x00,0x00,0x00,0x01,0x02,0x2C,0x00,0x01,0x00,0x00,0x00,0x03,0x64,0x32,0x00,0x00,0x00,0x28,0x4B,0x94,0xD5,0x02,0x07,0x00,0x00,0x04,0xB3,0x2A,0x00,0x9E,0x30,0x00,0x8D,0x36,0x00,0x7C,0x3E,0x00,0x6E,0x46,0x00,0x6E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x04,0x06,0x08,0x0A,0x0C,0x0E,0x10,0x12,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x04,0x06,0x08,0x0A,0x0C,0x1D,0x1E,0x1F,0x20,0x21,0x22,0x24,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x91,0x01};

void gt911_set_config(void) {
  uint8_t config_data[sizeof(GT911_Config_t)] = {0};

  gt911_read(GTP_REG_CONFIG_DATA, (uint8_t *)config_data, sizeof(config_data));
  if (config_data[0] == default_config[0]) {
    return;
  }
//   GT911_Config_t *p_config = (GT911_Config_t *)config_data;

//   gt911_read(GTP_REG_CONFIG_DATA, (uint8_t *)config_data, sizeof(config_data));

//   p_config->config_version = 0x50;

//   p_config->shake_count = 0x11;
//   p_config->noise_reduction = 10;
//   p_config->screen_touch_level = 0x60;

//   p_config->check_sum = 0;
//   for (int i = 0; i < sizeof(config_data) - 2; i++) {
//     p_config->check_sum += config_data[i];
//   }
//   p_config->check_sum = (~p_config->check_sum) + 1;
//   p_config->config_refresh = 0x01;

  gt911_write(GTP_REG_CONFIG_DATA, (uint8_t *)default_config, sizeof(default_config));
}

void gt911_init(void) {
  gt911_io_init();
  gt911_reset();
  gt911_set_config();
}
