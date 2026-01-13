#include "battery.h"
#include "device.h"
#include "power_manager.h"
#include "embed/extmod/trezorobj.h"

/// package: trezorio.__init__

/// class Battery:
///     """
///     """
typedef struct _mp_obj_Battery_t {
    mp_obj_base_t base;
} mp_obj_Battery_t;

/// def __init__(self):
///     """
///     """
STATIC mp_obj_t mod_trezorio_battery_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 0, false);
    mp_obj_Battery_t *o = m_new_obj(mp_obj_Battery_t);
    o->base.type = type;
    return MP_OBJ_FROM_PTR(o);
}

/// def exist(self) -> bool:
///     """
///     check weather battery is present
///     """
static mp_obj_t mod_trezorio_battery_is_exist(mp_obj_t self) {
    return pm_battery_exist() ? mp_const_true : mp_const_false;
}
static MP_DEFINE_CONST_FUN_OBJ_1(mod_trezorio_battery_is_exist_obj, mod_trezorio_battery_is_exist);

/// def charging(self) -> bool:
///     """
///     check weather battery is charging
///     """
static mp_obj_t mod_trezorio_battery_is_charging(mp_obj_t self) {
    return pm_get_power_source() == POWER_SOURCE_USB ? mp_const_true : mp_const_false;
}
static MP_DEFINE_CONST_FUN_OBJ_1(mod_trezorio_battery_is_charging_obj, mod_trezorio_battery_is_charging);

/// def state_of_charge(self) -> int|None:
///     """
///     read SOC (state of charge), in percent
///
///     Returns None if the battery is not present
///     Returns int between 0 and 100
///     """

static mp_obj_t mod_trezorio_battery_state_of_charge(mp_obj_t self) {
    int state = battery_read_SOC();
    if (state < 0) {
        return mp_const_none;
    }
    return MP_OBJ_NEW_SMALL_INT(state);
}

static MP_DEFINE_CONST_FUN_OBJ_1(mod_trezorio_battery_state_of_charge_obj, mod_trezorio_battery_state_of_charge);

/// def current(self) -> int|None:
///     """
///     read (current), in 1mA
///
///     Returns current in mA, positive in discharging, negative is charging
///     """

static mp_obj_t mod_trezorio_battery_current(mp_obj_t self) {
    int current = battery_read_current();
    return MP_OBJ_NEW_SMALL_INT(current);
}

static MP_DEFINE_CONST_FUN_OBJ_1(mod_trezorio_battery_current_obj, mod_trezorio_battery_current);

/// def voltage(self) -> int:
///     """
///     read (voltage), in 1mV
///
///     Returns voltage in mV
///     """

static mp_obj_t mod_trezorio_battery_voltage(mp_obj_t self) {
    int voltage = battery_read_voltage();
    return MP_OBJ_NEW_SMALL_INT(voltage);
}

static MP_DEFINE_CONST_FUN_OBJ_1(mod_trezorio_battery_voltage_obj, mod_trezorio_battery_voltage);


static const mp_rom_map_elem_t mod_trezorio_battery_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_exist), MP_ROM_PTR(&mod_trezorio_battery_is_exist_obj)},
    { MP_ROM_QSTR(MP_QSTR_charging), MP_ROM_PTR(&mod_trezorio_battery_is_charging_obj)},
    { MP_ROM_QSTR(MP_QSTR_state_of_charge), MP_ROM_PTR(&mod_trezorio_battery_state_of_charge_obj) },
    { MP_ROM_QSTR(MP_QSTR_current), MP_ROM_PTR(&mod_trezorio_battery_current_obj) },
    { MP_ROM_QSTR(MP_QSTR_voltage), MP_ROM_PTR(&mod_trezorio_battery_voltage_obj) },
};

static MP_DEFINE_CONST_DICT(mod_trezorio_battery_locals_dict, mod_trezorio_battery_locals_dict_table);

static const mp_obj_type_t mod_trezorio_Battery_type = {
    { &mp_type_type },
    .name = MP_QSTR_Battery,
    .make_new = mod_trezorio_battery_make_new,
    .locals_dict = (mp_obj_dict_t *)&mod_trezorio_battery_locals_dict,
};
