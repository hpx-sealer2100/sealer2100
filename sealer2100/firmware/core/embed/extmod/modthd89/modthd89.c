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

#include "py/objstr.h"
#include "py/runtime.h"

#include "se.h"

/// package: thd89

/// def get_version() -> str:
///     """
///     Read SE version
///     """
STATIC mp_obj_t mod_thd89_get_version(void) {
  char version[33];
  if (se_get_version(version)) {
    mp_raise_ValueError("read SE version failed");
  }
  return mp_obj_new_str_copy(&mp_type_str, (uint8_t*)version, strlen(version));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mod_thd89_get_version_obj,
                                 mod_thd89_get_version);

/// def get_public_key(
/// ) -> bytes:
///     """
///     Read device public key.
///     """
STATIC mp_obj_t mod_thd89_get_public_key(void) {
  uint8_t pk[65];
  if (se_get_dev_pubkey(pk)) {
    mp_raise_ValueError("read public key failed");
  }
  return mp_obj_new_str_copy(&mp_type_bytes, pk, 65);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mod_thd89_get_public_key_obj,
                                 mod_thd89_get_public_key);

/// def read_certificate(
/// ) -> bytes:
///     """
///     Read device certificate.
///     """
STATIC mp_obj_t mod_thd89_read_certificate(void) {
  uint8_t cert[2048];
  size_t cert_len = sizeof(cert);
  if (se_read_certificate(cert, &cert_len)) {
    mp_raise_ValueError("read certificate failed");
  }
  return mp_obj_new_str_copy(&mp_type_bytes, cert, cert_len);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_0(mod_thd89_read_certificate_obj,
                                 mod_thd89_read_certificate);

/// def sign_message(msg: bytes) -> bytes:
///     """
///     Sign message.
///     """
STATIC mp_obj_t mod_thd89_sign_message(mp_obj_t msg) {
  uint8_t signature[64];
  mp_buffer_info_t msg_info = {0};
  mp_get_buffer_raise(msg, &msg_info, MP_BUFFER_READ);

  if (se_sign_message(msg_info.buf, msg_info.len, signature)) {
    mp_raise_ValueError("sign message failed");
  }
  return mp_obj_new_str_copy(&mp_type_bytes, signature, 64);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_thd89_sign_message_obj,
                                 mod_thd89_sign_message);

/// def protect_user_storage() -> None:
///     """
///     make SE user storage protected.
///     """

STATIC mp_obj_t mod_thd89_protect_user_storage(void) {
  se_protect_user_storage();
  return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mod_thd89_protect_user_storage_obj,
                                 mod_thd89_protect_user_storage);

STATIC const mp_rom_map_elem_t mod_thd89_globals_table[] = {
    {MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_se_thd89)},
    {MP_ROM_QSTR(MP_QSTR_get_version),
     MP_ROM_PTR(&mod_thd89_get_version_obj)},
    {MP_ROM_QSTR(MP_QSTR_get_public_key),
     MP_ROM_PTR(&mod_thd89_get_public_key_obj)},
    {MP_ROM_QSTR(MP_QSTR_read_certificate),
     MP_ROM_PTR(&mod_thd89_read_certificate_obj)},
    {MP_ROM_QSTR(MP_QSTR_sign_message),
     MP_ROM_PTR(&mod_thd89_sign_message_obj)},
    {MP_ROM_QSTR(MP_QSTR_protect_user_storage),
     MP_ROM_PTR(&mod_thd89_protect_user_storage_obj)},
};
STATIC MP_DEFINE_CONST_DICT(mod_thd89_globals,
                            mod_thd89_globals_table);

const mp_obj_module_t mod_thd89 = {
    .base = {&mp_type_module},
    .globals = (mp_obj_dict_t *)&mod_thd89_globals,
};

MP_REGISTER_MODULE(MP_QSTR_thd89, mod_thd89);
