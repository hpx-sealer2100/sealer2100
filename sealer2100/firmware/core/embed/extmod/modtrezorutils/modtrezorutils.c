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

#include "power_manager.h"
#include "py/objstr.h"
#include "py/runtime.h"
#ifndef TREZOR_EMULATOR
#include "stm32h7xx_hal.h"
#include "supervise.h"
#endif

#include "version.h"

#if MICROPY_PY_TREZORUTILS

#include "embed/extmod/modtrezorutils/modtrezorutils-meminfo.h"
#include "embed/extmod/trezorobj.h"

#include <string.h>
#include <stdio.h>
#include "blake2s.h"
#include "common.h"
#include "flash.h"
#include "usb.h"

#ifndef TREZOR_EMULATOR
#include "device.h"
#include "sdram.h"
#include "image.h"
#endif

static void ui_progress(mp_obj_t ui_wait_callback, uint32_t current,
                        uint32_t total) {
  if (mp_obj_is_callable(ui_wait_callback)) {
    mp_call_function_2_protected(ui_wait_callback, mp_obj_new_int(current),
                                 mp_obj_new_int(total));
  }
}

/// def consteq(sec: bytes, pub: bytes) -> bool:
///     """
///     Compares the private information in `sec` with public, user-provided
///     information in `pub`.  Runs in constant time, corresponding to a length
///     of `pub`.  Can access memory behind valid length of `sec`, caller is
///     expected to avoid any invalid memory access.
///     """
STATIC mp_obj_t mod_trezorutils_consteq(mp_obj_t sec, mp_obj_t pub) {
  mp_buffer_info_t secbuf = {0};
  mp_get_buffer_raise(sec, &secbuf, MP_BUFFER_READ);
  mp_buffer_info_t pubbuf = {0};
  mp_get_buffer_raise(pub, &pubbuf, MP_BUFFER_READ);

  size_t diff = secbuf.len - pubbuf.len;
  for (size_t i = 0; i < pubbuf.len; i++) {
    const uint8_t *s = (uint8_t *)secbuf.buf;
    const uint8_t *p = (uint8_t *)pubbuf.buf;
    diff |= s[i] - p[i];
  }

  if (diff == 0) {
    return mp_const_true;
  } else {
    return mp_const_false;
  }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mod_trezorutils_consteq_obj,
                                 mod_trezorutils_consteq);

/// def memcpy(
///     dst: bytearray | memoryview,
///     dst_ofs: int,
///     src: bytes,
///     src_ofs: int,
///     n: int | None = None,
/// ) -> int:
///     """
///     Copies at most `n` bytes from `src` at offset `src_ofs` to
///     `dst` at offset `dst_ofs`. Returns the number of actually
///     copied bytes. If `n` is not specified, tries to copy
///     as much as possible.
///     """
STATIC mp_obj_t mod_trezorutils_memcpy(size_t n_args, const mp_obj_t *args) {
  mp_arg_check_num(n_args, 0, 4, 5, false);

  mp_buffer_info_t dst = {0};
  mp_get_buffer_raise(args[0], &dst, MP_BUFFER_WRITE);
  uint32_t dst_ofs = trezor_obj_get_uint(args[1]);

  mp_buffer_info_t src = {0};
  mp_get_buffer_raise(args[2], &src, MP_BUFFER_READ);
  uint32_t src_ofs = trezor_obj_get_uint(args[3]);

  uint32_t n = 0;
  if (n_args > 4) {
    n = trezor_obj_get_uint(args[4]);
  } else {
    n = src.len;
  }

  size_t dst_rem = (dst_ofs < dst.len) ? dst.len - dst_ofs : 0;
  size_t src_rem = (src_ofs < src.len) ? src.len - src_ofs : 0;
  size_t ncpy = MIN(n, MIN(src_rem, dst_rem));

  memmove(((char *)dst.buf) + dst_ofs, ((const char *)src.buf) + src_ofs, ncpy);

  return mp_obj_new_int(ncpy);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mod_trezorutils_memcpy_obj, 4, 5,
                                           mod_trezorutils_memcpy);

