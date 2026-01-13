#ifndef TREZORHAL_BATTERY_H_
#define TREZORHAL_BATTERY_H_
#include "stdbool.h"

#define BATTERY_I2C_PORT    GPIOB
#define BATTERY_I2C_SCL_PIN GPIO_PIN_6
#define BATTERY_I2C_SDA_PIN GPIO_PIN_7

//  return 单位1mA 充电的时候为﹢，放电的时候为-
int battery_read_current(void); // 读电流
//  return 单位1mV
int battery_read_voltage(void); // 读电压
//  return 单位0.1度开尔文273.15卡尔文为0摄氏度
int battery_read_temperature(void); // 读温度
//  return 单位1mAh
int battery_read_remained_capacity(void); // 读剩余电量
//  return 单位1mAh
int battery_read_full_capacity(void); // 满电量
//  return 单位1mA 充电的时候为﹢，放电的时候为-
int battery_read_average_current(void); // 平均电流
// return 1% 电荷状态，即剩余电量百分比
int battery_read_SOC(void); // 读取充电的状态

// return 电池的设置电量单位1mAh
int battery_read_design_capacity(void); // 读设置电量
// return Unit: 1min. 电池剩余时间
int battery_read_time_to_empty(void); // 读剩余时间
// return Unit: 1%. 0---100%
int battery_read_state_of_health(void); // 读取电池的健康状况

int battery_init(void);
bool battery_is_exist(void);
#endif
