#include STM32_HAL_H

#include <stdbool.h>
#include "sys.h"
#include "device.h"
#include "power_manager.h"

void motor_init(void) {
}

void motor_ctrl(bool on) {
  if (on) {
    pm_power_up(POWER_MODULE_MOTOR);
  } else {
    pm_power_down(POWER_MODULE_MOTOR);
  }
}
