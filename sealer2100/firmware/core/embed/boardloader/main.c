/*
 * This file is part of the Trezor project, https://trezor.io/
 *
 * Copyright (c) SatoshiLabs
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <string.h>

#include "blake2s.h"
#include "common.h"
#include "compiler_traits.h"
#include "display.h"
#include "emmc.h"
#include "ff.h"
#include "flash.h"
#include "image.h"
#include "mipi_lcd.h"
#include "qspi_flash.h"
#include "rng.h"
#include "device.h"

#ifdef TREZOR_MODEL_T
#include "sdram.h"
#include "touch.h"
#endif

#include "usb.h"
#include "lowlevel.h"
#include "version.h"
#include "memzero.h"
#include "ble.h"
#include "usart.h"

#define BOARD_MODE 1
#define BOOT_MODE 2
#define PIXEL_STEP 5

// TODO: change verification policy
#if PRODUCTION
const uint8_t BOARDLOADER_KEY_M = 2;
const uint8_t BOARDLOADER_KEY_N = 3;
#else
const uint8_t BOARDLOADER_KEY_M = 2;
const uint8_t BOARDLOADER_KEY_N = 3;
#endif

// TODO add our own keys to verify firmware
// clang-format off
static const uint8_t * const BOARDLOADER_KEYS[] = {
#if PRODUCTION
(const uint8_t *)"\xEE\xD4\xE5\x12\x3D\xB0\x81\x4F\x2D\xDB\x86\x6A\x68\x86\x2F\x24\xE9\xA8\x78\x34\x3E\x69\xB0\xD2\x52\x25\xB9\x51\x12\xCA\xA3\x77",
(const uint8_t *)"\x30\x37\x00\xC4\x5C\xAD\x09\xD3\xDA\xBE\x17\x6E\x1B\xEC\x24\xF0\xA6\xA8\x5B\x80\x9F\x5E\x8E\x09\x57\x92\x57\x09\x10\x1B\xF8\xCF",
(const uint8_t *)"\x24\xED\x84\xE2\x6B\x73\x5D\x7C\x7A\x44\xE4\xB6\xE6\xAA\xD4\x43\xA5\x5E\x6A\xD7\x50\x41\x4B\x71\x00\x6A\xF0\xD0\x16\xC7\x63\x42",
#else
// Hypermate dev_key
(const uint8_t *)"\xdb\x99\x5f\xe2\x51\x69\xd1\x41\xca\xb9\xbb\xba\x92\xba\xa0\x1f\x9f\x2e\x1e\xce\x7d\xf4\xcb\x2a\xc0\x51\x90\xf3\x7f\xcc\x1f\x9d",
(const uint8_t *)"\x21\x52\xf8\xd1\x9b\x79\x1d\x24\x45\x32\x42\xe1\x5f\x2e\xab\x6c\xb7\xcf\xfa\x7b\x6a\x5e\xd3\x00\x97\x96\x0e\x06\x98\x81\xdb\x12",
(const uint8_t *)"\x22\xfc\x29\x77\x92\xf0\xb6\xff\xc0\xbf\xcf\xdb\x7e\xdb\x0c\x0a\xa1\x4e\x02\x5a\x36\x5e\xc0\xe3\x42\xe8\x6e\x38\x29\xcb\x74\xb6",
#endif
};
// clang-format on

#define BOARD_VERSION "HyperMate Boardloader 1.0.0\n"

extern volatile uint32_t system_reset;

static FATFS fs_instance;
PARTITION VolToPart[FF_VOLUMES] = {
    {0, 1},
    {0, 2},
};
uint32_t *emmc_buf = (uint32_t *)0x24000000;
uint32_t *emmc_buf1 = (uint32_t *)0x24000000 + IMAGE_HEADER_SIZE;

#define BOOT_EMMC_BLOCKS (2 * 1024 * 1024)
void fatfs_init(void) {
  FRESULT res;
  BYTE work[FF_MAX_SS];
  MKFS_PARM mk_para = {
      .fmt = FM_FAT32,
  };

  LBA_t plist[] = {
      BOOT_EMMC_BLOCKS,
      100};  // 1G sectors for 1st partition and left all for 2nd partition
  res = f_mount(&fs_instance, "", 1);
  if (res == FR_OK) return;

  if (res == FR_NO_FILESYSTEM) {
    res = f_fdisk(0, plist, work); /* Divide physical drive 0 */
    if (res) {
      display_printf("f_fdisk error%d\n", res);
    }
    res = f_mkfs("0:", &mk_para, work, sizeof(work));
    if (res) {
      display_printf("f_mkfs 0 error%d\n", res);
    }
    FATFS fs;
    res = f_mount(&fs, "0:", 1);
    if (res) {
      display_printf("fatfs mount error");
    }
    f_setlabel("0:System Data");
    f_unmount("0:");

    res = f_mkfs("1:", &mk_para, work, sizeof(work));
    if (res) {
      display_printf("fatfs Format error");
    }
    res = f_mount(&fs, "1:", 1);
    if (res) {
      display_printf("fatfs mount error");
    }
    f_setlabel("1:User Data");
    f_unmount("1:");
  } else {
    display_printf("mount err %d\n", res);
  }
}

