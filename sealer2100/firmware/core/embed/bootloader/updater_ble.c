#include "updater.h"

#include "alignment.h"
#include "ble.h"
#include "bootui.h"
#include "common.h"
#include "fs.h"
#include "image.h"
#include "nordic_dfu.h"
#include "secure_heap.h"
#include "spi.h"

#include "boot_context.h"
#include "fsm.h"
#include "log.h"
#include "usart.h"
#include <stdbool.h>
#include <string.h>

#define MODULE "BLE updater"

#define CHUNK_SIZE 512

enum {
  BLE_UPDATER_DFU_NONE,
  BLE_UPDATER_DFU_WAITING_SEND,
  BLE_UPDATER_DFU_ENTER,
  BLE_UPDATER_DFU_PING,
  BLE_UPDATER_DFU_DONE,
};

typedef struct {
  image_header_t* header;
  uint8_t *firmware_block;
  uint32_t dfu_state;
  uint32_t ping_count;
  uint32_t wait_ble_send_until; // wait until ble send success when upgrade from bluetooth connection
  uint32_t obj_max_size;
  uint32_t crc;
}ble_updater_context_t;

static uint32_t crc32(const uint8_t* data, uint32_t size, uint32_t crc) {
  while (size--) {
    crc = crc ^ *data++;
    for (uint32_t j = 8; j > 0; j--)
    {
        crc = (crc >> 1) ^ (0xEDB88320U & ((crc & 1) ? 0xFFFFFFFF : 0));
    }
  }
  return ~crc;
}

#define failed(code, msg) updater_failed(code, msg)

static void verify_async(void) {
  updater_success(UPDATER_STATE_INSTALL);
}

static void enter_dfu(void) {
  ble_enter_dfu();
  ble_usart_irq_disable();
  hal_delay(50);
}


