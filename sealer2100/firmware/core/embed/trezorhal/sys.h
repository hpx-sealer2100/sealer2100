#ifndef _SYS_H_
#define _SYS_H_

#include <stdbool.h>
#include <stdint.h>

#define PERIOD_VALUE (uint32_t)(1000 - 1)
#define PULSE1_VALUE (uint32_t)(PERIOD_VALUE / 2)

void motor_init(void);
void motor_ctrl(bool on);

#define MOTOR_ON() motor_ctrl(true)
#define MOTOR_OFF() motor_ctrl(false)

#endif