int fatfs_check_res(void) {
  FRESULT res;
  FIL fsrc;
  res = f_mount(&fs_instance, "", 1);
  if (res != FR_OK) {
    display_printf("fatfs mount error");
    return 0;
  }
  res = f_open(&fsrc, "/res/.HYPERMATE_RESOURCE", FA_READ);
  if (res != FR_OK) {
    f_unmount("");
  }
  return res;
}

secbool check_emmc_image_contents(const image_header *const hdr,
                                     uint32_t firstskip, FIL fsrc) {
  void *data = emmc_buf1;

  FRESULT res;
  UINT num_of_read = 0;

  res = f_read(&fsrc, data, IMAGE_CHUNK_SIZE - firstskip, &num_of_read);
  if ((num_of_read != (IMAGE_CHUNK_SIZE - firstskip)) || (res != FR_OK)) {
    f_close(&fsrc);
    return secfalse;
  }

  int remaining = hdr->codelen;
  if (remaining <= IMAGE_CHUNK_SIZE - firstskip) {
    if (sectrue != check_single_hash(hdr->hashes, data,
                                     MIN(remaining, IMAGE_CHUNK_SIZE))) {
      return secfalse;
    } else {
      return sectrue;
    }
  }

  BLAKE2S_CTX ctx;
  uint8_t hash[BLAKE2S_DIGEST_LENGTH];
  blake2s_Init(&ctx, BLAKE2S_DIGEST_LENGTH);

  blake2s_Update(&ctx, data, MIN(remaining, IMAGE_CHUNK_SIZE - firstskip));
  int block = 1;
  int update_flag = 1;
  remaining -= IMAGE_CHUNK_SIZE - firstskip;
  while (remaining > 0) {
    res = f_read(&fsrc, data, MIN(remaining, IMAGE_CHUNK_SIZE), &num_of_read);
    if ((num_of_read != MIN(remaining, IMAGE_CHUNK_SIZE)) || (res != FR_OK)) {
      f_close(&fsrc);
      return secfalse;
    }

    if (remaining - IMAGE_CHUNK_SIZE > 0) {
      if (block % 2) {
        update_flag = 0;
        blake2s_Update(&ctx, data, MIN(remaining, IMAGE_CHUNK_SIZE));
        blake2s_Final(&ctx, hash, BLAKE2S_DIGEST_LENGTH);
        if (0 != memcmp(hdr->hashes + (block / 2) * 32, hash,
                        BLAKE2S_DIGEST_LENGTH)) {
          return secfalse;
        }
      } else {
        blake2s_Init(&ctx, BLAKE2S_DIGEST_LENGTH);
        blake2s_Update(&ctx, data, MIN(remaining, IMAGE_CHUNK_SIZE));
        update_flag = 1;
      }
    } else {
      if (update_flag) {
        blake2s_Update(&ctx, data, MIN(remaining, IMAGE_CHUNK_SIZE));
        blake2s_Final(&ctx, hash, BLAKE2S_DIGEST_LENGTH);
        if (0 != memcmp(hdr->hashes + (block / 2) * 32, hash,
                        BLAKE2S_DIGEST_LENGTH)) {
          return secfalse;
        }
      } else {
        if (sectrue != check_single_hash(hdr->hashes + (block / 2) * 32, data,
                                         MIN(remaining, IMAGE_CHUNK_SIZE))) {
          return secfalse;
        }
      }
    }
    block++;
    remaining -= MIN(remaining, IMAGE_CHUNK_SIZE);
  }
  return sectrue;
}

