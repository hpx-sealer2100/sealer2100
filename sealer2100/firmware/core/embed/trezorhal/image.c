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

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "common.h"
#include "flash.h"
#include "image.h"
#include "keys.h"
#include "secbool.h"
#include "sha2.h"
#include "log.h"
#include "ed25519-donna/ed25519.h"
#include "uart_log.h"

#define MODULE "image"


image_type_t image_get_type(const uint8_t* const data) {
  uint32_t magic = *(uint32_t*)data;
  if (magic == FIRMWARE_IMAGE_MAGIC) {
    return IMAGE_TYPE_FIRMWARE;
  } else if (magic == BLE_IMAGE_MAGIC) {
    return IMAGE_TYPE_BLE;
  } else if (magic == SE_IMAGE_MAGIC) {
    return IMAGE_TYPE_SE;
  } else if (magic == IRIS_IMAGE_MAGIC) {
    return IMAGE_TYPE_IRIS;
  }
  const image_bootloader_t* const bl = (const image_bootloader_t*)data;
  if (bl->header.magic == BOOTLOADER_IMAGE_MAGIC) {
    return IMAGE_TYPE_BOOTLOADER;
  }
  return IMAGE_TYPE_UNKNOWN;
}

const char* image_get_desc_name(image_type_t image_type) {
  if (image_type == IMAGE_TYPE_BOOTLOADER) {
    return "Bootloader";
  } else if (image_type == IMAGE_TYPE_FIRMWARE) {
    return "Firmware";
  } else if (image_type == IMAGE_TYPE_BLE) {
    return "Bluetooth";
  } else if (image_type == IMAGE_TYPE_SE) {
    return "SE";
  } else if (image_type == IMAGE_TYPE_IRIS) {
    return "IRIS";
  }
  return "Unknown firmware type";
}

int image_version_parse(const char *str, image_version_t *version) {
  uint32_t major = 0, minor = 0, patch = 0;
  if (memcmp(str, "None", 4) == 0) {
    version->major = 0;
    version->minor = 0;
    version->patch = 0;
    return 0;
  }
  int ret = sscanf(str, "%lu.%lu.%lu", &major, &minor, &patch);
  if (ret != 3) {
    return -1;
  }
  version->major = major & 0xFF;
  version->minor = minor & 0xFF;
  version->patch = patch & 0xFF;
  return 0;
}
void image_version_format(image_version_t version, char str[12]) {
  // 3*3+2+1
  // major.minor.patch
  sprintf(str, "%d.%d.%d", version.major, version.minor, version.patch);
}

int image_version_compare(image_version_t* const a, image_version_t* const b) {
    if (a->major < b->major) return -1;
    if (a->major > b->major) return 1;

    if (a->minor < b->minor) return -1;
    if (a->minor > b->minor) return 1;

    if (a->patch < b->patch) return -1;
    if (a->patch > b->patch) return 1;
    return 0;
}

secbool load_ble_image_header(const uint8_t *const data,
                              const uint32_t maxsize, image_header_t *const hdr) {
  memcpy(&hdr->magic, data, 4);
  // only use magic and version field
  if (hdr->magic != BLE_IMAGE_MAGIC) return secfalse;
  memcpy(&hdr->version, data + 4, 4);
  return sectrue;
}

void image_header_hash_update(SHA256_CTX *ctx, const image_header_t * const header) {
  // `header`: `magic` `version` `header_size` `code_size` `required_sig_count` is be protected
  size_t protected = offsetof(image_header_t, digest);
  sha256_Update(ctx, (const uint8_t *)header, protected);

  uint8_t filled[sizeof(image_header_t) - protected];
  memset(filled, 0x00, sizeof(filled));
  sha256_Update(ctx, filled, sizeof(filled));
}

void image_print_header(const image_header_t * const header) {
  (void)header;
  LOG_DEBUG(MODULE, "magic: 0x%08x", header->magic);
  LOG_DEBUG(MODULE, "header_size: 0x%08x", header->header_size);
  LOG_DEBUG(MODULE, "code_size: 0x%08x", header->code_size);
  LOG_DEBUG(MODULE, "version: 0x%08x", header->version.value);
  LOG_DEBUG(MODULE, "required_sig_count: 0x%02x", header->required_sig_count);
  LOG_HEXDUMP_DEBUG(MODULE, "digest", header->digest, sizeof(header->digest));
  LOG_DEBUG(MODULE, "sigmask: 0x%08x", header->sig_mask);
  uart_log_flush();
  for (int i = 0; i < sizeof(header->sig_mask)*8; i++) {
    bool present = (header->sig_mask & (1 << i)) != 0;
    if (present) {
      char label[17] = {0};
      snprintf(label, sizeof(label), "[%d] sig", i);
      LOG_HEXDUMP_DEBUG(MODULE, label, header->sigs[i], IMAGE_SIG_SIZE);
      uart_log_flush();
    }
  }
}