/// def halt(msg: str | None = None) -> None:
///     """
///     Halts execution.
///     """
STATIC mp_obj_t mod_trezorutils_halt(size_t n_args, const mp_obj_t *args) {
  mp_buffer_info_t msg = {0};
  if (n_args > 0 && mp_get_buffer(args[0], &msg, MP_BUFFER_READ)) {
    ensure(secfalse, msg.buf);
  } else {
    ensure(secfalse, "halt");
  }
  return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mod_trezorutils_halt_obj, 0, 1,
                                           mod_trezorutils_halt);

/// def reset() -> None:
///     """
///     Reset system.
///     """
STATIC mp_obj_t mod_trezorutils_reset(void) {
  restart();
  return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mod_trezorutils_reset_obj,
                                 mod_trezorutils_reset);

/// def firmware_hash(
///     challenge: bytes | None = None,
///     callback: Callable[[int, int], None] | None = None,
/// ) -> bytes:
///     """
///     Computes the Blake2s hash of the firmware with an optional challenge as
///     the key.
///     """
STATIC mp_obj_t mod_trezorutils_firmware_hash(size_t n_args,
                                              const mp_obj_t *args) {
  BLAKE2S_CTX ctx;
  mp_buffer_info_t chal = {0};
  if (n_args > 0 && args[0] != mp_const_none) {
    mp_get_buffer_raise(args[0], &chal, MP_BUFFER_READ);
  }

  if (chal.len != 0) {
    if (blake2s_InitKey(&ctx, BLAKE2S_DIGEST_LENGTH, chal.buf, chal.len) != 0) {
      mp_raise_msg(&mp_type_ValueError, "Invalid challenge.");
    }
  } else {
    blake2s_Init(&ctx, BLAKE2S_DIGEST_LENGTH);
  }

  mp_obj_t ui_wait_callback = mp_const_none;
  if (n_args > 1 && args[1] != mp_const_none) {
    ui_wait_callback = args[1];
  }

  ui_progress(ui_wait_callback, 0, FIRMWARE_SECTORS_COUNT);
  for (int i = 0; i < FIRMWARE_SECTORS_COUNT; i++) {
    uint8_t sector = FIRMWARE_SECTORS[i];
    uint32_t size = flash_sector_size(sector);
    const void *data = flash_get_address(sector, 0, size);
    if (data == NULL) {
      mp_raise_msg(&mp_type_RuntimeError, "Failed to read firmware.");
    }
    blake2s_Update(&ctx, data, size);
    ui_progress(ui_wait_callback, i + 1, FIRMWARE_SECTORS_COUNT);
  }

  vstr_t vstr = {0};
  vstr_init_len(&vstr, BLAKE2S_DIGEST_LENGTH);
  if (blake2s_Final(&ctx, vstr.buf, vstr.len) != 0) {
    vstr_clear(&vstr);
    mp_raise_msg(&mp_type_RuntimeError, "Failed to finalize firmware hash.");
  }

  return mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mod_trezorutils_firmware_hash_obj, 0,
                                           2, mod_trezorutils_firmware_hash);

/// def reboot_to_bootloader() -> None:
///     """
///     Reboots to bootloader.
///     """
STATIC mp_obj_t mod_trezorutils_reboot_to_bootloader() {
// actual reboot via trezorhal goes here:
#if !TREZOR_EMULATOR
  reboot_to_boot();
#endif
  return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mod_trezorutils_reboot_to_bootloader_obj,
                                 mod_trezorutils_reboot_to_bootloader);

/// def reboot2boardloader() -> None:
///     """
///     Reboots to boardloader.
///     """
STATIC mp_obj_t mod_trezorutils_reboot2boardloader() {
#if !TREZOR_EMULATOR
  reboot_to_board();
#endif
  return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mod_trezorutils_reboot2boardloader_obj,
                                 mod_trezorutils_reboot2boardloader);