static secbool check_emmc(image_header *hdr) {
  FRESULT res;

  res = f_mount(&fs_instance, "", 1);
  if (res != FR_OK) {
    return secfalse;
  }
  uint64_t cap = emmc_get_capacity_in_bytes();
  if (cap < 1024 * 1024) {
    return secfalse;
  }

  memzero(emmc_buf, IMAGE_HEADER_SIZE);

  FIL fsrc;
  UINT num_of_read = 0;

  res = f_open(&fsrc, "/boot/bootloader.bin", FA_READ);
  if (res != FR_OK) {
    return secfalse;
  }
  res = f_read(&fsrc, emmc_buf, IMAGE_HEADER_SIZE, &num_of_read);
  if ((num_of_read != IMAGE_HEADER_SIZE) || (res != FR_OK)) {
    f_close(&fsrc);
    return secfalse;
  }

  secbool new_present = secfalse;

  new_present =
      load_image_header((const uint8_t *)emmc_buf, BOOTLOADER_IMAGE_MAGIC,
                        BOOTLOADER_IMAGE_MAXSIZE, BOARDLOADER_KEY_M,
                        BOARDLOADER_KEY_N, BOARDLOADER_KEYS, hdr);
  if (sectrue == new_present) {
    new_present = check_emmc_image_contents(hdr, IMAGE_HEADER_SIZE, fsrc);
  }

  f_close(&fsrc);
  return sectrue == new_present ? sectrue : secfalse;
}

static void progress_callback(int pos, int len) { display_printf("."); }

static secbool copy_emmc(uint32_t code_len) {
  display_backlight(255);

  display_printf(BOARD_VERSION);
  display_printf("=====================\n\n");

  display_printf("new version bootloader found\n\n");

  display_printf("\n\nerasing flash:\n\n");

  // erase all flash (except boardloader)
  static const uint8_t sectors[] = {
      FLASH_SECTOR_BOOTLOADER_1,
      FLASH_SECTOR_BOOTLOADER_2,
  };

  if (sectrue !=
      flash_erase_sectors(sectors, sizeof(sectors), progress_callback)) {
    display_printf(" failed\n");
    return secfalse;
  }
  display_printf(" done\n\n");

  ensure(flash_unlock_write(), NULL);

  display_printf("coping boardloader to flash ...\n\n");

  memzero(emmc_buf, EMMC_BLOCK_SIZE);

  FIL fsrc;
  FRESULT res;
  UINT num_of_read;
  res = f_open(&fsrc, "/boot/bootloader.bin", FA_READ);
  if (res != FR_OK) {
    return secfalse;
  }
  int blocks = (IMAGE_HEADER_SIZE + code_len) / EMMC_BLOCK_SIZE;
  int percent = 0, percent_bak = 0;
  for (int i = 0; i < blocks; i++) {
    percent = (i * 100) / blocks;
    if (percent != percent_bak) {
      percent_bak = percent;
      display_printf("%d ", percent);
    }

    f_lseek(&fsrc, i * EMMC_BLOCK_SIZE);
    res = f_read(&fsrc, emmc_buf, EMMC_BLOCK_SIZE, &num_of_read);
    if ((num_of_read != EMMC_BLOCK_SIZE) || (res != FR_OK)) {
      f_close(&fsrc);
      return secfalse;
    }
    if (i * EMMC_BLOCK_SIZE < FLASH_FIRMWARE_SECTOR_SIZE) {
      for (int j = 0; j < EMMC_BLOCK_SIZE / (sizeof(uint32_t) * 8); j++) {
        ensure(
            flash_write_words(FLASH_SECTOR_BOOTLOADER_1,
                              i * EMMC_BLOCK_SIZE + j * (sizeof(uint32_t) * 8),
                              (uint32_t *)&emmc_buf[8 * j]),
            NULL);
      }
    } else {
      for (int j = 0; j < EMMC_BLOCK_SIZE / (sizeof(uint32_t) * 8); j++) {
        ensure(flash_write_words(
                   FLASH_SECTOR_BOOTLOADER_2,
                   (i - FLASH_FIRMWARE_SECTOR_SIZE / EMMC_BLOCK_SIZE) *
                           EMMC_BLOCK_SIZE +
                       j * (sizeof(uint32_t) * 8),
                   (uint32_t *)&emmc_buf[8 * j]),
               NULL);
      }
    }
  }
  f_close(&fsrc);
  f_unlink("/boot/bootloader.bin");
  ensure(flash_lock_write(), NULL);

  display_printf("\ndone\n\n");
  display_printf("Device will be restart in 3 seconds\n");

  for (int i = 3; i >= 0; i--) {
    display_printf("%d ", i);
    hal_delay(1000);
  }
  HAL_NVIC_SystemReset();
  return sectrue;
}