static void install_init(void) {
  updating_context_t *ctx = boot_ctx.updater.updating;
  ble_updater_context_t *bctx = ctx->user_data;

  if (bctx->dfu_state == BLE_UPDATER_DFU_NONE) {
    if (boot_ctx.host.channel == HOST_CHANNEL_BLE) {
      // send success first, because ble will disconnect when enter dfu
      fsm_sendSuccess("Updating bluetooth");
      bctx->wait_ble_send_until = hal_ticks_ms() + 200;
      bctx->dfu_state = BLE_UPDATER_DFU_WAITING_SEND;
      // yield
      return;
    } else {
      // when upgrade from usb, no need send success first, because usb will not disconnect when enter dfu
      bctx->dfu_state = BLE_UPDATER_DFU_ENTER;
      // yield
      return;
    }
  } else if (bctx->dfu_state == BLE_UPDATER_DFU_WAITING_SEND) {
    if (hal_ticks_ms() < bctx->wait_ble_send_until) {
      // yield
      return;
    }
    bctx->dfu_state = BLE_UPDATER_DFU_ENTER;
    // yield
    return;
  } else if (bctx->dfu_state == BLE_UPDATER_DFU_ENTER) {
    ui_event_broadcast(UI_EVENT_BLE_STATE, UI_BLE_STATE_MAKE_PARAM(UI_BLE_STATE_DISABLED));
    enter_dfu();
    bctx->dfu_state = BLE_UPDATER_DFU_PING;
    bctx->ping_count = 10;
    return;
  } else if (bctx->dfu_state == BLE_UPDATER_DFU_PING) {
    if (bctx->ping_count == 0) {
      LOG_DEBUG(MODULE, "can't enter dfu");
      failed(UPDATER_ERROR_IMAGE_INSTALL_FAILED, "Bluetooth enter DFU failed");
      return;
    }
    if (nrf_dfu_ping(bctx->ping_count--)) {
      LOG_DEBUG(MODULE, "%d ping dfu success", bctx->ping_count);
      bctx->dfu_state = BLE_UPDATER_DFU_DONE;
      return;
    }
    LOG_DEBUG(MODULE, "%d ping dfu failed", bctx->ping_count);
    hal_delay(10);
    return;
  }

  // dfu transfer config
  if (!nrf_dfu_set_prn()) {
    LOG_DEBUG(MODULE, "DFU set prn failed");
    failed(UPDATER_ERROR_IMAGE_INSTALL_FAILED, "Bluetooth DFU setting failed");
    return;
  }
  // not get MTU, use 64 packet size

  // skip header
  fs_file_seek(ctx->fp, IMAGE_HEADER_SIZE, FS_SEEK_SET);
  // get init data
  uint8_t init[CHUNK_SIZE] = {0};
  int r = fs_file_read(ctx->fp, init, sizeof(init));

  if (r != sizeof(init)) {
    LOG_DEBUG(MODULE, "read init data size failed");
    failed(UPDATER_ERROR_IMAGE_INSTALL_FAILED, "Read BLE binary failed");
    return;
  }

  uint32_t init_size = GET_UINT32_LE(init, 0);
  assert(init_size < CHUNK_SIZE - 4);
  if (!nrf_dfu_create_object(NRF_DFU_OBJ_TYPE_COMMAND, init_size)) {
    LOG_DEBUG(MODULE, "DFU create NRF_DFU_OBJ_TYPE_COMMAND failed");
    failed(UPDATER_ERROR_IMAGE_INSTALL_FAILED, "Bluetooth install failed");
    return;
  }

  uint32_t crc1 = crc32(init+4, init_size, 0xFFFFFFFF);
  nrf_dfu_write_object(init+4, init_size);
  uint32_t crc2 = 0;
  if (!nrf_dfu_get_crc(&crc2)) {
    LOG_DEBUG(MODULE, "DFU get object crc failed");
    failed(UPDATER_ERROR_IMAGE_INSTALL_FAILED, "Bluetooth install failed");
    return;
  }
  if (crc1 != crc2) {
    LOG_DEBUG(MODULE, "crc not match: crc1: %08x, crc2: %08x", crc1, crc2);
    failed(UPDATER_ERROR_IMAGE_INSTALL_FAILED, "Bluetooth install failed");
    return;
  }

  if (!nrf_dfu_execute_object()) {
    LOG_DEBUG(MODULE, "DFU execute object failed");
    failed(UPDATER_ERROR_IMAGE_INSTALL_FAILED, "Bluetooth install failed");
    return;
  }

  if (!nrf_dfu_select_object(NRF_DFU_OBJ_TYPE_DATA, &bctx->obj_max_size)) {
    LOG_DEBUG(MODULE, "DFU select object failed");
    failed(UPDATER_ERROR_IMAGE_INSTALL_FAILED, "Bluetooth install failed");
    return;
  }
  bctx->firmware_block = pvPortMalloc(bctx->obj_max_size);
  bctx->crc = 0xFFFFFFFF;
}

static void install_async(void) {
  updating_context_t *ctx = boot_ctx.updater.updating;
  ble_updater_context_t *bctx = ctx->user_data;

  if (fs_file_tell(ctx->fp) == 0) {
    // reset ble name and version
    memset(boot_ctx.ble.name, 0, sizeof(boot_ctx.ble.name));
    memset(boot_ctx.ble.version, 0, sizeof(boot_ctx.ble.version));
    install_init();
    return;
  }

  // install firmware
  uint8_t* block = bctx->firmware_block;
  uint32_t block_size = bctx->obj_max_size;
  int r = fs_file_read(ctx->fp, block, block_size);
  if (r < 0) {
    LOG_DEBUG(MODULE, "read firmware data size failed");
    failed(UPDATER_ERROR_IMAGE_INSTALL_FAILED, "Read BLE binary failed");
    return;
  }

  if (!nrf_dfu_create_object(NRF_DFU_OBJ_TYPE_DATA, r)) {
    LOG_DEBUG(MODULE, "DFU create NRF_DFU_OBJ_TYPE_DATA failed");
    failed(UPDATER_ERROR_IMAGE_INSTALL_FAILED, "Bluetooth install failed");
    return;
  }
  uint32_t crc1 = crc32(block, r, bctx->crc);
  nrf_dfu_write_object(block, r);
  uint32_t crc2;
  if (!nrf_dfu_get_crc(&crc2)) {
    LOG_DEBUG(MODULE, "DFU get object crc failed");
    failed(UPDATER_ERROR_IMAGE_INSTALL_FAILED, "Bluetooth install failed");
    return;
  }
  if (crc1 != crc2) {
    LOG_DEBUG(MODULE, "crc not match: crc1: %08x, crc2: %08x", crc1, crc2);
    failed(UPDATER_ERROR_IMAGE_INSTALL_FAILED, "Bluetooth install failed");
    return;
  }
  // the crc is the from firmware begin
  bctx->crc = ~crc2;
  if (!nrf_dfu_execute_object()) {
    LOG_DEBUG(MODULE, "DFU execute object failed");
    failed(UPDATER_ERROR_IMAGE_INSTALL_FAILED, "Bluetooth install failed");
    return;
  }

  if (fs_file_tell(ctx->fp) != fs_file_size(ctx->fp)) {
    return;
  }
  LOG_DEBUG(MODULE, "Bluetooth update success");
  if (boot_ctx.host.channel == HOST_CHANNEL_USB) {
    fsm_sendSuccess("Bluetooth update success");
  }
  updater_success(UPDATER_STATE_FINISHED);
}

