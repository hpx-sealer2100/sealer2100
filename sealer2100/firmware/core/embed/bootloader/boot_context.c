#include "boot_context.h"

#include <stdbool.h>
#include <string.h>

#include "common.h"
#include "log.h"

boot_context_t boot_ctx;

void boot_context_init(void) {
    memset(&boot_ctx, 0, sizeof(boot_context_t));
#if PRODUCTION
    boot_ctx.build = BUILD_MODE_PRODUCTION;
#else
    boot_ctx.build = BUILD_MODE_DEV;
#endif
}

void boot_alive_time_touch(void) {
    boot_ctx.last_alive_time_ms = hal_ticks_ms();
}

void boot_fs_msg_monitor_init(fs_msg_timeout_fn_t timeout_fn, uint32_t timeout_ms) {
    boot_ctx.monitor.fs_msg_timeout_fn = timeout_fn;
    boot_ctx.monitor.timeout_ms = timeout_ms;
    boot_ctx.device.busy = true;
    boot_fs_msg_monitor_touch();
}
void boot_fs_msg_monitor_reset(void) {
    memset(&boot_ctx.monitor, 0, sizeof(boot_ctx.monitor));
    boot_ctx.device.busy = false;
}
void boot_fs_msg_monitor_touch(void) {
    boot_ctx.monitor.last_time_ms = hal_ticks_ms();
}

// poll for fs message timeout
void boot_fs_msg_monitor_poll(void) {
    // if not set timeout_fn, do nothing
    if (boot_ctx.monitor.fs_msg_timeout_fn == NULL) {
        return;
    }

    // check is timeout
    if (hal_ticks_ms() - boot_ctx.monitor.last_time_ms > boot_ctx.monitor.timeout_ms) {
        LOG_ERROR("boot context", "fs message timeout");
        // should reset?, or reset by caller?
        boot_ctx.monitor.fs_msg_timeout_fn();
    }
}
