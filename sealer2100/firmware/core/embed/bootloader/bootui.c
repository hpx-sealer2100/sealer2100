#include "bootui.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "battery.h"
#include "ble.h"
#include "boot_context.h"
#include "common.h"
#include "device.h"
#include "image.h"
#include "iris.h"
#include "log.h"
#include "power_manager.h"
#include "se.h"
#include "secure_heap.h"
#include "updater.h"
#include "version.h"
#include "memzero.h"


#include "lfs.h"

#define MODULE "UI"
#define ENABLE_UI_TEST 0
#define ENABLE_LOG_BATTERY_INFO 0

typedef struct {
    lv_obj_t* op;
    lv_obj_t* bar;
} ui_updater_t;

struct ui_context_t {
    lv_obj_t* main;
    lv_obj_t* fs_rw_bar;
    ui_updater_t* updater;
};

#define FONT_SMALL           (&lv_font_ping_fang_regular_20)
#define FONT_BOLD            (&lv_font_ping_fang_medium_32)
#define FONT_BOLD_SMALL      (&lv_font_ping_fang_medium_20)
#define FONT_BOLD_LARGE      (&lv_font_ping_fang_medium_44)

#define BAR_HEIGHT           56
#define RADIUS               12
#define CONTENT_PAD          24
#define CONTENT_SPACE        20
#define BOTTOM_BUTTON_HEIGHT 76
#define BOTTOM_BUTTON_RADIUS 12

#define LABLED_CONTAINER_PAD 20
#define LABLED_CONTAINER_GAP 16

#define COLOR_BLACK 0x1D1D1D
#define COLOR_WHITE 0xFFFFFF
#define COLOR_GREEN 0x00FE33
#define COLOR_GRARY 0x282828

#define COLOR_LIGHT_GRAY 0xBFBFBF
#define COLOR_DARK_GREEN 0x0B3D15

// battery info event, the param is uint32_t, the highest bit is charging flag, the lower 8 bits is soc
uint32_t UI_EVENT_BATTERY = 0;
// ble pair code
uint32_t UI_EVENT_BLE_PAIR = 0;
// ble state event, the param is uint32_t
uint32_t UI_EVENT_BLE_STATE = 0;
// ble name event, the param is char* name
uint32_t UI_EVENT_BLE_NAME = 0;
// ble version event, the param is char* version
uint32_t UI_EVENT_BLE_VERSION = 0;
// usb state event, the param is uint32_t [0, 1]
uint32_t UI_EVENT_USB_STATE = 0;
// reboot EVENT
uint32_t UI_EVENT_REBOOT = 0;
// power off event
uint32_t UI_EVENT_POWER_OFF = 0;
// BLE info refresh EVENT
uint32_t UI_EVENT_BLE_INFO_REFRESH = 0;
// iris version event, the param is char* version
uint32_t UI_EVENT_IRIS_VERSION = 0;
// iris info refresh event
uint32_t UI_EVENT_IRIS_INFO_REFRESH = 0;

/// helper functions
static void ui_context_init(void) {
    if (boot_ctx.ui) {
        return;
    }

    boot_ctx.ui = lv_mem_alloc(sizeof(ui_context_t));
    memset(boot_ctx.ui, 0, sizeof(ui_context_t));
}

static void battery_update(lv_obj_t* obj, bool charging, int soc) {
#if ENABLE_LOG_BATTERY_INFO
    // LOG_DEBUG(MODULE, "battery charging : %d", charging);
    // LOG_DEBUG(MODULE, "battery soc      : %d%%", soc);
#endif
    char* state = NULL;
    if (charging) {
        state = "charging";
    } else {
        state = "normal";
    }

    soc -= (soc % 5);
    if (soc == 0) {
        soc = 5;
    }
    char img[32] = {0};
    sprintf(img, "A:/res/battery-%d-%s.png", soc, state);
    lv_img_set_src(obj, img);
}

static void do_event_broadcast(lv_obj_t* obj, lv_event_code_t code, void* param) {
    // send first
    lv_event_send(obj, code, param);

    // broadcast event to child
    uint32_t count = lv_obj_get_child_cnt(obj);
    for (uint32_t i = 0; i < count; i++) {
        lv_obj_t* child = lv_obj_get_child(obj, i);
        if (!child)
            continue;
        do_event_broadcast(child, code, param);
    }
}

static lv_obj_t* ui_btn_create_ex(lv_obj_t* parent, char* text, uint32_t bg_color, uint32_t text_color) {
    lv_obj_t* btn = lv_btn_create(parent);
    lv_obj_set_style_radius(btn, BOTTOM_BUTTON_RADIUS, LV_PART_MAIN);
    lv_obj_set_style_bg_color(btn, lv_color_hex(bg_color), LV_PART_MAIN);
    lv_obj_t* label = lv_label_create(btn);
    lv_obj_center(label);
    lv_label_set_text(label, text);
    lv_obj_set_style_text_color(label, lv_color_hex(text_color), LV_PART_MAIN);
    return btn;
}

#define ui_btn_create(parent, text) ui_btn_create_ex(parent, text, COLOR_DARK_GREEN, COLOR_GREEN)
#define ui_primary_btn_create(parent, text) ui_btn_create(parent, text)
#define ui_secondary_btn_create(parent, text) ui_btn_create_ex(parent, text, COLOR_GRARY, COLOR_WHITE)

