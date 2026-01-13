#ifndef __POWER_MANAGER_H__
#define __POWER_MANAGER_H__
typedef enum {
  POWER_SOURCE_USB,
  POWER_SOURCE_BATTERY,
} power_source_t;

#define pm_battery_exist() (1)
#define pm_get_power_source() (POWER_SOURCE_USB)

#endif