/// def firmware_version() -> str:
///     """
///     Returns the firmware version string.
///     """
STATIC mp_obj_t mod_trezorutils_firmware_version(void) {
#if TREZOR_EMULATOR
  return mp_obj_new_str_copy(&mp_type_str, (const uint8_t *)"EMULATOR", 8);
#else
  char version[12] = {0};
  image_version_format(FIRMWARE->header.version, version);
  return mp_obj_new_str_copy(&mp_type_str, (const uint8_t *)version, strlen(version));
#endif
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mod_trezorutils_firmware_version_obj,
                                 mod_trezorutils_firmware_version);

/// def boot_version() -> str:
///     """
///     Returns the bootloader version string.
///     """
STATIC mp_obj_t mod_trezorutils_boot_version(void) {
#ifdef TREZOR_EMULATOR
  return mp_obj_new_str_copy(&mp_type_str, (const uint8_t *)"EMULATOR", 8);
#else
  const image_bootloader_t* const bl = BOOTLOADER;
  char version[12] = {0};
  image_version_format(bl->header.version, version);
  return mp_obj_new_str_copy(&mp_type_str, (const uint8_t *)version, strlen(version));
#endif
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mod_trezorutils_boot_version_obj,
                                 mod_trezorutils_boot_version);

/// def board_version() -> str:
///     """
///     Returns the bootloader version string.
///     """
STATIC mp_obj_t mod_trezorutils_board_version(void) {
#ifdef TREZOR_EMULATOR
  return mp_obj_new_str_copy(&mp_type_str, (const uint8_t *)"EMULATOR", 8);
#else
  // we have combined boardloader and bootloader
  const image_bootloader_t* const bl = BOOTLOADER;
  char version[12] = {0};
  image_version_format(bl->header.version, version);
  return mp_obj_new_str_copy(&mp_type_str, (const uint8_t *)version, strlen(version));
#endif
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mod_trezorutils_board_version_obj,
                                 mod_trezorutils_board_version);
/// def usb_data_connected() -> bool:
///     """
///     Returns whether USB has been enumerated/configured
///     (and is not just connected by cable without data pins)
///     """
STATIC mp_obj_t mod_trezorutils_usb_data_connected() {
  return is_usb_configuard() == sectrue ? mp_const_true : mp_const_false;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mod_trezorutils_usb_data_connected_obj,
                                 mod_trezorutils_usb_data_connected);

STATIC mp_obj_str_t mod_trezorutils_revision_obj = {
    {&mp_type_bytes}, 0, sizeof(SCM_REVISION) - 1, (const byte *)SCM_REVISION};

STATIC mp_obj_str_t mod_trezorutils_build_id_obj = {
    {&mp_type_bytes}, 0, sizeof(BUILD_ID) - 1, (const byte *)BUILD_ID};

#define PASTER(s) MP_QSTR_##s
#define MP_QSTR(s) PASTER(s)