static lv_obj_t* ui_detail_labeld_create(lv_obj_t* parent, char* label, char* value) {
    lv_obj_t* container = lv_obj_create(parent);

    lv_obj_set_width(container, lv_pct(100));
    lv_obj_set_height(container, LV_SIZE_CONTENT);
    lv_obj_set_style_border_width(container, 0, LV_PART_MAIN);
    lv_obj_set_style_bg_color(container, lv_color_hex(COLOR_GRARY), LV_PART_MAIN);
    lv_obj_set_flex_flow(container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_radius(container, RADIUS, LV_PART_MAIN);
    lv_obj_set_style_pad_all(container, LABLED_CONTAINER_PAD, LV_PART_MAIN);
    lv_obj_set_style_pad_gap(container, LABLED_CONTAINER_GAP, LV_PART_MAIN);

    // label
    lv_obj_t* obj = lv_label_create(container);
    lv_obj_set_width(obj, lv_pct(100));
    lv_obj_set_style_text_font(obj, FONT_BOLD_SMALL, LV_PART_MAIN);
    lv_label_set_text(obj, label);

    // value
    obj = lv_label_create(container);
    lv_obj_set_width(obj, lv_pct(100));
    lv_obj_set_style_text_font(obj, FONT_SMALL, LV_PART_MAIN);
    lv_obj_set_style_text_color(obj, lv_color_hex(COLOR_LIGHT_GRAY), LV_PART_MAIN);
    lv_label_set_text(obj, value);

    // return the value obj
    return obj;
}

/// lv obj callbacks

static void idle_timer_cb(lv_timer_t* t) {
    if (boot_ctx.device.charging) {
        return;
    }

    if (hal_ticks_ms() - boot_ctx.last_alive_time_ms > IDLE_TIMEOUT_MS) {
        // idle timeout, power off
        LOG_DEBUG(MODULE, "idle timeout, power off");
        device_power_off();
    }
}

static void battery_timer_cb(lv_timer_t* t) {
    bool charging = pm_get_power_source() == POWER_SOURCE_USB;
    uint8_t soc = battery_read_SOC();

    void* param = UI_BATTERY_MAKE_PARAM(charging ? 1 : 0, soc);
    ui_event_broadcast(UI_EVENT_BATTERY, param);
}

static void reboot_timer_cb(lv_timer_t* t) {
    uint32_t type = UI_REBOOT_TYPE_GET(t->user_data);
    if (type == UI_REBOOT_TYPE_NORMAL) {
        restart();
    } else if (type == UI_REBOOT_TYPE_BOOTLOADER) {
        reboot_to_boot();
    } else if (type == UI_REBOOT_TYPE_BOARDLOADER) {
        reboot_to_board();
    } else {
        restart();
    }
}

static void reboot_event_cb(lv_event_t* e) {
    uint32_t type = UI_REBOOT_TYPE_GET(lv_event_get_param(e));

    lv_obj_t* content = ui_main_area();
    lv_obj_clean(content);
    lv_obj_t* obj = lv_label_create(content);
    lv_obj_center(obj);
    if (type == UI_REBOOT_TYPE_NORMAL) {
        lv_label_set_text(obj, "Restarting ...");
        LOG_DEBUG(MODULE, "Restarting ...");
    } else if (type == UI_REBOOT_TYPE_BOOTLOADER) {
        lv_label_set_text(obj, "Restarting to bootloader ...");
        LOG_DEBUG(MODULE, "Restarting to bootloader ...");
    } else if (type == UI_REBOOT_TYPE_BOARDLOADER) {
        lv_label_set_text(obj, "Restarting to boardloader ...");
        LOG_DEBUG(MODULE, "Restarting to boardloader ...");
    } else {
        lv_label_set_text(obj, "Restarting ...");
        LOG_DEBUG(MODULE, "Restarting ...");
    }

    lv_timer_t* timer = lv_timer_create(reboot_timer_cb, 500, UI_REBOOT_TYPE_MAKE_PARAM(type));
    lv_timer_set_repeat_count(timer, 1);
}


static void power_off_event_cb(lv_event_t* e) {
    (void)e;
    ui_jump_to(ui_power_off());
}

static void ble_info_refresh_timer_cb(lv_timer_t* timer) {
    bool done = strlen(boot_ctx.ble.name) && strlen(boot_ctx.ble.version);
    if (done) {
        // stop
        lv_timer_set_repeat_count(timer, 0);

        // del
        lv_timer_del(timer);
        return;
    }

    ble_async_refresh_dev_info();
}

static void ble_info_refresh_event_cb(lv_event_t* e) {
    // create a time for refresh ble info
    lv_timer_t* timer = lv_timer_create(ble_info_refresh_timer_cb, 1000, NULL);
    lv_timer_set_repeat_count(timer, -1);
}

static void iris_info_refresh_timer_cb(lv_timer_t* timer) {
    iris_poll_context_t* ctx = (void*)timer->user_data;
    bool done = strlen(boot_ctx.iris.version);
    if (done || iris_poll_context_is_error(ctx)) {
        // 模块响应成功，关闭模块
        if (done) {
            LOG_DEBUG(MODULE, "get iris version success, close iris");
        } else {
            const char* msg = iris_poll_context_get_error_msg(ctx);
            LOG_DEBUG(MODULE, "get iris version error, close iris, msg: %s", msg);
        }
        iris_close();

        // free resource
        vPortFree(ctx->resp_buf);
        vPortFree(ctx);

        // stop
        lv_timer_set_repeat_count(timer, 0);
        // del
        lv_timer_del(timer);
        return;
    }

    iris_async_response_poll(ctx);

    // have not done any operation, power on first
    if (ctx->operation == IRIS_MSG_ID_NONE) {
        iris_poll_context_reset_state(ctx);
        // 测试虹膜模块上电大约0.7s，等待响应
        iris_poll_context_set_timeout(ctx, 2000);
        // waiting for module status response
        ctx->operation = IRIS_MSG_ID_NOTE_MODULE_STATUS;
        // power on
        iris_open();
        LOG_DEBUG(MODULE, "iris open ...");
    }

    // if (ctx->operation == IRIS_MSG_ID_NOTE_MODULE_STATUS && ctx->state == IRIS_POLL_STATE_DONE) {
    //     iris_response_poll_context_reset_state(ctx);
    //     iris_response_poll_context_set_timeout(ctx, 5000);
    //     ctx->operation = IRIS_MSG_ID_WAIT;
    //     iris_async_send_reboot();
    //     LOG_DEBUG(MODULE, "iris reboot ...");
    // }

    // if (ctx->operation == IRIS_MSG_ID_WAIT && ctx->state == IRIS_POLL_STATE_DONE) {
    if (ctx->operation == IRIS_MSG_ID_NOTE_MODULE_STATUS && ctx->state == IRIS_POLL_STATE_DONE) {
        // 模块上电成功
        LOG_DEBUG(MODULE, "iris open success");
        if (ctx->resp.note.status == IRIS_MODULE_STATUS_BOUND) {
            LOG_DEBUG(MODULE, "iris module status: shared key bound");
            uint8_t key[32] = {0};
            if (!device_get_iris_pre_shared_key(key)) {
                LOG_ERROR(MODULE, "get pre shared key failed");
                ctx->state = IRIS_POLL_STATE_ERROR;
                // yield cpu
                return;
            }
            // 模块已经绑定密钥, 打开安全通道
            int ret = IRIS_ERROR_OK;
            do {
                hal_delay(10);
                ret = iris_sec_channel_open(key, 32);
            } while (ret == IRIS_HANDSHAKE_PENDING);
            memzero(key, sizeof(key));

            if (ret != IRIS_ERROR_OK) {
                LOG_ERROR(MODULE, "iris sec channel open failed, ret: %d", ret);
                ctx->state = IRIS_POLL_STATE_ERROR;
                // yield cpu
                return;
            }
            LOG_DEBUG(MODULE, "iris sec channel open success");
        }
        LOG_DEBUG(MODULE, "get version ...");
        iris_poll_context_reset_state(ctx);
        iris_poll_context_set_timeout(ctx, 1000);
        // waiting for version response
        ctx->operation = IRIS_MSG_ID_DEVICE_GET_VERSION;
        iris_async_get_version();
    }
    if (ctx->operation == IRIS_MSG_ID_DEVICE_GET_VERSION && ctx->state == IRIS_POLL_STATE_DONE) {
        // 版本号查询成功，更新版本号
        strcpy(boot_ctx.iris.version, (const char*)ctx->resp.reply.data);
        LOG_DEBUG(MODULE, "iris version: %s", boot_ctx.iris.version);
        ui_event_broadcast(UI_EVENT_IRIS_VERSION, boot_ctx.iris.version);

        // 发送 power down 命令
        iris_async_send_powerdown();
        // give power down time to effect
        lv_timer_set_period(timer, 1000);
        // iris_wait_power_down(true);
    }

    if (iris_poll_context_is_error(ctx)) {
        // 模块响应错误，发送 power down 命令
        iris_async_send_powerdown();
        // give power down time to effect
        lv_timer_set_period(timer, 1000);
    }
    // 发送完powerdown之后，至少要过一次 timer 才会进行`close`
}

static void iris_info_refresh_event_cb(lv_event_t* e) {
    // create a time for refresh iris info
    iris_poll_context_t* ctx = (void*)pvPortMalloc(sizeof(iris_poll_context_t));
    uint8_t* resp_buf = pvPortMalloc(64);
    iris_poll_context_init(ctx, 1000, resp_buf, 64);
    lv_timer_t* timer = lv_timer_create(iris_info_refresh_timer_cb, 100, ctx);
    lv_timer_set_repeat_count(timer, -1);
}

static void battery_low_power_event_cb(lv_event_t* e) {
    uint32_t* count = e->user_data;
    void* param = lv_event_get_param(e);
    bool charging = UI_BATTERY_PARM_GET_CHARGING(param);
    uint8_t soc = UI_BATTERY_PARM_GTT_SOC(param);

    if (charging) {
        if (*count > 0) {
            // switch to home screen
            ui_home();
            *count = 0;
        }
        return;
    }

    if (soc > 1) {
        return;
    }
    if (*count == 0) {
        ui_jump_to(ui_battery_low());
    }
    *count += 1;
    if (*count < 5) {
        return;
    }

    device_power_off();
}

static void status_bar_usb_state_event_cb(lv_event_t* e) {
    lv_obj_t* obj = lv_event_get_target(e);
    uint32_t state = UI_USB_STATE_GET(lv_event_get_param(e));
    if (state) {
        lv_obj_clear_flag(obj, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
    }
}

static void status_bar_ble_state_event_cb(lv_event_t* e) {
    lv_obj_t* obj = lv_event_get_target(e);
    uint32_t state = UI_BLE_STATE_GET(lv_event_get_param(e));
    if (state == UI_BLE_STATE_ENABLED) {
        lv_img_set_src(obj, "A:/res/ble-enabled.png");
        lv_obj_clear_flag(obj, LV_OBJ_FLAG_HIDDEN);
    } else if (state == UI_BLE_STATE_DISABLED) {
        lv_img_set_src(obj, "A:/res/ble-disabled.png");
        lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
    } else if (state == UI_BLE_STATE_CONNECTED) {
        lv_img_set_src(obj, "A:/res/ble-connected.png");
        lv_obj_clear_flag(obj, LV_OBJ_FLAG_HIDDEN);
    } else if (state == UI_BLE_STATE_DISCONNECTED) {
        lv_img_set_src(obj, "A:/res/ble-enabled.png");
        lv_obj_clear_flag(obj, LV_OBJ_FLAG_HIDDEN);
    }
}

static void status_bar_battery_event_cb(lv_event_t* e) {
    lv_obj_t* obj = lv_event_get_target(e);
    void* param = lv_event_get_param(e);
    bool charging = UI_BATTERY_PARM_GET_CHARGING(param);
    uint8_t soc = UI_BATTERY_PARM_GTT_SOC(param);
    if (boot_ctx.device.charging == charging && boot_ctx.device.soc == soc) {
        // no change
        return;
    }

    if (boot_ctx.device.charging != charging) {
        boot_alive_time_touch();
    }
#if ENABLE_LOG_BATTERY_INFO
    LOG_DEBUG(MODULE, "battery status changed");
#endif
    battery_update(obj, charging, soc);

    boot_ctx.device.charging = charging;
    boot_ctx.device.soc = soc;
}

static void bootloader_ble_name_event_cb(lv_event_t* e) {
    lv_obj_t* obj = lv_event_get_target(e);
    char* name = lv_event_get_param(e);
    lv_label_set_text(obj, name);
}

static void detail_ble_version_event_cb(lv_event_t* e) {
    lv_obj_t* obj = lv_event_get_target(e);
    char* version = lv_event_get_param(e);
    lv_label_set_text(obj, version);
}
static void detail_iris_version_event_cb(lv_event_t* e) {
    lv_obj_t* obj = lv_event_get_target(e);
    char* version = lv_event_get_param(e);
    lv_label_set_text(obj, version);
}

static void back_to_home_btn_cb(lv_event_t* e) {
    (void)e;
    ui_home();
}

static void restart_btn_cb(lv_event_t* e) {
    (void)e;
    LOG_DEBUG(MODULE, "user clicked restart, restarting ...");
    restart();
}

static void updater_cancel_btn_cb(lv_event_t* e) {
    (void)e;
    LOG_DEBUG(MODULE, "user cancel %s updating", boot_ctx.updater.updater->get_name_fn());
    updater_success(UPDATER_STATE_CANCELLED);
}

static void updater_install_btn_cb(lv_event_t* e) {
    (void)e;
    LOG_DEBUG(MODULE, "user confirm %s updating", boot_ctx.updater.updater->get_name_fn());
    updater_success(UPDATER_STATE_CONFIRMED);
}

static void ui_detail(void) {
    lv_obj_t* content = ui_main_area();

    lv_obj_t* container = lv_obj_create(content);

    lv_obj_add_flag(container, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_set_style_radius(container, RADIUS, LV_PART_MAIN);
    lv_obj_set_style_bg_color(container, lv_color_hex(COLOR_BLACK), LV_PART_MAIN);
    lv_obj_set_flex_flow(container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_border_width(container, 0, LV_PART_MAIN);
    lv_obj_set_size(container, lv_pct(100), 800-80-92);
    lv_obj_set_style_pad_all(container, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_gap(container, 8, LV_PART_MAIN);
    lv_obj_align(container, LV_ALIGN_TOP_LEFT, 0, 0);

    // mode
    ui_detail_labeld_create(container, "Model", "Selear2100");

    // firmware version
    char version[17] = {0};
    if (boot_ctx.firmware.valided) {
        const image_firmware_t* const fw = FIRMWARE;
        image_version_format(fw->header.version, version);
        ui_detail_labeld_create(container, "Firmware Version", version);
    } else {
        ui_detail_labeld_create(container, "Firmware Version", "None");
    }

    // bluetooth version
    if (strlen(boot_ctx.ble.version)) {
        ui_detail_labeld_create(container, "Bluetooth Version", boot_ctx.ble.version);
    } else {
        lv_obj_t* obj = ui_detail_labeld_create(container, "Bluetooth Version", "Pending");
        lv_obj_add_event_cb(obj, detail_ble_version_event_cb, UI_EVENT_BLE_VERSION, NULL);
    }

    // serial number
    char* dev_sn = NULL;
    device_get_serial(&dev_sn);
    if (dev_sn) {
        ui_detail_labeld_create(container, "Serial Number", dev_sn);
    } else {
        ui_detail_labeld_create(container, "Serial Number", "None");
    }

    // SE
    int ret = se_get_version(version);
    if (ret == 0) {
        ui_detail_labeld_create(container, "SE Version", version);
    } else {
        ui_detail_labeld_create(container, "SE Version", "None");
    }

    // bootloader version
    const image_bootloader_t* bl = BOOTLOADER;
    image_version_format(bl->header.version, version);
    ui_detail_labeld_create(container, "Bootloader Version", version);

    // iris version
    if (strlen(boot_ctx.iris.version)) {
        ui_detail_labeld_create(container, "Iris Version", boot_ctx.iris.version);
    } else {
        lv_obj_t* obj = ui_detail_labeld_create(container, "Iris Version", "Pending");
        lv_obj_add_event_cb(obj, detail_iris_version_event_cb, UI_EVENT_IRIS_VERSION, NULL);
    }

    // `back` button
    lv_obj_t* btn = ui_secondary_btn_create(content, "Back");
    lv_obj_set_size(btn, lv_pct(48), BOTTOM_BUTTON_HEIGHT);
    lv_obj_align(btn, LV_ALIGN_BOTTOM_LEFT, 0, 0);
    lv_obj_add_event_cb(btn, back_to_home_btn_cb, LV_EVENT_CLICKED, NULL);

    btn = ui_btn_create(content, "Restart");
    lv_obj_set_size(btn, lv_pct(48), BOTTOM_BUTTON_HEIGHT);
    lv_obj_align(btn, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
    lv_obj_add_event_cb(btn, restart_btn_cb, LV_EVENT_CLICKED, NULL);
}

static void ui_detail_btn_cb(lv_event_t* e) {
    (void)e;
    ui_jump_to(ui_detail());
}

static void test_timer_cb(lv_timer_t* timer) {
    (void)timer;
    static const char* pngs[] = {
        "A:/res/wallpapers/0.png",
        "A:/res/wallpapers/1.png",
        "A:/res/wallpapers/2.png",
        "A:/res/wallpapers/3.png",
        "A:/res/wallpapers/4.png",
        "A:/res/wallpapers/5.png",
    };
    static int idx = 0;
    const char* png = pngs[idx%6];
    LOG_DEBUG(MODULE, "show %s", png);
    static lv_obj_t* img = NULL;
    if (!img) {
        img = lv_img_create(lv_scr_act());
        lv_obj_set_size(img, lv_pct(100), lv_pct(100));
        lv_obj_set_pos(img, 0, 0);
    }
    lv_img_set_src(img, png);
    idx++;
}

static void ui_test(void) {
    // create a timer to refresh all png list in /res/test/
    lv_timer_t* timer = lv_timer_create(test_timer_cb, 1000, NULL);
    // timer->user_data = dir;
    lv_timer_set_repeat_count(timer , -1);
}


static void test_btn_cb(lv_event_t* e) {
    (void)e;
    ui_jump_to(ui_test());
}

void ui_event_register(void) {
    UI_EVENT_BATTERY = lv_event_register_id();
    UI_EVENT_BLE_PAIR = lv_event_register_id();
    UI_EVENT_BLE_STATE = lv_event_register_id();
    UI_EVENT_BLE_NAME = lv_event_register_id();
    UI_EVENT_BLE_VERSION = lv_event_register_id();
    UI_EVENT_USB_STATE = lv_event_register_id();
    UI_EVENT_REBOOT = lv_event_register_id();
    UI_EVENT_POWER_OFF = lv_event_register_id();
    UI_EVENT_BLE_INFO_REFRESH = lv_event_register_id();
    UI_EVENT_IRIS_VERSION = lv_event_register_id();
    UI_EVENT_IRIS_INFO_REFRESH = lv_event_register_id();
}

lv_obj_t* ui_main_area(void) {
    assert(boot_ctx.ui->main);
    return boot_ctx.ui->main;
}

void ui_init(void) {
    ui_context_init();
    lv_obj_t* obj = lv_obj_create(lv_scr_act());
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(obj, lv_color_hex(COLOR_BLACK), LV_PART_MAIN);
    lv_obj_set_size(obj, lv_pct(100), lv_pct(100));
    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(obj, CONTENT_PAD, LV_PART_MAIN);
    lv_obj_set_style_pad_top(obj, BAR_HEIGHT, LV_PART_MAIN);

    // add reboot event listener
    lv_obj_add_event_cb(lv_scr_act(), reboot_event_cb, UI_EVENT_REBOOT, NULL);
    // add power off event listener
    lv_obj_add_event_cb(lv_scr_act(), power_off_event_cb, UI_EVENT_POWER_OFF, NULL);
    // add ble refresh event listener
    lv_obj_add_event_cb(lv_scr_act(), ble_info_refresh_event_cb, UI_EVENT_BLE_INFO_REFRESH, NULL);
    // add iris refresh event listener
    lv_obj_add_event_cb(lv_scr_act(), iris_info_refresh_event_cb, UI_EVENT_IRIS_INFO_REFRESH, NULL);
    // add battery event listener, low power detect
    uint32_t *low_power_count = pvPortMalloc(sizeof(uint32_t));
    *low_power_count = 0;
    lv_obj_add_event_cb(lv_scr_act(), battery_low_power_event_cb, UI_EVENT_BATTERY, low_power_count);
    boot_ctx.ui->main = obj;

    // add idle timer
    lv_timer_t* timer = lv_timer_create(idle_timer_cb, 1000, NULL);
    lv_timer_set_repeat_count(timer, -1);

    // refresh ble info
    ui_event_broadcast(UI_EVENT_BLE_INFO_REFRESH, NULL);
    // refresh iris info
    ui_event_broadcast(UI_EVENT_IRIS_INFO_REFRESH, NULL);
}

void ui_status_bar_init(void) {
    ui_context_init();

    lv_obj_t* container = lv_obj_create(lv_layer_top());

    lv_obj_set_style_bg_color(container, lv_color_hex(COLOR_BLACK), LV_PART_MAIN);
    lv_obj_set_flex_flow(container, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(container, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_set_style_border_width(container, 0, LV_PART_MAIN);
    lv_obj_set_width(container, lv_pct(100));
    lv_obj_set_height(container, LV_SIZE_CONTENT);
    lv_obj_set_style_pad_all(container, 8, LV_PART_MAIN);
    lv_obj_set_style_pad_top(container, 4, LV_PART_MAIN);
    lv_obj_set_style_pad_gap(container, 8, LV_PART_MAIN);

    // a mode label
    lv_obj_t* obj = NULL;
    obj = lv_label_create(container);
    lv_obj_set_style_text_font(obj, FONT_SMALL, LV_PART_MAIN);
    lv_obj_set_flex_grow(obj, 1);
    if (boot_ctx.build != BUILD_MODE_PRODUCTION) {
        lv_label_set_text(obj, "bootloader");
    } else {
        lv_label_set_text(obj, "");
    }

    // usb state
    obj = lv_img_create(container);
    lv_img_set_src(obj, "A:/res/usb.png");
    lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_event_cb(obj, status_bar_usb_state_event_cb, UI_EVENT_USB_STATE, NULL);

    // ble status
    obj = lv_img_create(container);
    lv_img_set_src(obj, "A:/res/ble-enabled.png");
    lv_obj_add_event_cb(obj, status_bar_ble_state_event_cb, UI_EVENT_BLE_STATE, NULL);

    // battery
    obj = lv_img_create(container);
    lv_img_set_src(obj, "A:/res/battery-60-charging.png");
    lv_obj_add_event_cb(obj, status_bar_battery_event_cb, UI_EVENT_BATTERY, NULL);

    bool charging = pm_get_power_source() == POWER_SOURCE_USB;
    uint8_t soc = battery_read_SOC();
    battery_update(obj, charging, soc);
    boot_ctx.device.charging = charging;
    boot_ctx.device.soc = soc;

    lv_timer_t* timer = lv_timer_create(battery_timer_cb, 1000, NULL);
    lv_timer_set_repeat_count(timer, -1);
}

void ui_event_broadcast(lv_event_code_t code, void* param) {
    do_event_broadcast(lv_layer_top(), code, param);
    do_event_broadcast(lv_scr_act(), code, param);
}

void ui_boardloader(const char* version) {
  // this function use lvgl to show boardloader ui
  // it maybe can't show when resource not copied to fs
  // because lvgl load font from fatfs, fatfs is located in fs
  // so we can't show boardloader ui when fs is not mounted
  // It occured when production

  lv_obj_t *screen = lv_scr_act();
  lv_obj_set_style_pad_all(screen, CONTENT_PAD, LV_PART_MAIN);

  // board version
  lv_obj_t *label = lv_label_create(screen);
  lv_label_set_text_static(label, version);
  lv_obj_set_pos(label, 16, 16);

  // mass storage
  lv_obj_t *mass_storage_label = lv_label_create(screen);
  lv_label_set_text_static(mass_storage_label, "USB Mass Storage Mode");
  lv_obj_set_pos(mass_storage_label, 16, 70);

  // split
  lv_obj_t *split_label = lv_label_create(screen);
  lv_label_set_text_static(split_label, "======================");
  lv_obj_set_pos(split_label, 16, 112);

  // detail button
  lv_obj_t* btn = ui_btn_create(screen, "Restart");
  lv_obj_set_size(btn, lv_pct(100), BOTTOM_BUTTON_HEIGHT);
  lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 0, 0);
  lv_obj_add_event_cb(btn, restart_btn_cb, LV_EVENT_CLICKED, NULL);
}

void ui_bootloader(void) {
    lv_obj_t* content = ui_main_area();

    lv_obj_t* container = lv_obj_create(content);

    lv_obj_set_style_bg_color(container, lv_color_hex(COLOR_BLACK), LV_PART_MAIN);
    lv_obj_set_flex_flow(container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(container, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_set_style_border_width(container, 0, LV_PART_MAIN);
    lv_obj_set_size(container, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_pad_gap(container, CONTENT_SPACE, LV_PART_MAIN);
    lv_obj_align(container, LV_ALIGN_TOP_MID, 0, 180);

    // logo
    lv_obj_t* obj = lv_img_create(container);
    lv_img_set_src(obj, "A:/res/logo.png");

    // ble name
    obj = lv_label_create(container);
    lv_obj_set_style_text_font(obj, FONT_BOLD_LARGE, LV_PART_MAIN);
    if (strlen(boot_ctx.ble.name)) {
        lv_label_set_text(obj, boot_ctx.ble.name);
    } else {
        lv_label_set_text(obj, "");
        lv_obj_add_event_cb(obj, bootloader_ble_name_event_cb, UI_EVENT_BLE_NAME, NULL);
    }


    // label: `Update Mode`
    obj = lv_label_create(container);
    lv_label_set_text(obj, "Update Mode");
    lv_obj_set_style_text_font(obj, FONT_SMALL, LV_PART_MAIN);
    lv_obj_set_style_text_color(obj, lv_color_hex(COLOR_LIGHT_GRAY), LV_PART_MAIN);

    // detail button
    lv_obj_t* btn = ui_btn_create(content, "View Details");
    lv_obj_set_size(btn, lv_pct(100), BOTTOM_BUTTON_HEIGHT);
    lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_add_event_cb(btn, ui_detail_btn_cb, LV_EVENT_CLICKED, NULL);

    if (ENABLE_UI_TEST) {
        // test button
        btn = ui_secondary_btn_create(content, "Test");
        lv_obj_set_size(btn, lv_pct(100), BOTTOM_BUTTON_HEIGHT);
        lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 0, -96);
        lv_obj_add_event_cb(btn, test_btn_cb, LV_EVENT_CLICKED, NULL);
    }
}

void ui_ble_pairing(char* code) {

    // icon
    lv_obj_t* content = ui_main_area();
    lv_obj_t* icon = lv_img_create(content);

    lv_img_set_src(icon, "A:/res/bluetooth-pairing.png");
    lv_obj_align(icon, LV_ALIGN_TOP_MID, 0, 60);

    // label: `Pairing Code`
    lv_obj_t* label = lv_label_create(content);
    lv_label_set_text(label, "Bluetooth pairing");
    lv_obj_set_style_text_font(label, FONT_BOLD_LARGE, LV_PART_MAIN);
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 180);


    // code
    label = lv_label_create(content);
    lv_obj_set_style_text_font(label, FONT_BOLD_LARGE, LV_PART_MAIN);
    lv_label_set_text(label, code);
    lv_obj_center(label);

    // ok button
    lv_obj_t* btn = ui_secondary_btn_create(content, "OK");
    lv_obj_set_size(btn, lv_pct(100), BOTTOM_BUTTON_HEIGHT);
    lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_bg_color(btn, lv_color_hex(COLOR_GRARY), LV_PART_MAIN);
    // go back
    lv_obj_add_event_cb(btn, back_to_home_btn_cb, LV_EVENT_CLICKED, NULL);
}

void ui_ble_pair_failed(void) {
    // icon
    lv_obj_t* content = ui_main_area();
    lv_obj_t* icon = lv_img_create(content);
    lv_img_set_src(icon, "A:/res/hp/ic_cuowu555.png");
    lv_obj_align(icon, LV_ALIGN_TOP_MID, 0, 140);

    // code
    lv_obj_t* label = lv_label_create(content);
    lv_obj_set_style_text_font(label, FONT_BOLD_LARGE, LV_PART_MAIN);
    lv_label_set_text(label, "Bluetooth pair failed");
    lv_obj_align_to(label, icon, LV_ALIGN_OUT_BOTTOM_MID, 0, CONTENT_SPACE);

    // ok button
    lv_obj_t* btn = ui_secondary_btn_create(content, "OK");
    lv_obj_set_size(btn, lv_pct(100), BOTTOM_BUTTON_HEIGHT);
    lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 0, 0);
    // go back
    lv_obj_add_event_cb(btn, back_to_home_btn_cb, LV_EVENT_CLICKED, NULL);
}

static void power_off_btn_cb(lv_event_t* e) {
    (void)e;
    device_power_off();
}

void ui_power_off(void) {
    lv_obj_t* content = ui_main_area();
    // icon
    lv_obj_t* icon = lv_img_create(content);
    lv_img_set_src(icon, "A:/res/hp/ic_guanji2.png");
    lv_obj_align(icon, LV_ALIGN_TOP_MID, 0, 60);

    // show prompt "Do you want to power off?"
    lv_obj_t* obj = lv_label_create(content);
    lv_obj_set_width(obj, lv_pct(100));
    lv_obj_set_style_text_font(obj, FONT_BOLD_LARGE, LV_PART_MAIN);
    lv_obj_align(obj, LV_ALIGN_TOP_MID, 0, 180);
    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_label_set_text(obj, "Do you want to power off?");

    lv_obj_t* btn = ui_secondary_btn_create(content, "Cancel");
    lv_obj_set_size(btn, lv_pct(48), BOTTOM_BUTTON_HEIGHT);
    lv_obj_align(btn, LV_ALIGN_BOTTOM_LEFT, 0, 0);
    lv_obj_add_event_cb(btn, back_to_home_btn_cb, LV_EVENT_CLICKED, NULL);

    btn = ui_btn_create(content, "Power Off");
    lv_obj_set_size(btn, lv_pct(48), BOTTOM_BUTTON_HEIGHT);
    lv_obj_align(btn, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
    lv_obj_add_event_cb(btn, power_off_btn_cb, LV_EVENT_CLICKED, NULL);
}

void ui_battery_low(void) {
    lv_obj_t* content = ui_main_area();

    lv_obj_t* img = lv_img_create(content);
    lv_img_set_src(img, "A:/res/battery-low.png");
    lv_obj_center(img);

    lv_obj_t* label = lv_label_create(content);
    lv_obj_set_width(label, lv_pct(100));
    lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_label_set_text(label,"Low battery. Shutting down soon.\n Please charge.");

    lv_obj_align_to(label, content, LV_ALIGN_BOTTOM_MID, 0, 0);
}


void ui_fs_file_op_reset(void) {
    lv_obj_clean(ui_main_area());
    boot_ctx.ui->fs_rw_bar = NULL;
}
void ui_fs_file_op(char* txt) {
    if (boot_ctx.ui->fs_rw_bar) {
        return;
    }

    lv_obj_t* content = ui_main_area();
    lv_obj_clean(content);

    // icon
    lv_obj_t* icon = lv_img_create(content);
    lv_img_set_src(icon, "A:/res/logo.png");
    lv_obj_center(icon);

    // label
    lv_obj_t* obj = lv_label_create(content);
    lv_obj_set_style_text_font(obj, FONT_SMALL, LV_PART_MAIN);
    lv_obj_set_style_text_color(obj, lv_color_hex(COLOR_LIGHT_GRAY), LV_PART_MAIN);
    lv_obj_align(obj, LV_ALIGN_TOP_MID, 0, 630);
    lv_label_set_text(obj, txt);

    // bar
    lv_obj_t* bar = lv_bar_create(content);
    lv_bar_set_mode(bar, LV_BAR_MODE_NORMAL);
    lv_obj_set_size(bar, 240, 6);
    lv_bar_set_range(bar, 0, 100);
    lv_obj_set_style_pad_all(bar, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(bar, LV_RADIUS_CIRCLE, LV_PART_MAIN);

    lv_obj_set_style_bg_color(bar, lv_color_hex(0), LV_PART_MAIN);
    lv_obj_set_style_bg_color(bar, lv_color_hex(COLOR_GREEN), LV_PART_INDICATOR);
    lv_bar_set_value(bar, 0, LV_ANIM_OFF);
    lv_obj_align(bar, LV_ALIGN_TOP_MID, 0, 678);

    boot_ctx.ui->fs_rw_bar = bar;
}

void ui_fs_file_op_update(int pct) {
    lv_obj_t* bar = boot_ctx.ui->fs_rw_bar;
    lv_bar_set_value(bar, pct, LV_ANIM_OFF);
}

void ui_updater_confirm(const char* dsc) {
    updater_context_t* ctx = &boot_ctx.updater;
    lv_obj_t* content = ui_main_area();

    // icon
    lv_obj_t* icon = lv_img_create(content);
    lv_img_set_src(icon, "A:/res/update.png");
    lv_obj_align(icon, LV_ALIGN_TOP_MID, 0, 60);

    // title: xxxx Update
    char txt[128] = {0};
    sprintf(txt, "%s Update", dsc);
    lv_obj_t* obj = lv_label_create(content);

    lv_obj_set_width(obj, lv_pct(100));
    lv_obj_set_style_text_font(obj, FONT_BOLD_LARGE, LV_PART_MAIN);
    lv_obj_align(obj, LV_ALIGN_TOP_MID, 0, 180);
    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_label_set_text(obj, txt);

    lv_obj_t* msg = lv_label_create(content);
    // label: A new xxx available！
    sprintf(txt, "A new %s available！", dsc);
    lv_label_set_text(msg, txt);
    lv_obj_set_style_text_color(msg, lv_color_hex(COLOR_LIGHT_GRAY), LV_PART_MAIN);
    lv_obj_set_style_text_font(msg, FONT_SMALL, LV_PART_MAIN);
    lv_obj_align_to(msg, obj, LV_ALIGN_OUT_BOTTOM_MID, 0, 16);

    lv_obj_t* container = lv_obj_create(content);

    lv_obj_set_style_bg_color(container, lv_color_hex(COLOR_BLACK), LV_PART_MAIN);
    lv_obj_set_flex_flow(container, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(container, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_set_style_border_width(container, 0, LV_PART_MAIN);
    lv_obj_set_size(container, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_pad_gap(container, CONTENT_SPACE, LV_PART_MAIN);
    lv_obj_center(container);

    // current version -> new version
    obj = lv_label_create(container);
    lv_label_set_text(obj, ctx->updater->get_cur_ver_fn());

    obj = lv_img_create(container);
    lv_img_set_src(obj, "A:/res/arrow-right_2.png");

    obj = lv_label_create(container);
    lv_label_set_text(obj, ctx->updater->get_new_ver_fn());
    lv_obj_set_style_text_font(obj, FONT_BOLD, LV_PART_MAIN);

    lv_obj_t* btn = ui_secondary_btn_create(content, "Cancel");
    lv_obj_set_size(btn, lv_pct(48), BOTTOM_BUTTON_HEIGHT);
    lv_obj_align(btn, LV_ALIGN_BOTTOM_LEFT, 0, 0);
    lv_obj_add_event_cb(btn, updater_cancel_btn_cb, LV_EVENT_CLICKED, NULL);

    btn = ui_btn_create(content, "Install");
    lv_obj_set_size(btn, lv_pct(48), BOTTOM_BUTTON_HEIGHT);
    lv_obj_align(btn, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
    lv_obj_add_event_cb(btn, updater_install_btn_cb, LV_EVENT_CLICKED, NULL);
}

void ui_updater(const char* dsc) {
    lv_obj_t* content = ui_main_area();

    // icon
    lv_obj_t* icon = lv_img_create(content);
    lv_img_set_src(icon, "A:/res/update.png");
    lv_obj_align(icon, LV_ALIGN_TOP_MID, 0, 60);

    // title: xxxx Update
    char txt[128] = {0};
    sprintf(txt, "%s Update", dsc);
    lv_obj_t* obj = lv_label_create(content);

    lv_obj_set_width(obj, lv_pct(100));
    lv_obj_set_style_text_font(obj, FONT_BOLD_LARGE, LV_PART_MAIN);
    lv_obj_align(obj, LV_ALIGN_TOP_MID, 0, 180);
    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_label_set_text(obj, txt);

    lv_obj_t* msg = lv_label_create(content);
    // label: Please DO NOT power off.
    lv_label_set_text(msg, "Please DO NOT power off.");
    lv_obj_set_style_text_color(msg, lv_color_hex(COLOR_LIGHT_GRAY), LV_PART_MAIN);
    lv_obj_set_style_text_font(msg, FONT_SMALL, LV_PART_MAIN);
    lv_obj_align_to(msg, obj, LV_ALIGN_OUT_BOTTOM_MID, 0, 16);

    // operation: `verifying` or `installing`
    obj = lv_label_create(content);
    lv_obj_set_style_text_font(obj, FONT_SMALL, LV_PART_MAIN);
    lv_obj_set_style_text_color(obj, lv_color_hex(COLOR_LIGHT_GRAY), LV_PART_MAIN);
    lv_obj_align(obj, LV_ALIGN_TOP_MID, 0, 630);
    lv_label_set_text(obj, "");

    boot_ctx.ui->updater = pvPortMalloc(sizeof(ui_updater_t));
    memset(boot_ctx.ui->updater, 0, sizeof(ui_updater_t));
    boot_ctx.ui->updater->op = obj;

    // bar
    lv_obj_t* bar = lv_bar_create(content);
    lv_bar_set_mode(bar, LV_BAR_MODE_NORMAL);
    lv_obj_set_size(bar, 240, 6);
    lv_bar_set_range(bar, 0, 100);
    lv_obj_set_style_pad_all(bar, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(bar, LV_RADIUS_CIRCLE, LV_PART_MAIN);

    lv_obj_set_style_bg_color(bar, lv_color_hex(0), LV_PART_MAIN);
    lv_obj_set_style_bg_color(bar, lv_color_hex(COLOR_GREEN), LV_PART_INDICATOR);
    lv_bar_set_value(bar, 0, LV_ANIM_OFF);
    lv_obj_align(bar, LV_ALIGN_TOP_MID, 0, 678);
    boot_ctx.ui->updater->bar = bar;
}

void ui_updater_reset(void) {
    if (boot_ctx.ui->updater) {
        vPortFree(boot_ctx.ui->updater);
        boot_ctx.ui->updater = NULL;
    }
}

void ui_updater_update(const char* op, int pct) {
    ui_updater_t* updater = boot_ctx.ui->updater;

    if (strcmp(op, lv_label_get_text(updater->bar)) != 0) {
        lv_label_set_text(updater->op, op);
    }

    lv_bar_set_value(updater->bar, pct, LV_ANIM_OFF);
}


// provide `_fatal` for common
__attribute__((noreturn))
void __fatal(const char* lines[]) {
    lv_obj_t* content = ui_main_area();
    lv_obj_clean(content);

    lv_obj_add_flag(content, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(content, power_off_btn_cb, LV_EVENT_CLICKED, NULL);

    // title
    lv_obj_t* obj = lv_label_create(content);
    lv_label_set_text(obj, "System problem detected");
    lv_obj_set_width(obj, lv_pct(100));
    lv_obj_set_style_text_font(obj, FONT_BOLD_LARGE, LV_PART_MAIN);
    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);

    lv_obj_t* container = lv_obj_create(content);

    lv_obj_add_flag(container, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_set_style_radius(container, RADIUS, LV_PART_MAIN);
    lv_obj_set_style_bg_color(container, lv_color_hex(COLOR_BLACK), LV_PART_MAIN);
    lv_obj_set_flex_flow(container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_border_width(container, 0, LV_PART_MAIN);
    lv_obj_set_size(container, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_set_style_pad_all(container, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_gap(container, 8, LV_PART_MAIN);
    lv_obj_align(container, LV_ALIGN_TOP_LEFT, 0, 120);

    while (*lines) {
        obj = lv_label_create(container);
        lv_obj_set_width(obj, lv_pct(100));
        lv_label_set_text(obj, *lines);
        lines++;
    }

    obj = lv_label_create(content);
    lv_label_set_text(obj, "Tap to shutdown");
    lv_obj_set_style_text_font(obj, FONT_BOLD, LV_PART_MAIN);
    lv_obj_align(obj, LV_ALIGN_BOTTOM_MID, 0, 0);

    while (1) {
        lv_timer_handler();
        hal_delay(1);
    }
}