static void __abort(void) {
  updating_context_t *ctx = boot_ctx.updater.updating;
  ble_updater_context_t *bctx = ctx->user_data;

  // need set high to avoid reboot to dfu
  SET_COMBUS_HIGH();
  hal_delay(1);
  // restart ble
  nrf_dfu_restart();
  ble_usart_irq_enable();
  // need enable spi slave after dfu restart
  spi_slave_init();
  hal_delay(100);
  if (!bctx) {
    return;
  }

  if (bctx->header) {
    vPortFree(bctx->header);
    bctx->header = NULL;
  }
  if (bctx->firmware_block) {
    vPortFree(bctx->firmware_block);
    bctx->firmware_block = NULL;
  }

  vPortFree(bctx);
  ctx->user_data = NULL;

  // refresh ble info when success or failed
  // reset ble info
  memset(&boot_ctx.ble, 0x00, sizeof(boot_ctx.ble));
  // refresh ble info after abort
  ui_event_broadcast(UI_EVENT_BLE_STATE, UI_BLE_STATE_MAKE_PARAM(UI_BLE_STATE_ENABLED));
  ui_event_broadcast(UI_EVENT_BLE_INFO_REFRESH, NULL);
}

static updater_t updater = {
    .get_name_fn = updater_get_name,
    .get_cur_ver_fn = updater_get_cur_ver,
    .get_new_ver_fn = updater_get_new_ver,
    .get_error_fn = updater_get_error,
    .get_processed_fn = updater_get_processed,
    .verify_image_fn = verify_async,
    .install_image_fn = install_async,
    .abort_fn = __abort,
    .cancelled_fn = updater_canceled,
};
void update_ble(void) {
  updating_context_t *ctx = boot_ctx.updater.updating;

  // step 0. setup current version
  // if (strlen(boot_ctx.ble.version) == 0) {
  //   LOG_DEBUG(MODULE, "BLE info not synced");
  //   failed(UPDATER_ERROR_READ_FAILED, "can't get ble version");
  //   return;
  // }

  strcpy(ctx->cur_ver, boot_ctx.ble.version);

  // step 1. load ble header
  uint8_t header[IMAGE_HEADER_SIZE] = {0};
  int r = 0;

  r = fs_file_read(ctx->fp, header, sizeof(header));
  if (r < sizeof(header)) {
    LOG_DEBUG(MODULE, "read BLE header filed");
    failed(UPDATER_ERROR_READ_FAILED, "Read BLE binary failed");
    return;
  }
  fs_file_seek(ctx->fp, 0, FS_SEEK_SET);

  ble_updater_context_t* bctx = pvPortMalloc(sizeof(ble_updater_context_t));
  memset(bctx, 0, sizeof(ble_updater_context_t));
  bctx->header = pvPortMalloc(sizeof(image_header_t));
  memset(bctx->header, 0, sizeof(image_header_t));
  ctx->user_data = bctx;


  if (sectrue != load_ble_image_header(header, BLE_IMAGE_MAGIC, bctx->header)) {
    LOG_DEBUG(MODULE, "load BLE header failed");
    failed(UPDATER_ERROR_IMAGE_VERIFY_FAILED, "Invalid BLE binary file");
    // have not register updater, free manually
    vPortFree(bctx->header);
    vPortFree(bctx);
    ctx->user_data = NULL;
    return;
  }

  // setp 2. setup new version
  image_version_format(bctx->header->version, ctx->new_ver);


  // step 3. register updater
  updater_register(&updater);
}
