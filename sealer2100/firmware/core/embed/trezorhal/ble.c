#include "ble.h"
#include "common.h"
#include "spi.h"
#include "usart.h"
#include <stdbool.h>


static uint8_t xor(uint8_t *buf, uint32_t len) {
  uint8_t crc = 0;
  while (len--) {
    crc ^= *buf++;
  }
  return crc;
}

static void ble_cmd_packet(uint8_t *value, uint8_t value_len) {
  uint8_t cmd[64] = {0};
  cmd[0] = 0x5a;
  cmd[1] = 0xa5;
  cmd[2] = ((value_len + 1) >> 8) & 0xff;
  cmd[3] = (value_len + 1) & 0xff;
  memcpy(cmd + 4, value, value_len);
  cmd[value_len + 4] = xor(cmd, value_len + 4);
  ble_usart_send(cmd, value_len + 5);
}

void ble_cmd_req(uint8_t cmd, uint8_t value) {
  uint8_t buf[64] = {0};
  buf[0] = cmd;
  buf[1] = value;
  ble_cmd_packet(buf, 2);
}

void ble_reboot(void) {
  ble_cmd_req(BLE_CMD_REBOOT, BLE_CMD_REBOOT_PARAM);
}

void ble_enter_dfu(void) {
  spi_slave_deinit();
  hal_delay(1);
  SET_COMBUS_LOW();
  hal_delay(1);
  ble_reboot();
}

void ble_async_get_name(void) {
  ble_cmd_req(BLE_CMD_INFO, BLE_CMD_INFO_PARAM_ADV_NAME);
}

void ble_async_get_version(void) {
  ble_cmd_req(BLE_CMD_INFO, BLE_CMD_INFO_PARAM_VERSION);
}

void ble_async_disconnect(void) {
  ble_cmd_req(BLE_CMD_CTRL, BLE_CMD_CTRL_PARAM_DISCONNECT);
}



void ble_async_refresh_dev_info(void) {
  ble_async_get_name();
  ble_async_get_version();
}

void ble_function_on() {
  BLE_POWER_ON();
  spi_slave_init();
  ble_usart_init();
}

void ble_function_off() {
  BLE_POWER_OFF();
  spi_slave_deinit();
  ble_usart_deinit();
}
