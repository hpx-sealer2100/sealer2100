#include "boot_power_key.h"
#include "boot_updater.h"
#include "bootui.h"
#include "fs.h"
#include "fsport/lfs_port.h"
#include "fsport/fatfs_diskio.h"
#include "stm32h7xx_hal.h"
#include "common.h"
#include "flash.h"
#include "image.h"
#include "lv_port.h"
#include "mipi_lcd.h"
#include "mpu.h"
#include "sdram.h"
#include "thd89/se.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "i2c.h"
#include "display.h"
#include "rng.h"
#include "touch.h"

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
#include "memzero.h"

#include "log.h"
#include "boot_context.h"
#include "updater.h"

#define MODULE "upgrader"

#define UPGRADER_BIN_BOOT "/upgrader/bootloader.bin"
#define UPGRADER_BIN_FIRMWARE "/upgrader/firmware.bin"
#define UPGRADER_BIN_SE  "/upgrader/se.bin"

typedef void (*binary_verify_cb_t)(int pct);

// provide fs_t* __fs__ for lvgl font
fs_t* __fs__ = &boot_ctx.fs;

static void bus_fault_enable()
{
    SCB->SHCSR |= SCB_SHCSR_BUSFAULTENA_Msk;
}
static inline void lvgl_last_active_time_monitor(void) {
  lv_disp_t* disp = lv_disp_get_default();
  uint32_t active_time = disp->last_activity_time;
  if (active_time != boot_ctx.monitor.lvgl_last_active_time) {
    boot_ctx.monitor.lvgl_last_active_time = active_time;
    boot_alive_time_touch();
  }
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

bool verify_bootloader(void) {
  const char* binary = UPGRADER_BIN_BOOT;
  ui_jump_to(ui_operation_init(fs_base_name(binary), "Verifying ..."));
  binary_verify_cb_t cb = ui_operation_update;

  // open file
  fs_file_t f = {0};
  int ret = fs_file_open(__fs__, &f, binary, FS_O_RDONLY);
  if (ret < 0) {
    LOG_DEBUG(MODULE, "open file %s failed", binary);
    return false;
  }

  // get file size
  fs_info_t stat = {0};
  fs_stat(__fs__, binary, &stat);
  uint32_t total = stat.size;
  uint32_t processed = 0;

  // read header
  uint8_t header[sizeof(image_bootloader_t)] = {0};
  ret = fs_file_read(&f, header, sizeof(image_bootloader_t));
  if (ret != sizeof(image_bootloader_t)) {
    LOG_DEBUG(MODULE, "read header failed");
    return false;
  }
  processed += sizeof(image_bootloader_t);
  if (cb) cb(processed*100.0/total);

  // only cache header
  image_bootloader_t* const bl = (image_bootloader_t*)header;

  SHA256_CTX ctx;
  sha256_Init(&ctx);

  // vector table protected by signature
  sha256_Update(&ctx, bl->vector_table, sizeof(bl->vector_table));

  // `header` partly protected by signature
  image_header_hash_update(&ctx, &bl->header);

  size_t chunk_size = 16*1024;
  uint8_t data[16*1024] = {0};
  // copy file to memory
  while ((ret = fs_file_read(&f, data, chunk_size)) > 0) {
    processed += ret;
    sha256_Update(&ctx, data, ret);
    int pct = (int)(processed * 100.0 / total);
    if (cb) cb(pct);
  }
  fs_file_close(&f);

  // compute digest
  uint8_t digest[32];
  sha256_Final(&ctx, digest);
  LOG_HEXDUMP_DEBUG(MODULE, "bootloader computed digest", digest, sizeof(digest));
  LOG_HEXDUMP_DEBUG(MODULE, "bootloader stored digest", bl->header.digest, sizeof(bl->header.digest));

  // verify
  // step 0: check magic
  if (bl->header.magic != BOOTLOADER_IMAGE_MAGIC) {
    LOG_DEBUG(MODULE, "bootloader magic %04x is not BOOTLOADER_IMAGE_MAGIC", bl->header.magic);
    return false;
  }

  // step 1: check digest
  if (memcmp(digest, bl->header.digest, sizeof(digest)) != 0) {
    LOG_DEBUG(MODULE, "bootloader digest check failed");
    return false;
  }

  // step 2: verify signature
  if (image_header_verify(&bl->header) != sectrue) {
    LOG_DEBUG(MODULE, "bootloader signature verify failed");
    return false;
  }
  LOG_DEBUG(MODULE, "bootloader signature verify success");
  if (cb) cb(100);
  return true;
}

bool verify_firmware(void) {
  const char* binary = UPGRADER_BIN_FIRMWARE;
  ui_jump_to(ui_operation_init(fs_base_name(binary), "Verifying ..."));
  binary_verify_cb_t cb = ui_operation_update;

  // open file
  fs_file_t f = {0};
  int ret = fs_file_open(__fs__, &f, binary, FS_O_RDONLY);
  if (ret < 0) {
    LOG_DEBUG(MODULE, "open file %s failed", binary);
    return false;
  }

  // get file size
  fs_info_t stat = {0};
  fs_stat(__fs__, binary, &stat);
  uint32_t total = stat.size;
  uint32_t processed = 0;

  // read header
  uint8_t header[sizeof(image_firmware_t)] = {0};
  ret = fs_file_read(&f, header, sizeof(image_firmware_t));
  if (ret != sizeof(image_firmware_t)) {
    LOG_DEBUG(MODULE, "read header failed");
    return false;
  }
  processed += sizeof(image_firmware_t);

  // only cache header
  image_firmware_t* const fw = (image_firmware_t*)header;

  SHA256_CTX ctx;
  sha256_Init(&ctx);

  // `header` partly protected by signature
  image_header_hash_update(&ctx, &fw->header);
  if (cb) cb(processed*100.0/total);

  size_t chunk_size = 16*1024;
  uint8_t data[16*1024] = {0};
  // copy file to memory
  while ((ret = fs_file_read(&f, data, chunk_size)) > 0) {
    processed += ret;
    sha256_Update(&ctx, data, ret);
    int pct = (int)(processed * 100.0 / total);
    if (cb) cb(pct);
  }
  fs_file_close(&f);

  // compute digest
  uint8_t digest[32];
  sha256_Final(&ctx, digest);
  LOG_HEXDUMP_DEBUG(MODULE, "firmware computed digest", digest, sizeof(digest));
  LOG_HEXDUMP_DEBUG(MODULE, "firmware stored digest", fw->header.digest, sizeof(fw->header.digest));

  // verify
  // step 0: check magic
  if (fw->header.magic != FIRMWARE_IMAGE_MAGIC) {
    LOG_DEBUG(MODULE, "firmware magic %04x is not FIRMWARE_IMAGE_MAGIC", fw->header.magic);
    return false;
  }

  // step 1: check digest
  if (memcmp(digest, fw->header.digest, sizeof(digest)) != 0) {
    LOG_DEBUG(MODULE, "firmware digest check failed");
    return false;
  }

  // step 2: verify signature
  if (image_header_verify(&fw->header) != sectrue) {
    LOG_DEBUG(MODULE, "firmware signature verify failed");
    return false;
  }
  LOG_DEBUG(MODULE, "firmware signature verify success");
  if (cb) cb(100);
  return true;
}

bool verify_se(void) {
#define SE_HEADER_SIZE 512
#define SE_CHUNK_SIZE 512
  const char* binary = UPGRADER_BIN_SE;
  ui_jump_to(ui_operation_init(fs_base_name(binary), "Verifying ..."));
  binary_verify_cb_t cb = ui_operation_update;

  // open file
  fs_file_t f = {0};
  int ret = fs_file_open(__fs__, &f, binary, FS_O_RDONLY);
  if (ret < 0) {
    LOG_DEBUG(MODULE, "open file %s failed", binary);
    return false;
  }

  // get file size
  fs_info_t stat = {0};
  fs_stat(__fs__, binary, &stat);
  uint32_t total = stat.size;
  uint32_t processed = 0;

  typedef struct {
    uint8_t magic[4];
    uint32_t version;
    // the `header_t`
    uint32_t header_size;
    // code size
    uint32_t code_size;
    uint8_t digest[32];
    uint8_t signature[64];
  }__attribute__((packed, aligned(SE_HEADER_SIZE))) se_header_t ;

  se_header_t header = {0};
  // read header
  ret = fs_file_read(&f, &header, sizeof(header));
  if (ret != sizeof(header)) {
    LOG_DEBUG(MODULE, "read file failed");
    return false;
  }

  if (header.header_size + header.code_size != total) {
    LOG_DEBUG(MODULE, "SE header size %d + code size %d != total size %d", header.header_size, header.code_size, total);
    return false;
  }
  processed += sizeof(header);
  if (cb) cb(processed*100/total);
  SHA256_CTX ctx;
  sha256_Init(&ctx);

  uint8_t chunk[SE_CHUNK_SIZE] = {0};
  while((ret = fs_file_read(&f, chunk, SE_CHUNK_SIZE)) > 0) {
    processed += ret;
    sha256_Update(&ctx, chunk, ret);
    int pct = (int)(processed * 100 / total);
    if (cb) cb(pct);
  }
  fs_file_close(&f);

  uint8_t digest[32];
  sha256_Final(&ctx, digest);
  LOG_HEXDUMP_DEBUG(MODULE, "SE computed digest", digest, sizeof(digest));
  LOG_HEXDUMP_DEBUG(MODULE, "SE stored digest", header.digest, sizeof(header.digest));
  if (memcmp(digest, header.digest, sizeof(digest)) != 0) {
    LOG_DEBUG(MODULE, "SE digest check failed");
    return false;
  }

  return true;
}

void try_move_storage(void) {
  // step 0: verify storage magic
  #define __FLASH_STORAGE_OFFSET__ ((40-16) * FLASH_FIRMWARE_SECTOR_SIZE)
  #define __STORAGE_MAGIC_0__ ((uint32_t)0x3243524e)
  #define __STORAGE_MAGIC_1__ ((uint32_t)0x5743524e)

  // qspi api need inner address, not mapped address
  #define __QSPI_ADDRESS__(addr) ((uint32_t)(addr) - QSPI_FLASH_BASE_ADDRESS)

  const uint8_t* p1 = (void*)(QSPI_FLASH_BASE_ADDRESS + __FLASH_STORAGE_OFFSET__);
  const uint8_t* p2 = (void*)(QSPI_FLASH_BASE_ADDRESS + __FLASH_STORAGE_OFFSET__ + FLASH_STORAGE_SECTOR_SIZE);
  bool verified = false;
  uint32_t magic = *(uint32_t*)p1;
  if (magic == __STORAGE_MAGIC_0__ || magic == __STORAGE_MAGIC_1__) {
    LOG_DEBUG(MODULE, "storage magic %04x found at %p", magic, p1);
    verified = true;
  }
  magic = *(uint32_t*)p2;
  if (magic == __STORAGE_MAGIC_0__ || magic == __STORAGE_MAGIC_1__) {
    LOG_DEBUG(MODULE, "storage magic %04x found at %p", magic, p2);
    verified = true;
  }

  if (!verified) {
    LOG_DEBUG(MODULE, "Not found storage, skip move storage");
    return;
  }
  LOG_DEBUG(MODULE, "has found storage, move to new address");
  ui_jump_to(ui_operation_init("Storage", "Moving ..."));

  // step 1: erase storage
  const uint8_t* d1 = flash_get_address(FLASH_SECTOR_STORAGE_1, 0, FLASH_STORAGE_SECTOR_SIZE);
  const uint8_t* d2 = flash_get_address(FLASH_SECTOR_STORAGE_2, 0, FLASH_STORAGE_SECTOR_SIZE);
  LOG_DEBUG(MODULE, "earse storage new address: %p and %p", d1, d2);
  qspi_flash_erase_block_64k(__QSPI_ADDRESS__(d1));
  ui_operation_update(16);
  qspi_flash_erase_block_64k(__QSPI_ADDRESS__(d2));
  ui_operation_update(33);
  LOG_DEBUG(MODULE, "storage erase done");

  // step 2: copy storage to new sector
  uint32_t processed = 0;
  uint32_t total = FLASH_STORAGE_SECTOR_SIZE*2;

  uint8_t chunk[1024] = {0};
  uint32_t size = FLASH_STORAGE_SECTOR_SIZE;

  const uint8_t *p = p1;
  const uint8_t *d = d1;
  LOG_DEBUG(MODULE, "copy storage from %p to %p ...", p1, d1);
  while (size > 0) {
    qspi_flash_read_buffer(chunk, __QSPI_ADDRESS__(p), sizeof(chunk));
    qspi_flash_write_buffer_unsafe(chunk, __QSPI_ADDRESS__(d), sizeof(chunk));
    p += sizeof(chunk);
    d += sizeof(chunk);
    size -= sizeof(chunk);
    processed += sizeof(chunk);
    int pct = 33 + (processed * 33 / total);
    ui_operation_update(pct);
  }
  LOG_DEBUG(MODULE, "copy storage from %p to %p done", p1, d1);
  LOG_DEBUG(MODULE, "copy storage from %p to %p ...", p2, d2);
  p = p2;
  d = d2;
  size = FLASH_STORAGE_SECTOR_SIZE;
  while (size > 0) {
    qspi_flash_read_buffer(chunk, __QSPI_ADDRESS__(p), sizeof(chunk));
    qspi_flash_write_buffer_unsafe(chunk, __QSPI_ADDRESS__(d), sizeof(chunk));
    p += sizeof(chunk);
    d += sizeof(chunk);
    size -= sizeof(chunk);
    processed += sizeof(chunk);
    int pct = 33 + (processed * 33 / total);
    ui_operation_update(pct);
  }
  LOG_DEBUG(MODULE, "copy storage from %p to %p done", p2, d2);

  // step 3: erase old storage
  LOG_DEBUG(MODULE, "erase old storage %p and %p", p1, p2);
  qspi_flash_erase_block_64k(__QSPI_ADDRESS__(p1));
  ui_operation_update(83);
  qspi_flash_erase_block_64k(__QSPI_ADDRESS__(p2));
  ui_operation_update(100);
  LOG_DEBUG(MODULE, "storage move done");
}

void upgrader_begin_install(const char* path, image_type_t type) {
  LOG_DEBUG(MODULE, "begin install %s: %s", image_get_desc_name(type), path);
  if (type == IMAGE_TYPE_BOOTLOADER) {
    boot_ctx.upgrader_op.state = UPGRADER_OP_STATE_BOOT_INSTALL;
    boot_ctx.upgrader_op.path = path;
    // update time
    boot_ctx.upgrader_op.active_time = HAL_GetTick();
  } else if (type == IMAGE_TYPE_FIRMWARE) {
    boot_ctx.upgrader_op.state = UPGRADER_OP_STATE_FIRMWARE_INSTALL;
    boot_ctx.upgrader_op.path = path;
    // update time
    boot_ctx.upgrader_op.active_time = HAL_GetTick();
  } else if (type == IMAGE_TYPE_SE) {
    boot_ctx.upgrader_op.state = UPGRADER_OP_STATE_SE_INSTALL;
    boot_ctx.upgrader_op.path = path;
    // update time
    boot_ctx.upgrader_op.active_time = HAL_GetTick();
  } else {
    LOG_DEBUG(MODULE, "unknown image type %d", type);
  }
}

void upgrader_op_poll(void) {
  if (boot_ctx.upgrader_op.state == UPGRADER_OP_STATE_NONE) {
    return;
  } else if (boot_ctx.upgrader_op.state == UPGRADER_OP_STATE_BOOT_INSTALL) {
    // delay 3s then begin install
    if (HAL_GetTick() < boot_ctx.upgrader_op.active_time + 3000 ) {
      return;
    }
    // begin install
    updater_update_image(boot_ctx.upgrader_op.path, false);
    boot_ctx.upgrader_op.state = UPGRADER_OP_STATE_BOOT_INSTALLING;
    return;
  } else if (boot_ctx.upgrader_op.state == UPGRADER_OP_STATE_BOOT_INSTALLING) {
    // do nothing
    return;
  } else if (boot_ctx.upgrader_op.state == UPGRADER_OP_STATE_BOOT_INSTALL_DONE) {
    // bootloader intall done, begin install firmware
    upgrader_begin_install(UPGRADER_BIN_FIRMWARE, IMAGE_TYPE_FIRMWARE);
    return;
  } else if (boot_ctx.upgrader_op.state == UPGRADER_OP_STATE_FIRMWARE_INSTALL) {
    // delay 3s then begin install
    if (HAL_GetTick() < boot_ctx.upgrader_op.active_time + 3000 ) {
      return;
    }
    // begin install
    updater_update_image(boot_ctx.upgrader_op.path, false);
    boot_ctx.upgrader_op.state = UPGRADER_OP_STATE_FIRMWARE_INSTALLING;
    return;
  } else if (boot_ctx.upgrader_op.state == UPGRADER_OP_STATE_FIRMWARE_INSTALLING) {
    // do nothing
    return;
  } else if (boot_ctx.upgrader_op.state == UPGRADER_OP_STATE_FIRMWARE_INSTALL_DONE) {
    // firmware install done, begin install se
    upgrader_begin_install(UPGRADER_BIN_SE, IMAGE_TYPE_SE);
  } else if (boot_ctx.upgrader_op.state == UPGRADER_OP_STATE_SE_INSTALL) {
    // delay 3s then begin install
    if (HAL_GetTick() < boot_ctx.upgrader_op.active_time + 3000 ) {
      return;
    }
    // begin install
    updater_update_image(boot_ctx.upgrader_op.path, false);
    boot_ctx.upgrader_op.state = UPGRADER_OP_STATE_SE_INSTALLING;
    return;
  } else if (boot_ctx.upgrader_op.state == UPGRADER_OP_STATE_SE_INSTALLING) {
    // do nothing
    return;
  } else if (boot_ctx.upgrader_op.state == UPGRADER_OP_STATE_SE_INSTALL_DONE) {
    // se install done, switch state to done
    // delay 3s then switch state to done
    if (HAL_GetTick() < boot_ctx.upgrader_op.active_time + 1000 ) {
      return;
    }
    // firmware install done
    ui_jump_to(ui_upgrader(UI_UPDATER_STATE_DONE));
    // switch state to none
    boot_ctx.upgrader_op.state = UPGRADER_OP_STATE_DONE;
    boot_ctx.upgrader_op.active_time = HAL_GetTick();
    return;
  } else if (boot_ctx.upgrader_op.state == UPGRADER_OP_STATE_INSTALL_FAILED) {
    // install failed
    ui_jump_to(ui_upgrader(UI_UPDATER_STATE_INSTALL_FAILED));
    // switch state to none
    boot_ctx.upgrader_op.state = UPGRADER_OP_STATE_NONE;
    return;
  } else if (boot_ctx.upgrader_op.state == UPGRADER_OP_STATE_DONE) {
    // delay 5s then restart
    if (HAL_GetTick() < boot_ctx.upgrader_op.active_time + 3000 ) {
      return;
    }
    boot_ctx.upgrader_op.state = UPGRADER_OP_STATE_NONE;
    LOG_DEBUG(MODULE, "Upgrade done, restarting ...");
    ui_event_broadcast(UI_EVENT_REBOOT, UI_REBOOT_TYPE_MAKE_PARAM(UI_REBOOT_TYPE_NORMAL));
  }
}

int main(void) {
#if defined(UPGRADER_DEBUG) && UPGRADER_DEBUG
  // when run in SDRAM, it can't config mpu again
  mpu_config_bootloader();
#else
  // in startup.s `SystemInit` will reset SCB->VTOR
  // set VTOR to vector table offset again
  extern uint32_t _vector_offset;
  SCB->VTOR = (uint32_t)&_vector_offset;
#endif

  memset((void*)SRAM_BASE_ADDRESS, 0, SRAM_SIZE);
  reset_flags_reset();
  periph_init();
  system_clock_config();
  __enable_irq();
  /* Enable the CPU Cache */
  cpu_cache_enable();
  rng_init();
  __stack_chk_guard = rng_get();

  // use log
  uart_log_init();

  random_delays_init();
  wait_random();
  flash_otp_init();
  device_para_init();
  bus_fault_enable();
#if defined (UPGRADER_DEBUG) && UPGRADER_DEBUG
  sdram_init();
#endif
  emmc_init();

  se_init();
  {
    uint8_t secret[32] = {0};
    if (device_get_pre_shared_key(secret)) {
      se_handshake(secret, sizeof(secret));
    }
    memzero(secret, sizeof(secret));
  }

  qspi_flash_init();
  qspi_flash_config();
  qspi_flash_memory_mapped();

  boot_context_init();
  fs_setup(&boot_ctx.fs);

  battery_init();
  pm_init();
  i2c4_init();
  display_backlight(0);
  timer_init();
  touch_init();
  lcd_init(DISPLAY_RESX, DISPLAY_RESY, LCD_PIXEL_FORMAT_RGB565);
  lcd_pwm_init();
  display_backlight(80);

  boot_power_key_init();
  LOG_DEBUG(MODULE, "lvgl init...");
  lv_init();
  lv_port_disp_init();
  lv_port_indev_init();
  lv_port_fs_init(&boot_ctx.fs);
  ui_event_register();
  ui_init();
  ui_status_bar_init();

  // only bootloader and firmware are verified
  bool verified = verify_bootloader() && verify_firmware() && verify_se();

  ui_jump_to(ui_upgrader(verified ? UI_UPDATER_STATE_NONE : UI_UPDATER_STATE_VERIFY_FAILED));
  LOG_DEBUG(MODULE, "upgrader init done");
  if (verified) {
    try_move_storage();
#if defined(UPGRADER_DEBUG) && UPGRADER_DEBUG
    // for test, begin install firmware, upgrader stored in flash at 0x08000000
    upgrader_begin_install(UPGRADER_BIN_FIRMWARE, IMAGE_TYPE_FIRMWARE);
#else
    upgrader_begin_install(UPGRADER_BIN_BOOT, IMAGE_TYPE_BOOTLOADER);
#endif
  }
  while(1) {
      boot_updater_poll();
      boot_power_key_poll();
      boot_fs_msg_monitor_poll();
      upgrader_op_poll();
      lv_timer_handler();
      lvgl_last_active_time_monitor();
      __WFI();
      __WFE();
  };

  while(1);
}
