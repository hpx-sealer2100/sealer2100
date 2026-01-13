#include "power_manager.h"

#include <stdint.h>

#include "i2c.h"
#include "power_manager.h"

#define IP6303_ADDR 0x60

// registers
#define PSTATE_CTL0 0x00
#define PSTATE_CTL1 0x01
#define PSTATE_CTL2 0x02
#define PSTATE_CTL3 0x03
#define PSTATE_SET  0x04
#define PPATH_CTL   0x05
#define PROTECT_CTL2 0x08
#define PROTECT_CTL3 0x09
#define PROTECT_CTL4 0x0A
#define PROTECT_CTL5 0x97
#define LDO_OCFLAG   0x0C
#define DCDC_GOOD    0x0D
#define LDO_GOOD     0x0E
#define PWRON_REC0   0x10
#define PWROFF_REC0  0x11
#define PWROFF_REC1  0x12
#define POFF_LDO     0x18
#define POFF_DCDC    0x19
#define WDOG_CTL0    0x1A
#define WDOG_CTL1    0xFF
#define LDO_MASK     0x1B
#define PWRON_REC1   0x1C
#define DC_CTL       0x20
#define DC1_VSET     0x21
#define DC2_VSET     0x26
#define DC3_VSET     0x2B
#define LDO_EN       0x40
#define LDOSW_EN     0x41
#define LDO2_VSEL    0x42
#define LDO3_VSEL    0x43
#define LDO4_VSEL    0x44
#define LDO5_VSEL    0x45
#define LDO6_VSEL    0x46
#define LDO7_VSEL    0x47
#define LDO_CTL0     0x48
#define LDO_CTL1     0x49
#define LDO_CTL2     0x4A
#define SLDO1_2_VSEL 0x4D
#define CHG_ANA_CTL0 0x50
#define CHG_ANA_CTL1 0x51
#define CHG_DIG_CTL0 0x53
#define CHG_DIG_CTL1 0x54
#define CHG_DIG_CTL2 0x55
#define NTC_ANA_CTL  0x56
#define NTC_DIG_CTL  0x57
#define CHG_DIG_CTL3 0x58
#define ADC_ANA_CTL0 0x60
#define ADC_DATA_VBAT 0x64
#define ADC_DATA_IBAT 0x65
#define ADC_DATA_ICHG 0x66
#define ADC_DATA_GP1  0x67
#define ADC_DATA_GP2  0x68
#define INTS_CTL      0x70
#define INT_FLAG0     0x71
#define INT_FLAG1     0x72
#define INT_MASK0     0x73
#define INT_MASK1     0x74
#define MFP_CTL0      0x75
#define MFP_CTL1      0x76
#define MFP_CTL2      0x77
#define GPIO_OE0     0x78
#define GPIO_OE1     0x79
#define GPIO_IE0     0x7A
#define GPIO_IE1     0x7B
#define GPIO_DATA0   0x7C
#define GPIO_DATA1   0x7D
#define PAD_PU0      0x7E
#define PAD_PU1      0x7F
#define PAD_PD0      0x80
#define PAD_PD1      0x81
#define PAD_CTL0     0x82
#define INT_PENDING0 0x83
#define INT_PENDING1 0x84
#define PAD_DRV0     0x85
#define PAD_DRV1     0x86
#define PAD_DRV2     0x86
#define ADDR_CTL     0x99

static inline int pm_set_flag_bits(uint8_t reg, uint8_t value, uint8_t mask) {
    uint8_t flag = 0;
    if (i2c1_read_reg(IP6303_ADDR, reg, &flag, 1)) {
        return 1;
    }
    // already enabled
    if ((flag & mask) == value) {
        return 0;
    }
    flag = (flag & ~mask) | value;
    if (i2c1_write_reg(IP6303_ADDR, reg, &flag, 1)) {
        return 1;
    }
    return 0;

}

static inline int pm_enable_flag_bits(uint8_t reg, uint8_t mask) {
    uint8_t flag = 0;
    if (i2c1_read_reg(IP6303_ADDR, reg, &flag, 1)) {
        return 1;
    }
    // already enabled
    if ((flag & mask) == mask) {
        return 0;
    }
    flag |= mask;
    if (i2c1_write_reg(IP6303_ADDR, reg, &flag, 1)) {
        return 1;
    }
    return 0;
}
static inline int pm_disable_flag_bits(uint8_t reg, uint8_t mask) {
    uint8_t flag = 0;
    if (i2c1_read_reg(IP6303_ADDR, reg, &flag, 1)) {
        return 1;
    }
    // already disabled
    if ((flag & ~mask) == flag ) {
        return 0;
    }
    flag &= ~mask;
    if (i2c1_write_reg(IP6303_ADDR, reg, &flag, 1)) {
        return 1;
    }
    return 0;
}