void show_poweron_bar(void) {
  static bool forward = true;
  static uint32_t step = 0, location = 0, indicator = 0;

  if (forward) {
    step += PIXEL_STEP;
    if (step <= 90) {
      indicator += PIXEL_STEP;
    } else if (step <= 160) {
      location += PIXEL_STEP;
    } else if (step < 250) {
      location += PIXEL_STEP;
      indicator -= PIXEL_STEP;
    } else {
      forward = false;
    }
  } else {
    step -= PIXEL_STEP;
    if (step > 160) {
      location -= PIXEL_STEP;
      indicator += PIXEL_STEP;
    } else if (step > 90) {
      location -= PIXEL_STEP;
    } else if (step > 0) {
      indicator -= PIXEL_STEP;
    } else {
      forward = true;
    }
  }

  display_bar_radius(160, 352, 160, 4, COLOR_DARK, COLOR_BLACK, 2);
  display_bar_radius(160 + location, 352, indicator, 4, COLOR_WHITE,
                     COLOR_BLACK, 2);
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

int main(void) {
  volatile uint32_t startup_mode_flag = *STAY_IN_FLAG_ADDR;
  reset_flags_reset();

  periph_init();

  /* Enable the CPU Cache */
  cpu_cache_enable();

  system_clock_config();
  set_pcb_version();

  rng_init();

  flash_option_bytes_init();

  clear_otg_hs_memory();

  flash_otp_init();

  sdram_init();

  mpu_config();

  touch_init();
  emmc_init();
  fatfs_init();
  display_backlight(0);

  if (startup_mode_flag != STAY_IN_BOARDLOADER_FLAG &&
      startup_mode_flag != STAY_IN_BOOTLOADER_FLAG) {
  }


  uint32_t mode = 0;
  //bool factory_mode = false;

  if (startup_mode_flag == STAY_IN_BOARDLOADER_FLAG) {
    mode = BOARD_MODE;
    *STAY_IN_FLAG_ADDR = 0;
  }
  if (startup_mode_flag == STAY_IN_BOOTLOADER_FLAG) {
    mode = BOOT_MODE;
  }

#if !PRODUCTION
  if (mode == BOARD_MODE) {
    lcd_init(DISPLAY_RESX, DISPLAY_RESY, LCD_PIXEL_FORMAT_RGB565);
    lcd_pwm_init();
    //如果是指令强制进boardloader，加载U盘模式
    display_backlight(80);
    display_printf(BOARD_VERSION);
    display_printf("USB Mass Storage Mode\n");
    display_printf("======================\n\n");
    usb_msc_init();
    while (1) {
      if (system_reset == 1) {
        hal_delay(5);
        restart();
      }
    }
  }
#endif


  if (mode == BOOT_MODE) {
    *STAY_IN_FLAG_ADDR = STAY_IN_BOOTLOADER_FLAG;
    SCB_CleanDCache();
  }

  image_header hdr_inner, hdr_sd;

  const uint8_t sectors[] = {
      FLASH_SECTOR_BOOTLOADER_1,
      FLASH_SECTOR_BOOTLOADER_2,
  };
  secbool boot_hdr = secfalse, boot_present = secfalse;

  boot_hdr = load_image_header((const uint8_t *)BOOTLOADER_START,
                               BOOTLOADER_IMAGE_MAGIC, BOOTLOADER_IMAGE_MAXSIZE,
                               BOARDLOADER_KEY_M, BOARDLOADER_KEY_N,
                               BOARDLOADER_KEYS, &hdr_inner);

  if (sectrue == boot_hdr) {
    boot_present = check_image_contents(&hdr_inner, IMAGE_HEADER_SIZE, sectors,
                                        sizeof(sectors));
  }

  if (sectrue == check_emmc(&hdr_sd)) {
    lcd_init(DISPLAY_RESX, DISPLAY_RESY, LCD_PIXEL_FORMAT_RGB565);
    lcd_pwm_init();
    display_backlight(80);
    //EMMC上面找到了更高版本bootloader，进行升级
    if (sectrue == boot_hdr) { // trigger system reset
      if (memcmp(&hdr_sd.version, &hdr_inner.version, 4) >= 0) {
        return copy_emmc(hdr_sd.codelen) == sectrue ? 0 : 3;
      }
    } else {
      return copy_emmc(hdr_sd.codelen) == sectrue ? 0 : 3;
    }
  }
#if !PRODUCTION
  //boot_present = sectrue;  //调试bootloader的时候才打开
  if (boot_present == secfalse) {
    //如果Flash上面没有Boot，同时emmc上面也没有可用于升级的boot，则加载U盘模式
    lcd_init(DISPLAY_RESX, DISPLAY_RESY, LCD_PIXEL_FORMAT_RGB565);
    lcd_pwm_init();
    display_backlight(80);

    display_printf(BOARD_VERSION);
    display_printf("USB Mass Storage Mode\n");
    display_printf("======================\n\n");
    usb_msc_init();
    while (1) {
      if (system_reset == 1) {
        hal_delay(5);
        restart();
      }
    }
  }
#else
  (void)boot_present;
#endif
  jump_to(BOOTLOADER_START + IMAGE_HEADER_SIZE);

  return 0;
}
