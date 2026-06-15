#include "boot_ble.h"

#include <stdint.h>

#include "ble.h"
#include "boot_context.h"
#include "bootui.h"
#include "common.h"
#include "log.h"
#include "secbool.h"
#include "usart.h"
#include "options.h"
#include "messages.h"
#include "spi.h"

#define MODULE "boot-ble"

void boot_ble_init(void) {
  BLE_CTL_PIN_INIT();
  ble_function_on();
}

void boot_ble_uart_poll(void) {
  if (ble_usart_can_read() == secfalse) {
    return;
  }
  char code[7] = {0};
  uint8_t buf[64] = {0};
  uint32_t n = ble_usart_read(buf, sizeof(buf));

  char name[17] = {0};
  char version[17] = {0};

  if (!n) {
    return;
  }

  uint8_t id = buf[4];
  uint8_t *resp = &buf[5];
  uint16_t N = buf[2] << 8 | buf[3];
  N -= 2; // remove id and crc
  boot_alive_time_touch();

  switch (id) {
    case BLE_RESP_ADV_NAME:
      memcpy(name, (char *)resp, MIN(sizeof(name), N));
      name[N] = 0;
      ui_event_broadcast(UI_EVENT_BLE_NAME, name);
      // cache name
      strcpy(boot_ctx.ble.name, name);
      LOG_DEBUG(MODULE, "BLE name: %s", name);
      break;
    case BLE_RESP_VERSION:
      memcpy(version, resp, N);
      version[N] = 0;
      ui_event_broadcast(UI_EVENT_BLE_VERSION, version);
      strcpy(boot_ctx.ble.version, version);
      LOG_DEBUG(MODULE, "BLE version: %s", version);
      break;
    case BLE_NOTIFY_STATE:
      if (resp[0] == BLE_NOTIFY_STATE_CONNECTED) {
        ui_event_broadcast(UI_EVENT_BLE_STATE, UI_BLE_STATE_MAKE_PARAM(UI_BLE_STATE_CONNECTED));
        LOG_DEBUG(MODULE, "BLE connected");
      } else if (resp[0] == BLE_NOTIFY_STATE_DISCONNECTED) {
        ui_event_broadcast(UI_EVENT_BLE_STATE, UI_BLE_STATE_MAKE_PARAM(UI_BLE_STATE_DISCONNECTED));
        LOG_DEBUG(MODULE, "BLE disconnected");
      } else {
        LOG_DEBUG(MODULE, "unknown state: %d", resp[0]);
      }
      break;
    case BLE_NOTIFY_PAIR_CODE:
      memcpy(code, resp, 6);
      ui_jump_to(ui_ble_pairing(code));
      LOG_DEBUG(MODULE, "BLE pair code: %s", code);
      break;
    case BLE_NOTIFY_PAIR_RESULT:
      if (resp[0] == BLE_NOTIFY_PAIR_RESULT_SUCCESS) {
        ui_home();
        LOG_DEBUG(MODULE, "BLE pair success");
      } else if (resp[0] == BLE_NOTIFY_PAIR_RESULT_FAILURE) {
        ui_jump_to(ui_ble_pair_failed());
        LOG_DEBUG(MODULE, "BLE pair failure");
      }
      break;
    default:
      LOG_DEBUG(MODULE, "unknown ble response: %d", id);
      break;
  }
}

void boot_ble_poll(void) {

 // poll write
  if (boot_ctx.host.state == HOST_CHANNEL_STATE_PROCESSED && boot_ctx.host.channel == HOST_CHANNEL_BLE) {
    const uint8_t* data = msg_out_data();

    if (!data) {
      boot_ctx.host.state = HOST_CHANNEL_STATE_IDLE;
      return;
    }
    do {
      boot_alive_time_touch();
      int r = spi_slave_send(data, TREZOR_PKG_SIZE, 50);
      ensure(sectrue *(r == TREZOR_PKG_SIZE), NULL);
      data = msg_out_data();
    } while(data);

    boot_ctx.host.state = HOST_CHANNEL_STATE_IDLE;
    boot_ctx.host.channel = HOST_CHANNEL_NONE;
  }


  // poll read
  static CONFIDENTIAL uint8_t buf[TREZOR_PKG_SIZE] __attribute__((aligned(4)));
  int len = spi_slave_poll(buf);
  if (len != TREZOR_PKG_SIZE) {
    return;
  }

  boot_ctx.host.channel = HOST_CHANNEL_BLE;
  boot_alive_time_touch();

  if (!msg_is_tiny()) {
    msg_read(buf, len);
  } else {
    msg_read_tiny(buf, len);
  }
}
