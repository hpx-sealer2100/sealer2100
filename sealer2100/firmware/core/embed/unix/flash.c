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

#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <string.h>

#include "common.h"
#include "flash.h"
#include "profile.h"
#include "secbool.h"

#ifndef FLASH_FILE
#define FLASH_FILE profile_flash_path()
#endif

const uint8_t FIRMWARE_SECTORS[FIRMWARE_SECTORS_COUNT] = {
    FLASH_SECTOR_FIRMWARE_START,
    4,
    5,
    6,
    7,
    8,
    9,
    10,
    11,
    12,
    13,
    FLASH_SECTOR_FIRMWARE_END,
    FLASH_SECTOR_FIRMWARE_EXTRA_START,
    17,
    18,
    19,
    20,
    21,
    22,
    23,
    24,
    25,
    26,
    27,
    28,
    29,
    30,
    FLASH_SECTOR_FIRMWARE_EXTRA_END
};

const uint8_t STORAGE_SECTORS[STORAGE_SECTORS_COUNT] = {
    FLASH_SECTOR_STORAGE_1,
    FLASH_SECTOR_STORAGE_2,
};

static uint8_t *FLASH_BUFFER = NULL;
static uint32_t FLASH_SIZE;

static void flash_exit(void) {
  int r = munmap(FLASH_BUFFER, FLASH_SIZE);
  ensure(sectrue * (r == 0), "munmap failed");
}

void flash_init(void) {
  if (FLASH_BUFFER) return;

  FLASH_SIZE = STORAGE_SECTORS_COUNT * FLASH_STORAGE_SECTOR_SIZE;

  // check whether the file exists and it has the correct size
  struct stat sb;
  int r = stat(FLASH_FILE, &sb);

  // (re)create if non existent or wrong size
  if (r != 0 || sb.st_size != FLASH_SIZE) {
    int fd = open(FLASH_FILE, O_RDWR | O_CREAT | O_TRUNC, (mode_t)0600);
    ensure(sectrue * (fd >= 0), "open failed");
    for (int i = 0; i < FLASH_SIZE / 16; i++) {
      ssize_t s = write(
          fd,
          "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF",
          16);
      ensure(sectrue * (s >= 0), "write failed");
    }
    r = close(fd);
    ensure(sectrue * (r == 0), "close failed");
  }

  // mmap file
  int fd = open(FLASH_FILE, O_RDWR);
  ensure(sectrue * (fd >= 0), "open failed");

  void *map = mmap(0, FLASH_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  ensure(sectrue * (map != MAP_FAILED), "mmap failed");

  FLASH_BUFFER = (uint8_t *)map;

  atexit(flash_exit);
}

secbool flash_unlock_write(void) { return sectrue; }

secbool flash_lock_write(void) { return sectrue; }

const void *flash_get_address(uint8_t sector, uint32_t offset, uint32_t size) {
  if (sector >= FLASH_SECTOR_STORAGE_1 && sector <= FLASH_SECTOR_STORAGE_2) {
    if (offset + size > FLASH_STORAGE_SECTOR_SIZE) {
      return NULL;
    }
    return (const void *)(FLASH_BUFFER +
                          (sector - FLASH_SECTOR_STORAGE_1) *
                              FLASH_STORAGE_SECTOR_SIZE +
                          offset);
  }
  return NULL;
}

uint32_t flash_sector_size(uint8_t sector) {
  if (sector >= FLASH_SECTOR_STORAGE_1 && sector <= FLASH_SECTOR_STORAGE_2) {
    return FLASH_STORAGE_SECTOR_SIZE;
  }
  return 0;
}

secbool flash_erase_sectors(const uint8_t *sectors, int len,
                            void (*progress)(int pos, int len)) {
  if (progress) {
    progress(0, len);
  }
  for (int i = 0; i < len; i++) {
    const uint8_t sector = sectors[i];
    if (sector >= FLASH_SECTOR_STORAGE_1 && sector <= FLASH_SECTOR_STORAGE_2) {
      const uint32_t offset = FLASH_STORAGE_SECTOR_SIZE * (sector - FLASH_SECTOR_STORAGE_1);
      const uint32_t size = FLASH_STORAGE_SECTOR_SIZE;
      memset(FLASH_BUFFER + offset, 0xFF, size);
    }
    if (progress) {
      progress(i + 1, len);
    }
  }
  return sectrue;
}

secbool flash_write_byte(uint8_t sector, uint32_t offset, uint8_t data) {
  uint8_t *flash = (uint8_t *)flash_get_address(sector, offset, 1);
  if (!flash) {
    return secfalse;
  }
  if ((flash[0] & data) != data) {
    return secfalse;  // we cannot change zeroes to ones
  }
  flash[0] = data;
  return sectrue;
}

secbool flash_write_byte4(uint8_t sector, uint32_t offset, uint8_t data[4]) {
  return flash_write_bytes(sector, offset, data, 4);
}

secbool flash_write_bytes(uint8_t sector, uint32_t offset, uint8_t *data,
                          uint32_t len) {
  uint8_t *flash = (uint8_t *)flash_get_address(sector, offset, len);
  if (!flash) {
    return secfalse;
  }
  for (uint32_t i = 0; i < len; i++) {
    if ((flash[i] & data[i]) != data[i]) {
      return secfalse;  // we cannot change zeroes to ones
    }
  }
  memcpy(flash, data, len);
  return sectrue;
}

secbool flash_write_word(uint8_t sector, uint32_t offset, uint32_t data) {
  if (offset % sizeof(uint32_t)) {  // we write only at 4-byte boundary
    return secfalse;
  }
  uint32_t *flash = (uint32_t *)flash_get_address(sector, offset, sizeof(data));
  if (!flash) {
    return secfalse;
  }
  if ((flash[0] & data) != data) {
    return secfalse;  // we cannot change zeroes to ones
  }
  flash[0] = data;
  return sectrue;
}

secbool flash_otp_read(uint8_t block, uint8_t offset, uint8_t *data,
                       uint8_t datalen) {
  return secfalse;
}

secbool flash_otp_write(uint8_t block, uint8_t offset, const uint8_t *data,
                        uint8_t datalen) {
  return secfalse;
}

secbool flash_otp_lock(uint8_t block) { return secfalse; }

secbool flash_otp_is_locked(uint8_t block) { return secfalse; }