int pm_init(void) {
    i2c1_init();
    // maybe some config for IP6303

    // enable battery exist check
    pm_enable_flag_bits(0x4C, (1<<7));
    pm_enable_flag_bits(PSTATE_CTL3, (1<<0));

    // enable long press wakeup
    pm_enable_flag_bits(PSTATE_CTL0, (1<<5));

    // set ONOFF time, 2 second
    pm_set_flag_bits(PSTATE_SET, (0x01<<1), 0x06);

    /**
     * camera: LDO2
     * bluetooth: LDO3
     * motor: LDO5
     * NFC: DC2
     */
    // 2.8 V
    uint8_t flag = 0x54;
    i2c1_write_reg(IP6303_ADDR, LDO2_VSEL, &flag, 1);
    // 3.3 V
    flag = 0x68;
    i2c1_write_reg(IP6303_ADDR, LDO3_VSEL, &flag, 1);
    i2c1_write_reg(IP6303_ADDR, LDO5_VSEL, &flag, 1);

    // 3.3v
    flag = 0xd8;
    i2c1_write_reg(IP6303_ADDR, DC2_VSET, &flag, 1);


    flag = 0xD7; //110 10111  600mAh
    i2c1_write_reg(IP6303_ADDR, CHG_DIG_CTL0, &flag, 1);

    return 0;
}

int pm_power_off(void) {
    // POFF_EN = 1
    return pm_enable_flag_bits(PSTATE_CTL0, 1);
}

bool pm_battery_exist(void) {
    uint8_t flag;
    if (i2c1_read_reg(IP6303_ADDR, CHG_DIG_CTL2, &flag, 1)) {
        return false;
    }

    return flag & (1 << 3);
}

power_source_t pm_get_power_source(void) {
    uint8_t flag;
    if (i2c1_read_reg(IP6303_ADDR, LDO_GOOD, &flag, 1)) {
        return POWER_SOURCE_USB;
    }
    if (flag & 0x01) {
        return POWER_SOURCE_USB;
    }
    return POWER_SOURCE_BATTERY;
}

int pm_power_up(power_module_t module) {
    /**
     * camera: LDO2
     * bluetooth: LDO3
     * motor: LDO5
     * NFC: DCDC2
     */
    uint8_t reg = LDO_EN;
    uint8_t bit = 0;
    switch (module) {
        case POWER_MODULE_CAMERA:
            bit = 2;
            break;
        case POWER_MODULE_BLUETOOTH:
            bit = 3;
            break;
        case POWER_MODULE_MOTOR:
            bit = 5;
            break;
        case POWER_MODULE_NFC:
            reg = DC_CTL;
            bit = 1;
            break;
        default:
            return 1;
    }
    return pm_enable_flag_bits(reg, 1 << bit);
}

int pm_power_down(power_module_t source) {
    uint8_t bit = 0;
    uint8_t reg = LDO_EN;
    switch (source) {
        case POWER_MODULE_CAMERA:
            bit = 2;
            break;
        case POWER_MODULE_BLUETOOTH:
            bit = 3;
            break;
        case POWER_MODULE_MOTOR:
            bit = 5;
            break;
        case POWER_MODULE_NFC:
            reg = DC_CTL;
            bit = 1;
            break;
        default:
            return 1;
    }
    return pm_disable_flag_bits(reg, 1 << bit);
}

#include "button.h"
uint32_t pm_button_read(void) {
    static uint8_t last_state = 0xFF;
    uint8_t flag;
    if (i2c1_read_reg(IP6303_ADDR, INT_FLAG1, &flag, 1)){
        return 0;
    }
    uint8_t state;
    // 1: down 0: up
    state = (flag & (1<<5));
    if (state == last_state){
        return 0;
    }
    last_state = state;
    uint32_t evt = (state ? BTN_EVT_DOWN : BTN_EVT_UP) | BTN_POWER;
    return evt;
}

void pm_battery_long_press_reset(bool enable) {
    if (enable) {
        pm_enable_flag_bits(PSTATE_CTL1, 1);
        pm_enable_flag_bits(PSTATE_CTL2, (1<<5));
    } else {
        pm_disable_flag_bits(PSTATE_CTL1, 1);
        pm_disable_flag_bits(PSTATE_CTL2, (1<<5));
    }
}
