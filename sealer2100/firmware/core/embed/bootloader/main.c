#include "boot_ble.h"
#include "boot_power_key.h"
#include "boot_updater.h"
#include "boot_usb.h"
#include "fs.h"
#include "fsport/lfs_port.h"
#include "fsport/fatfs_diskio.h"
#include "iris.h"
#include "stm32h7xx_hal.h"
#include "common.h"
#include "flash.h"
#include "image.h"
#include "lv_port.h"
#include "mipi_lcd.h"
#include "mpu.h"
#include "sdram.h"
#include "thd89/se.h"
#include "spi.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "i2c.h"
#include "ble.h"
#include "display.h"
#include "rng.h"
#include "touch.h"

#include "memzero.h"
#include "battery.h"
#include "lowlevel.h"
#include "qspi_flash.h"
#include "random_delays.h"
#include "secbool.h"
#include "stm32h747xx.h"
#include "uart_log.h"
#include "device.h"
#include "power_manager.h"
#include "lvgl.h"
#include "keys.h"
#include "timer.h"
#include "emmc.h"
#include "nand_flash.h"
#include "rand.h"
#include "sram.h"

#include "log.h"
#include "boot_context.h"

#define MODULE "bootloader"

#define NAND_TEST 0

#define RESOURCE_FLAG_FILE "/res/.HYPERMATE_RESOURCE"

static void bus_fault_enable()
{
    SCB->SHCSR |= SCB_SHCSR_BUSFAULTENA_Msk;
}
static void bus_fault_disable()
{
    SCB->SHCSR &= ~SCB_SHCSR_BUSFAULTENA_Msk;
}

static inline void set_pcb_version(void) {
  pcb_version_t v;
#ifdef PCB_VERSION_1_0
#error "PCB v1.0 board is no longer support"
  v = PCB_V1_0;
#else
  v = PCB_V1_1;
#endif
  device_set_pcb_version(v);
}

static inline void lvgl_last_active_time_monitor(void) {
  lv_disp_t* disp = lv_disp_get_default();
  uint32_t active_time = disp->last_activity_time;
  if (active_time != boot_ctx.monitor.lvgl_last_active_time) {
    boot_ctx.monitor.lvgl_last_active_time = active_time;
    boot_alive_time_touch();
  }
}

#define BOARD_VERSION "HyperMate Boardloader 1.0.0"
void boardloader(bool has_resource) {
  LOG_DEBUG("boardloader", "hello boardloader");
  battery_init();
  pm_init();
  i2c4_init();
  display_backlight(0);
  timer_init();
  touch_init();
  lcd_init(DISPLAY_RESX, DISPLAY_RESY, LCD_PIXEL_FORMAT_RGB565);
  lcd_pwm_init();
  display_backlight(80);
  if (has_resource) {
    LOG_DEBUG(MODULE, "lvgl init...");
    lv_init();
    lv_port_disp_init();
    lv_port_indev_init();
    lv_port_fs_init(&boot_ctx.fs);
    ui_boardloader(BOARD_VERSION);
  }
  boot_usb_msc_init();
  while(1) {
    if (has_resource) {
      lv_timer_handler();
    }
    __WFI();
    __WFE();
  }
}

void bootloader(void) {
  LOG_DEBUG(MODULE, "hello bootloader");

  battery_init();
  pm_init();
  i2c4_init();
  display_backlight(0);
  timer_init();
  touch_init();
  lcd_init(DISPLAY_RESX, DISPLAY_RESY, LCD_PIXEL_FORMAT_RGB565);
  lcd_pwm_init();
  display_backlight(80);

  boot_usb_init(boot_ctx.firmware.valided);
  boot_power_key_init();
  LOG_DEBUG(MODULE, "lvgl init...");
  lv_init();
  lv_port_disp_init();
  lv_port_indev_init();
  lv_port_fs_init(&boot_ctx.fs);
  ui_event_register();
  ui_init();
  ui_status_bar_init();
  ui_home();
  boot_ble_init();
  LOG_DEBUG(MODULE, "bootloader init done");
  while(1) {
      boot_ble_poll();
      boot_usb_poll();
      boot_ble_uart_poll();
      boot_usb_state_poll();
      boot_updater_poll();
      boot_power_key_poll();
      boot_fs_msg_monitor_poll();
      lv_timer_handler();
      lvgl_last_active_time_monitor();
      __WFI();
      __WFE();
  };
}