secbool __wur
image_header_verify(const image_header_t * const header) {
  if (header->required_sig_count < IMAGE_MIN_REQUIRED_SIG_COUNT) {
    if (PRODUCTION) {
      return secfalse;
    } else {
      LOG_DEBUG(MODULE, "Not config `required_sig_count` in develop mode, skip...");
      return sectrue;
    }
  }
  int count = 0;
  for (int i = 0; i < sizeof(header->sig_mask)*8; i++) {
    bool present = (header->sig_mask & (1 << i)) != 0;
    if (present) {
      if (i >= KEY_N) {
        LOG_DEBUG(MODULE, "pubkey index %d is out of range", i);
        return secfalse;
      }
      LOG_DEBUG(MODULE, "use %d pubkey verify sig", i);
      LOG_HEXDUMP_DEBUG(MODULE, "pubkey", KEYS[i], KEY_SIZE);
      LOG_HEXDUMP_DEBUG(MODULE, "sig", header->sigs[i], IMAGE_SIG_SIZE);
      uart_log_flush();
      if (0 != ed25519_sign_open(header->digest, sizeof(header->digest), KEYS[i], header->sigs[i])) {
        LOG_ERROR(MODULE, "verify sig %d failed", i);
        return secfalse;
      }
      LOG_DEBUG(MODULE, "verify sig %d success", i);
      count++;
    }
  }
  if (count >= header->required_sig_count) {
    return sectrue;
  } else if (PRODUCTION){
    LOG_DEBUG(MODULE, "sign count %d < required %d in develop mode, skip...", count, header->required_sig_count);
    return sectrue;
  }
  return secfalse;
}

secbool __wur
image_verify_bootloader(uint32_t address) {
  image_bootloader_t* const bl = (image_bootloader_t*)address;

  // step 0: check magic
  if (bl->header.magic != BOOTLOADER_IMAGE_MAGIC) return secfalse;

  // step 1: calculate digest
  SHA256_CTX ctx;
  sha256_Init(&ctx);

  // `vector_table` is be protected by signature
  sha256_Update(&ctx, bl->vector_table, sizeof(bl->vector_table));

  // `header` partly protected by signature
  image_header_hash_update(&ctx, &bl->header);

  // `code` is be protected by signature
  sha256_Update(&ctx, bl->code, bl->header.code_size - sizeof(image_bootloader_t));

  uint8_t digest[32];
  sha256_Final(&ctx, digest);
  LOG_HEXDUMP_DEBUG(MODULE, "bootloader computed digest", digest, sizeof(digest));
  LOG_HEXDUMP_DEBUG(MODULE, "bootloader stored digest", bl->header.digest, sizeof(bl->header.digest));

  // step 2: check digest
  if (memcmp(digest, bl->header.digest, sizeof(digest)) != 0) return secfalse;

  // step 3: verify signature
  return image_header_verify(&bl->header);
}

secbool __wur
image_verify_firmware(uint32_t address) {
  image_firmware_t* const fw = (image_firmware_t*)address;

  // step 0: check magic
  if (fw->header.magic != FIRMWARE_IMAGE_MAGIC) return secfalse;

  // step 1: calculate digest
  SHA256_CTX ctx;
  sha256_Init(&ctx);

  // `header` partly protected by signature
  image_header_hash_update(&ctx, &fw->header);

  // `code` is be protected by signature
  uint32_t code_size = fw->header.code_size;
  const uint8_t *sector = FIRMWARE_SECTORS;
  uint32_t sector_size = FLASH_FIRMWARE_SECTOR_SIZE;
  // the first sector contain `image_header_t`, header have hash update
  sector_size -= sizeof(image_firmware_t);
  code_size -= FLASH_FIRMWARE_SECTOR_SIZE;

  const uint8_t *addr = flash_get_address(*sector, sizeof(image_firmware_t), sector_size);
  sector++;

  sha256_Update(&ctx, addr, sector_size);

  while (code_size != 0) {
    sector_size = MIN(FLASH_FIRMWARE_SECTOR_SIZE, code_size);
    addr = flash_get_address(*sector, 0, sector_size);
    sector++;
    code_size -= sector_size;
    sha256_Update(&ctx, addr, sector_size);
  }

  uint8_t digest[32];
  sha256_Final(&ctx, digest);
  LOG_HEXDUMP_DEBUG(MODULE, "firmware computed digest", digest, sizeof(digest));
  LOG_HEXDUMP_DEBUG(MODULE, "firmware stored digest", fw->header.digest, sizeof(fw->header.digest));

  // step 2: check digest
  if (memcmp(digest, fw->header.digest, sizeof(digest)) != 0) return secfalse;

  // step 3: verify signature
  return image_header_verify(&fw->header);
}
