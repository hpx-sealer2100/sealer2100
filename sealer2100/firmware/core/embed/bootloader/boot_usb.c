#include "boot_usb.h"

#include <stdint.h>

#include "log.h"
#include "options.h"
#include "boot_context.h"
#include "bootui.h"
#include "common.h"
#include "messages.h"
#include "secbool.h"
#include "usb.h"

#define MODULE "boot-usb"

#define USB_IFACE_NUM   0
#define USB_PACKET_SIZE 64

void boot_usb_init(bool usb21_landing)
{
    usb_dev_info_t dev_info = {
        .device_class = 0x00,
        .device_subclass = 0x00,
        .device_protocol = 0x00,
        .vendor_id = 0x1209,
        .product_id = 0x6869,
        .release_num = 0x0200,
        .manufacturer = "HyperPay",
        .product = "Sealer2001 Boot",
        .serial_number = "000000000000000000000000",
        .interface = "Sealer2001 Interface",
        .usb21_enabled = true,
        .usb21_landing = usb21_landing,
    };

    static uint8_t rx_buffer[USB_PACKET_SIZE];

    static const usb_webusb_info_t webusb_info = {
        .iface_num = USB_IFACE_NUM,
        .ep_in = USB_EP_DIR_IN | 0x01,
        .ep_out = USB_EP_DIR_OUT | 0x01,
        .subclass = 0,
        .protocol = 0,
        .max_packet_len = sizeof(rx_buffer),
        .rx_buffer = rx_buffer,
        .polling_interval = 1,
    };

    usb_init(&dev_info);

    ensure(usb_webusb_add(&webusb_info), NULL);

    usb_start();
}

void boot_usb_poll(void) {
  // poll write
  if (boot_ctx.host.state == HOST_CHANNEL_STATE_PROCESSED && boot_ctx.host.channel == HOST_CHANNEL_USB) {
    const uint8_t* data = msg_out_data();

    if (!data) {
      boot_ctx.host.state = HOST_CHANNEL_STATE_IDLE;
      return;
    }
    do {
      boot_alive_time_touch();
      int r = usb_webusb_write_blocking(USB_IFACE_NUM, data, USB_PACKET_SIZE, 5);
      ensure(sectrue *(r == USB_PACKET_SIZE), NULL);
      data = msg_out_data();
    } while(data);

    boot_ctx.host.state = HOST_CHANNEL_STATE_IDLE;
    boot_ctx.host.channel = HOST_CHANNEL_NONE;
  }


  // poll read
  static CONFIDENTIAL uint8_t buf[USB_PACKET_SIZE] __attribute__((aligned(4)));
  int len = usb_webusb_read_blocking(USB_IFACE_NUM, buf, USB_PACKET_SIZE, 1);
  if (len != USB_PACKET_SIZE) {
    return;
  }

  boot_ctx.host.channel = HOST_CHANNEL_USB;
  boot_alive_time_touch();

  if (!msg_is_tiny()) {
    msg_read(buf, len);
  } else {
    msg_read_tiny(buf, len);
  }
}

void boot_usb_state_poll(void) {
  bool connectd = is_usb_connected() == sectrue;
  if (boot_ctx.device.usb_connected == connectd) {
    return;
  }
  // changed
  if (connectd) {
    boot_alive_time_touch();
    ui_event_broadcast(UI_EVENT_USB_STATE, UI_USB_STATE_MAKE_PARAM(1));
    LOG_DEBUG(MODULE, "USB connected");
  } else {
    boot_alive_time_touch();
    ui_event_broadcast(UI_EVENT_USB_STATE, UI_USB_STATE_MAKE_PARAM(0));
    LOG_DEBUG(MODULE, "USB disconnected");
  }

  // cache
  boot_ctx.device.usb_connected = !!connectd;
}

void boot_usb_msc_init(void) {
    usb_msc_init(MSC_BLOCK_EMMC);
}
