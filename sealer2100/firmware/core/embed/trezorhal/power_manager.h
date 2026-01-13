#ifndef __POWER_MANAGER_H__
#define __POWER_MANAGER_H__
#include <stdint.h>
#include <stdbool.h>

typedef enum {
  POWER_SOURCE_USB,
  POWER_SOURCE_BATTERY,
} power_source_t;

typedef enum {
  POWER_MODULE_CAMERA,
  POWER_MODULE_BLUETOOTH,
  POWER_MODULE_MOTOR,
  POWER_MODULE_NFC,
}power_module_t;

int pm_init(void);
// power off `PM` self
int pm_power_off(void);
bool pm_battery_exist(void);
power_source_t pm_get_power_source(void);
int pm_power_up(power_module_t source);
int pm_power_down(power_module_t source);
uint32_t pm_button_read(void);
void pm_battery_long_press_reset(bool enable);

#endif
