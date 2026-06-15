#include "iris.h"

#include "mpconfigport.h"
#include "embed/extmod/trezorobj.h"
#include "device.h"
#include <string.h>


/// package: trezorio.__init__

/// class Iris:
///     """
///     Iris device
///     """
///     def __init__(self):
///         ...
///     def is_opened(self) -> bool:
///         ...
///     def open(self):
///         ...
///     def close(self):
///         ...
///     def write(self, msg: bytes):
///         ...
///     def sec_channel_open(self):
///         ...
///     def __enter__(self) -> 'Iris':
///         ...
///     def __exit__(
///         self, type: type[BaseException] | None,
///         value: BaseException | None,
///         traceback: TracebackType | None,
///     ) -> None:
///         ...

typedef struct {
  mp_obj_base_t base;
  bool opened;
} mp_obj_Iris_t;

static mp_obj_t mod_trezorio_Iris_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
  mp_obj_Iris_t *o = m_new_obj(mp_obj_Iris_t);
  o->base.type = type;
  o->opened = false;
  return MP_OBJ_FROM_PTR(o);
}

static mp_obj_t mod_trezorio_Iris_is_opened(mp_obj_t self) {
  mp_obj_Iris_t *o = MP_OBJ_TO_PTR(self);
  return mp_obj_new_bool(o->opened);
}

static MP_DEFINE_CONST_FUN_OBJ_1(mod_trezorio_Iris_is_opened_obj, mod_trezorio_Iris_is_opened);

static mp_obj_t mod_trezorio_Iris_open(mp_obj_t self) {
    mp_obj_Iris_t *o = MP_OBJ_TO_PTR(self);

    if (o->opened) {
        return mp_const_none;
    }

    iris_open();
    o->opened = true;
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(mod_trezorio_Iris_open_obj, mod_trezorio_Iris_open);

static mp_obj_t mod_trezorio_Iris_close(mp_obj_t self) {
    mp_obj_Iris_t *o = MP_OBJ_TO_PTR(self);

    iris_close();
    o->opened = false;
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(mod_trezorio_Iris_close_obj, mod_trezorio_Iris_close);

static mp_obj_t mod_trezorio_Iris_write(mp_obj_t self, mp_obj_t msg) {
    mp_obj_Iris_t *o = MP_OBJ_TO_PTR(self);

    if (!o->opened) {
        mp_raise_ValueError("iris is not opend");
    }
    mp_buffer_info_t buf = {0};
    mp_get_buffer_raise(msg, &buf, MP_BUFFER_READ);
    iris_usart_send(buf.buf, buf.len);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(mod_trezorio_Iris_write_obj, mod_trezorio_Iris_write);

static mp_obj_t mod_trezorio_Iris_sec_channel_open(mp_obj_t self) {
    mp_obj_Iris_t *o = MP_OBJ_TO_PTR(self);
    if (!o->opened) {
        mp_raise_ValueError("iris is not opend");
    }
    uint8_t key[32] = {0};
    if (!device_get_iris_pre_shared_key(key)) {
        mp_raise_ValueError("get pre shared key failed");
    }
    int ret = iris_sec_channel_open(key, sizeof(key));
    memset(key, 0, sizeof(key));
    if (ret < 0) {
        mp_raise_ValueError("sec channel open failed");
    } else if (ret == IRIS_HANDSHAKE_PENDING) {
        return mp_obj_new_int(1);
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(mod_trezorio_Iris_sec_channel_open_obj, mod_trezorio_Iris_sec_channel_open);

static mp_obj_t mod_trezorio_Iris___exit__(size_t n_args, const mp_obj_t *args) {
    mp_obj_Iris_t *o = MP_OBJ_TO_PTR(args[0]);
    if (!o->opened) {
        return mp_const_none;
    }

    iris_close();
    o->opened = false;
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mod_trezorio_Iris___exit___obj, 4, 4, mod_trezorio_Iris___exit__);

static mp_obj_t mod_trezorio_Iris___del__(mp_obj_t self) {
    mp_obj_Iris_t *o = MP_OBJ_TO_PTR(self);
    if (o->opened) {
        iris_close();
    }
    o->opened = false;
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_trezorio_Iris___del___obj, mod_trezorio_Iris___del__);


static const mp_rom_map_elem_t mod_trezorio_Iris_locals_dict_table[] = {
  { MP_ROM_QSTR(MP_QSTR_is_opened), MP_ROM_PTR(&mod_trezorio_Iris_is_opened_obj) },
  { MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&mod_trezorio_Iris_open_obj) },
  { MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&mod_trezorio_Iris_close_obj) },
  { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&mod_trezorio_Iris_write_obj) },
  { MP_ROM_QSTR(MP_QSTR_sec_channel_open), MP_ROM_PTR(&mod_trezorio_Iris_sec_channel_open_obj) },
  { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&mod_trezorio_Iris___del___obj) },
  { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&mp_identity_obj) },
  { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&mod_trezorio_Iris___exit___obj) },
};

static MP_DEFINE_CONST_DICT(mp_type_Iris_locals_dict, mod_trezorio_Iris_locals_dict_table);

static const mp_obj_type_t mod_trezorio_Iris_type = {
  { &mp_type_type },
  .name = MP_QSTR_Iris,
  .make_new = mod_trezorio_Iris_make_new,
  .locals_dict = (mp_obj_dict_t *)&mp_type_Iris_locals_dict,
};


