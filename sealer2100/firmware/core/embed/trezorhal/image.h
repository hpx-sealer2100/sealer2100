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

#ifndef __TREZORHAL_IMAGE_H__
#define __TREZORHAL_IMAGE_H__

#include <assert.h>
#include <stdint.h>

#include "flash.h"
#include "keys.h"
#include "secbool.h"
#include "sha2.h"

#define IMAGE_HEADER_SIZE            0x400 // size of the bootloader or firmware header
#define IMAGE_SIG_SIZE               64    // ed25519 signature size, R||S

#define IMAGE_MIN_REQUIRED_SIG_COUNT 1

#define BOOTLOADER_START             0x08000000
#define FIRMWARE_START               0x08060000

#define BOOTLOADER_IMAGE_MAGIC       0x42545048 // HPTB
#define BOOTLOADER_IMAGE_MAX_SIZE    (BOOTLOADER_SECTORS_COUNT * FLASH_BOOTLOADER_SECTOR_SIZE)
#define FIRMWARE_IMAGE_MAGIC         0x46545048 // HPTF
#define FIRMWARE_IMAGE_MAX_SIZE      (FIRMWARE_SECTORS_COUNT * FLASH_FIRMWARE_SECTOR_SIZE)
#define BLE_IMAGE_MAGIC              0x33383235 // 5283
#define SE_IMAGE_MAGIC               0x39384854 // TH89
#define IRIS_IMAGE_MAGIC             0x53495249 // IRIS

typedef enum {
    IMAGE_TYPE_UNKNOWN,
    IMAGE_TYPE_BOOTLOADER,
    IMAGE_TYPE_FIRMWARE,
    IMAGE_TYPE_SE,
    IMAGE_TYPE_BLE,
    IMAGE_TYPE_IRIS,
} image_type_t;

typedef struct {
    union {
        uint32_t value;
        struct {
            uint8_t build;
            uint8_t patch;
            uint8_t minor;
            uint8_t major;
        };
    };
} image_version_t;

typedef struct {
    uint32_t magic;                // magic of the image, need be protected
    image_version_t version;       // version of the image, need be protected
    uint32_t header_size;          // size of the header, need be protected
    uint32_t code_size;            // size of the code, need be protected
    uint32_t required_sig_count;   // required signature count, need be protected
    uint8_t _reserved[0x80 - 20];  // reserved, for future use
    // filed under this filed, no need to be protected, fill 0x00 when compute the digest
    uint8_t digest[32];                                       // digest of the image
    uint32_t sig_mask;                                        // signature mask
    uint8_t sigs[KEY_N][IMAGE_SIG_SIZE];                      // signatures of the image, ed25519 signature
    uint8_t padding[0x380 - 32 - 4 - KEY_N * IMAGE_SIG_SIZE]; // padding to 0x400
} image_header_t;

_Static_assert(
    IMAGE_HEADER_SIZE == sizeof(image_header_t), "IMAGE_HEADER_SIZE must be equal to sizeof(image_header_t)"
);

typedef struct {
    uint8_t vector_table[0x400];
    image_header_t header;
    uint8_t code[0];
} image_bootloader_t;

typedef struct {
    image_header_t header;
    uint8_t code[0];
} image_firmware_t;

#define BOOTLOADER ((const image_bootloader_t* const)BOOTLOADER_START)
#define FIRMWARE ((const image_firmware_t* const)FIRMWARE_START)

image_type_t image_get_type(const uint8_t* const data);

const char* image_get_desc_name(image_type_t image_type);

int image_version_parse(const char* str, image_version_t* version);
void image_version_format(image_version_t version, char str[12]);

// 0: a = b, +n: a > b, -n: a < b
int image_version_compare(image_version_t* const a, image_version_t* const b);

secbool __wur
load_ble_image_header(const uint8_t* const data, const uint32_t maxsize, image_header_t* const hdr);

static inline secbool __wur image_is_bootloader(uint32_t address) {
    return ((image_bootloader_t*)address)->header.magic == BOOTLOADER_IMAGE_MAGIC;
}

static inline secbool __wur image_is_firmware(uint32_t address) {
    return ((image_firmware_t*)address)->header.magic == FIRMWARE_IMAGE_MAGIC;
}

void image_print_header(const image_header_t* const header);

void image_header_hash_update(SHA256_CTX* ctx, const image_header_t* const header);

secbool __wur image_header_verify(const image_header_t* const header);

secbool __wur image_verify_bootloader(uint32_t address);

secbool __wur image_verify_firmware(uint32_t address);

#endif
