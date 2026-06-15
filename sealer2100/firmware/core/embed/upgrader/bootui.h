#ifndef __BOOT_UI_H__
#define __BOOT_UI_H__
#include <stdbool.h>
#include <stdint.h>

#include "lvgl.h"

typedef enum {
    UI_UPDATER_STATE_NONE,
    UI_UPDATER_STATE_VERIFY_FAILED,
    UI_UPDATER_STATE_INSTALL_FAILED,
    UI_UPDATER_STATE_DONE,
} ui_upgrader_state;


#define ui_jump_to(view)              \
    do                                \
    {                                 \
        lv_obj_clean(ui_main_area()); \
        view;                         \
    }                                 \
    while ( 0 )

#define ui_home() ui_jump_to(ui_upgrader(UI_UPDATER_STATE_NONE))


#define UI_BATTERY_MAKE_PARAM(charging, soc) (void*)((uint32_t)(((charging)<<31) | soc))
#define UI_BATTERY_PARM_GET_CHARGING(param)  (((uint32_t)param)>> 31)
#define UI_BATTERY_PARM_GTT_SOC(param)       (((uint32_t)param) & 0xff)

#define UI_BLE_STATE_ENABLED                0x01
#define UI_BLE_STATE_DISABLED               0x02
#define UI_BLE_STATE_CONNECTED              0x03
#define UI_BLE_STATE_DISCONNECTED           0x04

#define UI_BLE_STATE_MAKE_PARAM(state)      (void*)((uint32_t)state)
#define UI_BLE_STATE_GET(param)             ((uint32_t)param)

#define UI_USB_STATE_DISCONNECTED           0x00
#define UI_USB_STATE_CONNECTED              0x01

#define UI_USB_STATE_MAKE_PARAM(state)      (void*)((uint32_t)state)
#define UI_USB_STATE_GET(param)             ((uint32_t)param)

#define UI_REBOOT_TYPE_NORMAL               0x01
#define UI_REBOOT_TYPE_BOARDLOADER          0x02
#define UI_REBOOT_TYPE_BOOTLOADER           0x03
#define UI_REBOOT_TYPE_MAKE_PARAM(state)         (void*)((uint32_t)state)
#define UI_REBOOT_TYPE_GET(param)           ((uint32_t)param)

typedef struct ui_context_t ui_context_t;

extern uint32_t UI_EVENT_BATTERY;
extern uint32_t UI_EVENT_BLE_PAIR;
extern uint32_t UI_EVENT_BLE_STATE;
extern uint32_t UI_EVENT_BLE_NAME;
extern uint32_t UI_EVENT_BLE_VERSION;
extern uint32_t UI_EVENT_USB_STATE;
extern uint32_t UI_EVENT_REBOOT;
extern uint32_t UI_EVENT_POWER_OFF;
extern uint32_t UI_EVENT_BLE_INFO_REFRESH;
extern uint32_t UI_EVENT_IRIS_VERSION;
extern uint32_t UI_EVENT_IRIS_INFO_REFRESH;

void ui_event_register(void);
void ui_init(void);
void ui_status_bar_init(void);

void ui_event_broadcast(lv_event_code_t code, void* param);

lv_obj_t* ui_main_area(void);

// views
void ui_operation_init(const char* binary, const char* op);
void ui_upgrader(ui_upgrader_state state);
void ui_power_off(void);
void ui_battery_low(void);
void ui_operation_update(int pct);

void ui_updater_confirm(const char *dsc);
void ui_updater(const char* dsc);
void ui_updater_update(const char* op, int pct);
void ui_updater_reset(void);

#endif