static mp_obj_t mod_trezorutils_power_off(void) {
#ifndef TREZOR_EMULATOR
  device_power_off();
#endif
  return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_0(mod_trezorutils_power_off_obj, mod_trezorutils_power_off);

/// def power_source() -> int:
///     """
///     return the device power source, 0: usb, 1: battery
///     """
STATIC mp_obj_t mod_trezorutils_power_source() {
  return MP_OBJ_NEW_SMALL_INT(pm_get_power_source());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mod_trezorutils_power_source_obj,
                                 mod_trezorutils_power_source);

/// SCM_REVISION: bytes
/// BUILD_ID: bytes
/// VERSION_MAJOR: int
/// VERSION_MINOR: int
/// VERSION_PATCH: int
/// MODEL: str
/// EMULATOR: bool
/// BITCOIN_ONLY: bool
/// FIRMWARE_SECTORS_COUNT: int

STATIC const mp_rom_map_elem_t mp_module_trezorutils_globals_table[] = {
    {MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_trezorutils)},
    {MP_ROM_QSTR(MP_QSTR_consteq), MP_ROM_PTR(&mod_trezorutils_consteq_obj)},
    {MP_ROM_QSTR(MP_QSTR_memcpy), MP_ROM_PTR(&mod_trezorutils_memcpy_obj)},
    {MP_ROM_QSTR(MP_QSTR_halt), MP_ROM_PTR(&mod_trezorutils_halt_obj)},
    {MP_ROM_QSTR(MP_QSTR_reboot_to_bootloader),
     MP_ROM_PTR(&mod_trezorutils_reboot_to_bootloader_obj)},
    {MP_ROM_QSTR(MP_QSTR_reboot2boardloader),
     MP_ROM_PTR(&mod_trezorutils_reboot2boardloader_obj)},
    {MP_ROM_QSTR(MP_QSTR_reset), MP_ROM_PTR(&mod_trezorutils_reset_obj)},
    {MP_ROM_QSTR(MP_QSTR_firmware_hash),
     MP_ROM_PTR(&mod_trezorutils_firmware_hash_obj)},
    {MP_ROM_QSTR(MP_QSTR_firmware_version),
     MP_ROM_PTR(&mod_trezorutils_firmware_version_obj)},
    {MP_ROM_QSTR(MP_QSTR_boot_version),
     MP_ROM_PTR(&mod_trezorutils_boot_version_obj)},
    {MP_ROM_QSTR(MP_QSTR_board_version),
     MP_ROM_PTR(&mod_trezorutils_board_version_obj)},

    {MP_ROM_QSTR(MP_QSTR_usb_data_connected),
     MP_ROM_PTR(&mod_trezorutils_usb_data_connected_obj)},
    {MP_ROM_QSTR(MP_QSTR_power_source), MP_ROM_PTR(&mod_trezorutils_power_source_obj)},
    // various built-in constants
    {MP_ROM_QSTR(MP_QSTR_SCM_REVISION),
     MP_ROM_PTR(&mod_trezorutils_revision_obj)},
    {MP_ROM_QSTR(MP_QSTR_BUILD_ID), MP_ROM_PTR(&mod_trezorutils_build_id_obj)},
    {MP_ROM_QSTR(MP_QSTR_VERSION_MAJOR), MP_ROM_INT(VERSION_MAJOR)},
    {MP_ROM_QSTR(MP_QSTR_VERSION_MINOR), MP_ROM_INT(VERSION_MINOR)},
    {MP_ROM_QSTR(MP_QSTR_VERSION_PATCH), MP_ROM_INT(VERSION_PATCH)},
#if defined TREZOR_MODEL_1
    {MP_ROM_QSTR(MP_QSTR_MODEL), MP_ROM_QSTR(MP_QSTR_1)},
#elif defined TREZOR_MODEL_T
    {MP_ROM_QSTR(MP_QSTR_MODEL), MP_ROM_QSTR(MP_QSTR_T)},
#elif defined TREZOR_MODEL_R
    {MP_ROM_QSTR(MP_QSTR_MODEL), MP_ROM_QSTR(MP_QSTR_R)},
#else
#error Unknown Trezor model
#endif
#ifdef TREZOR_EMULATOR
    {MP_ROM_QSTR(MP_QSTR_EMULATOR), mp_const_true},
    MEMINFO_DICT_ENTRIES
#else
    {MP_ROM_QSTR(MP_QSTR_EMULATOR), mp_const_false},
#endif
#if BITCOIN_ONLY
    {MP_ROM_QSTR(MP_QSTR_BITCOIN_ONLY), mp_const_true},
#else
    {MP_ROM_QSTR(MP_QSTR_BITCOIN_ONLY), mp_const_false},
#endif
    {MP_ROM_QSTR(MP_QSTR_power_off), MP_ROM_PTR(&mod_trezorutils_power_off_obj)},
};

STATIC MP_DEFINE_CONST_DICT(mp_module_trezorutils_globals,
                            mp_module_trezorutils_globals_table);

const mp_obj_module_t mp_module_trezorutils = {
    .base = {&mp_type_module},
    .globals = (mp_obj_dict_t *)&mp_module_trezorutils_globals,
};

MP_REGISTER_MODULE(MP_QSTR_trezorutils, mp_module_trezorutils);

#endif  // MICROPY_PY_TREZORUTILS