static void fs_test(void) {
  fs_test_list_all_files(&boot_ctx.fs);
  // fs_speed_test(&boot_ctx.fs);
}

void fs_setup(fs_t* fs) {
  fs_init(fs);

  // user can test pcb version and set `lfs` or `fatfs` port

  // use fatfs and emmc
  const fatfs_diskio_t* config = fatfs_port_emmc_diskio();
  fatfs_port_t* port = fatfs_port_init(config);
  fs_fatfs_register(fs, port);

  // use lfs and nand, for test
  // const struct lfs_config* config = lfs_port_nand_config();
  // lfs_port_t* port = lfs_port_init(config);
  // fs_lfs_register(fs, port);


  // use lfs and emmc, for test
  // const struct lfs_config* config = lfs_port_emmc_config();
  // lfs_port_t* port = lfs_port_init(config);
  // fs_lfs_register(fs, port);
}


static void iris_try_set_shared_secret(void) {
#define IRIS_BIND_AGAIN 0 // for test

#if IRIS_BIND_AGAIN
  // wait for 2s, for test
  LOG_DEBUG(MODULE, "try bind iris shared secret again for test ...");
  hal_delay(2000);
#endif
  // step 0. check is device setup shared secret
  int ret = 0;

  if (!device_is_pre_shared_key_set()) {
    // speed up
    return;
  }

  if (device_is_iris_pre_shared_key_set()) {
    LOG_DEBUG(MODULE, "MCU have set pre shared key, iris have bind shared secret");
#if !IRIS_BIND_AGAIN
    return;
#endif
  }

  // step 1. open iris
  iris_open();
  uint8_t resp_buf[256] = {0};

  // setp 2. wait for iris open done
  iris_poll_context_t ctx = {0};
  // nand version iris, first powerup, need long timeout
  ctx = iris_poll_wait(5000, resp_buf, sizeof(resp_buf));
  if (ctx.state != IRIS_POLL_STATE_DONE) {
    // maybe not connect when production
    LOG_DEBUG(MODULE, "iris open failed");
    iris_wait_power_down(false);
    return;
  }

  // step 3. check is iris have setup shared secret
  // power up note
  if (!iris_response_is_note(&ctx.resp) || ctx.resp.id != IRIS_MSG_ID_NOTE_MODULE_STATUS) {
    LOG_ERROR(MODULE, "invalid power up note");
    iris_wait_power_down(false);
    return;
  }
#if !IRIS_BIND_AGAIN
  iris_note_t note = ctx.resp.note;
  if (note.status == IRIS_MODULE_STATUS_BOUND) {
    LOG_DEBUG(MODULE, "iris have setup shared secret");
    // just power down, no need handshake, and send power down note
    iris_wait_power_down(true);
    return;
  }

  // step 4. check version
  iris_async_get_version();
  memset(&ctx, 0, sizeof(ctx));
  ctx = iris_poll_wait(1000, resp_buf, sizeof(resp_buf));
  if (ctx.state != IRIS_POLL_STATE_DONE) {
    LOG_ERROR(MODULE, "iris get version failed");
    iris_wait_power_down(true);
    return;
  }
  if (!iris_response_replay_is(&ctx.resp, IRIS_MSG_ID_DEVICE_GET_VERSION)) {
    LOG_ERROR(MODULE, "iris get version failed");
    iris_wait_power_down(true);
    return;
  }
  LOG_HEXDUMP_DEBUG(MODULE, "version", ctx.resp.reply.data, ctx.resp.reply.size);
  iris_version_t ver = {0};
  ret = iris_version_parse((char*)ctx.resp.reply.data, &ver);
  if (ret != IRIS_ERROR_OK) {
    LOG_DEBUG(MODULE, "iris version parser failed, ret: %d", ret);
    iris_wait_power_down(true);
    return;
  }
  if (!IRIS_IS_SUPPORT_SEC_CHANNEL(ver)) {
    LOG_DEBUG(MODULE, "iris version not support sec channel");
    iris_wait_power_down(true);
    return;
  }
#endif
  // step 5. set shared secret
  uint8_t key[32] = {0};
  random_buffer(key, sizeof(key));
#if IRIS_BIND_AGAIN
  if (device_is_iris_pre_shared_key_set()) {
    LOG_DEBUG(MODULE, "iris have bind shared secret, reuse it");
    device_get_iris_pre_shared_key(key);
  }
#endif
  ret = iris_set_shared_secret(key, sizeof(key));
  if (ret != IRIS_ERROR_OK) {
    memzero(key, sizeof(key));
    LOG_ERROR(MODULE, "iris set shared secret failed, ret: %d", ret);
  } else {
    device_set_iris_pre_shared_key(key);
    memzero(key, sizeof(key));
  }

  iris_wait_power_down(true);
}

