#include "boot_power_key.h"

#include <memory.h>

#include "boot_context.h"

#include "bootui.h"
#include "button.h"
#include "common.h"
#include "device.h"
#include "log.h"
#include "power_manager.h"

static struct {
    uint32_t last_poll_time;
    uint32_t last_press_time;
    bool pressed;
    bool have_notified;
} power_key_ctx;

void boot_power_key_init(void) {
    memset(&power_key_ctx, 0, sizeof(power_key_ctx));
}

void boot_power_key_poll(void) {
    if (hal_ticks_ms() - power_key_ctx.last_poll_time < 30) {
        return;
    }
    power_key_ctx.last_poll_time = hal_ticks_ms();

    // test device is busy
    if (boot_ctx.device.busy) {
        return;
    }

    // only poll power key when power source is not USB
    if (pm_get_power_source() == POWER_SOURCE_USB) {
        return;
    }

    uint32_t evt = pm_button_read();
    if (evt == 0 && power_key_ctx.pressed) {
        // 2 seconds pressed
        if (hal_ticks_ms() - power_key_ctx.last_press_time >= 2000 && !power_key_ctx.have_notified) {
            // notify ui power off
            LOG_DEBUG("power key monitor", "Power key pressed for 2 seconds, notify power off event");
            power_key_ctx.have_notified = true;
            ui_event_broadcast(UI_EVENT_POWER_OFF, NULL);
        }
    } else if (evt & BTN_EVT_DOWN) {
        if (!power_key_ctx.pressed) {
            power_key_ctx.pressed = true;
            power_key_ctx.have_notified = false;
            power_key_ctx.last_press_time = hal_ticks_ms();
            LOG_DEBUG("power key monitor", "Power key pressed, start monitor");
        }
    } else if (evt & BTN_EVT_UP) {
        power_key_ctx.pressed = false;
        power_key_ctx.have_notified = false;
        power_key_ctx.last_press_time = 0;
    }
}