// provide fs_t* __fs__ for lvgl font
fs_t* __fs__ = &boot_ctx.fs;
int main(void)
{
  bool serial_set = false, cert_set = false, res_set = false, vaild_firmware = false;

  mpu_config_bootloader();
  memset((void*)SRAM_BASE_ADDRESS, 0, SRAM_SIZE);
  reset_flags_reset();
  periph_init();
  /* Enable the CPU Cache */
  cpu_cache_enable();
  system_clock_config();
  rng_init();
  __stack_chk_guard = rng_get();

  // use log
  uart_log_init();

  random_delays_init();
  wait_random();

  flash_option_bytes_init();
  clear_otg_hs_memory();
  flash_otp_init();
  set_pcb_version();
  device_para_init();
  bus_fault_enable();
  sdram_init();
  emmc_init();
  se_init();
  {
    uint8_t secret[32] = {0};
    if (device_get_pre_shared_key(secret)) {
      se_handshake(secret, sizeof(secret));
    }
    memzero(secret, sizeof(secret));
  }

#if NAND_TEST
  nand_flash_init();
#else
  qspi_flash_init();
  qspi_flash_config();
  qspi_flash_memory_mapped();
#endif

  boot_context_init();

  fs_setup(&boot_ctx.fs);

  iris_try_set_shared_secret();

  (void)fs_test;
  // fs_test();

  // extern int iris_sec_channel_handshake_test(void);
  // iris_sec_channel_handshake_test();

  res_set = fs_is_exists(&boot_ctx.fs, RESOURCE_FLAG_FILE);
  if (!res_set) {
    LOG_DEBUG(MODULE, "resource not set, will enter boardloader mode");
    boardloader(res_set);
    while(1);
  }

  serial_set = device_serial_set();
  size_t cert_len = 0;
  cert_set = se_get_certificate_len(&cert_len) == 0 && cert_len > 0;

  // check if firmware valid
  if ( !NAND_TEST && sectrue == image_verify_firmware(FIRMWARE_START) ) {
      vaild_firmware = true;
      boot_ctx.firmware.valided = true;
  }


#if !PRODUCTION
  if (!serial_set) {
    LOG_DEBUG(MODULE, "serial not set, set it for devloping mode");
    serial_set = true;
  }

  if (!cert_set) {
    LOG_DEBUG(MODULE, "cert not set, set it for devloping mode");
    cert_set = true;
  }
  if (!vaild_firmware) {
    LOG_DEBUG(MODULE, "firmware not valid, will enter bootloader mode");
  }
#endif

    run_mode_t mode = device_get_run_mode();
    // mode = RUN_MODE_BOARDLOADER; // for easy development
    // mode = RUN_MODE_BOOTLOADER; // for easy development

    // use instruction enter boardloader mode
    if (mode == RUN_MODE_BOARDLOADER) {
        boardloader(res_set);
        while(1);
    }
    // use instruction enter bootloader mode
    if (mode == RUN_MODE_BOOTLOADER) {
        bootloader();
        while(1);
    }
    if (!vaild_firmware) {
        bootloader();
        while(1);
    }

#if PRODUCTION
    if (!serial_set || !cert_set || !res_set || !vaild_firmware) {
        LOG_DEBUG(MODULE, "device not manuafactured, will enter bootloader mode");
        bootloader();
        while(1);
    }
#endif

    uart_log_flush();
    uart_log_deinit();

    bus_fault_disable();

    const image_firmware_t* const fw = FIRMWARE;
    jump_to((uint32_t)fw->code);
    return 0;
}
