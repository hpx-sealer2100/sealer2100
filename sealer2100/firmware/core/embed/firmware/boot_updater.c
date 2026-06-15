#include "boot_updater.h"

#include "mpu.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include STM32_HAL_H

#include "common.h"
#include "display_defs.h"
#include "firmware_context.h"
#include "lv_conf.h"
#include "lv_port.h"
#include "lvgl.h"
#include "mipi_lcd.h"
#include "sdram.h"
#include "touch.h"

#include "ports/stm32/gccollect.h"
#include "py/gc.h"
#include "py/stackctrl.h"

#include "fs.h"
#include "image.h"

#include "power_manager.h"
#include "secure_heap.h"

#include "log.h"

#define MODULE               "updater"

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

#define COLOR_BLACK          0x1D1D1D
#define COLOR_WHITE          0xFFFFFF
#define COLOR_GREEN          0x00FE33
#define COLOR_GRARY          0x282828
#define COLOR_RED            0xFF4D4F

#define COLOR_LIGHT_GRAY     0xBFBFBF
#define COLOR_DARK_GREEN     0x0B3D15

#define ui_jump_to(view)              \
    do {                              \
        lv_obj_clean(ui_main_area()); \
        view;                         \
    } while (0)

/// updater
typedef const char* (*updater_get_name_fn_t)(void);
typedef const char* (*updater_get_cur_ver_fn_t)(void);
typedef const char* (*updater_get_new_ver_fn_t)(void);
typedef const char* (*updater_get_error_fn_t)(void);
typedef int (*updater_get_processed_fn_t)(void);
typedef void (*updater_canceled_fn_t)(void);
typedef void (*updater_verify_image_fn_t)(void);
typedef void (*updater_install_image_fn_t)(void);
typedef void (*updater_install_abort_fn_t)(void);

typedef enum {
    UPDATER_STATE_NONE,
    UPDATER_STATE_ERROR,
    UPDATER_STATE_NOTIFY_APP,
    UPDATER_STATE_WAIT_BUTTON_ACK,
    UPDATER_STATE_ASK_CONFIRM,
    UPDATER_STATE_WAITING_CONFIRM,
    UPDATER_STATE_CANCELLED,
    UPDATER_STATE_CONFIRMED,
    UPDATER_STATE_VERIFY,
    UPDATER_STATE_INSTALL,
    UPDATER_STATE_FINISHED,
} updater_state_t;

typedef struct {
    updater_get_name_fn_t get_name_fn;
    updater_get_cur_ver_fn_t get_cur_ver_fn;
    updater_get_new_ver_fn_t get_new_ver_fn;
    updater_get_error_fn_t get_error_fn;
    updater_get_processed_fn_t get_processed_fn;
    updater_verify_image_fn_t verify_image_fn;
    updater_install_image_fn_t install_image_fn;
    updater_install_abort_fn_t abort_fn;
    updater_canceled_fn_t cancelled_fn;
} updater_t;

typedef enum {
    UPDATER_ERROR_SUCCESS,
    UPDATER_ERROR_INVALID_PATH,
    UPDATER_ERROR_READ_FAILED,
    UPDATER_ERROR_UNKNOWN_IMAGE_TYPE,
    UPDATER_ERROR_INVALID_STATE,
    UPDATER_ERROR_INVALID_VERSION,
    UPDATER_ERROR_IMAGE_VERIFY_FAILED,
    UPDATER_ERROR_IMAGE_INSTALL_FAILED,
} updater_error_t;

typedef struct {
    char path[64];
    image_type_t image_type;
    char cur_ver[33];
    char new_ver[33];
    void* user_data; // the special updater ctx
    updater_error_t error;
    fs_file_t* fp;
    bool reboot_on_success;
} updating_context_t;

typedef struct {
    updater_state_t state;
    uint32_t wait_confirm_uintil;
    updater_t* updater;
    updating_context_t* updating;
} updater_context_t;

/// ui
typedef struct {
    lv_obj_t* op;
    lv_obj_t* bar;
} ui_updater_t;

struct ui_context_t {
    lv_obj_t* main;
    ui_updater_t* updater;
};

void ui_init();
lv_obj_t* ui_main_area(void);
void ui_updater(const char* dsc);
void ui_updater_reset(void);
void ui_updater_update(const char* op, int pct);
void ui_restart(const char* desc, uint32_t desc_color);

static updater_context_t updater_ctx;
static struct ui_context_t ui_ctx;

void updater_poll(void);

void updater_update_image(const char* path, bool reboot_on_success);
void updater_begin(updater_state_t state);
void updater_register(updater_t* updater);
void updater_failed(updater_error_t error);
// updater success and switch to next `state`
void updater_success(updater_state_t state);
void updater_finished(void);
const char* updater_error_format(updater_error_t error);

// updater fn
const char* updater_get_name(void);
const char* updater_get_cur_ver(void);
const char* updater_get_new_ver(void);
const char* updater_get_error(void);
int updater_get_processed(void);
void updater_canceled(void);

// updaters
void update_bootloader(void);


/// export functions
void boot_updater(const char* path) {
    // log have init

    // fatfs have init

    // lcd have init

    // lvgl memory use micropython memory manager
    mp_stack_set_top(&_estack);
    mp_stack_set_limit((char*)&_estack - (char*)&_sstack - 1024);
    gc_init(&_heap_start, &_heap_end);

    // lvgl init
    lv_init();
    lv_port_disp_init();
    lv_port_indev_init();
    lv_port_fs_init(&firmware_ctx.fs);
    lv_theme_default_init(
        NULL, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), 1, LV_FONT_DEFAULT
    );

    ui_init();
    // firmware confirg flash read only, we need enable write
    mpu_update_enable();

    updater_update_image(path, true);
    while (1) {
        updater_poll();
        lv_timer_handler();
        hal_delay(1);
    }
}

/// updater implement

void updater_poll(void) {
    updater_context_t* ctx = &updater_ctx;
    switch (ctx->state) {
    case UPDATER_STATE_NONE:
        // do nothing
        return;
    case UPDATER_STATE_NOTIFY_APP:
        // upgrading bootloader, not interactive with user
        // when upgrade bootloader, no need to notify app, just switch next state
        updater_success(UPDATER_STATE_WAIT_BUTTON_ACK);
        return;
    case UPDATER_STATE_WAIT_BUTTON_ACK:
        // upgrade bootloader, not interactive with user
        // when upgrade bootloader, no need wait button ack, just switch next state
        updater_success(UPDATER_STATE_ASK_CONFIRM);
        return;
    case UPDATER_STATE_ASK_CONFIRM:
        // upgrade bootloader, not interactive with user
        // when upgrade bootloader, no need wait confirm, just switch next state
        updater_success(UPDATER_STATE_WAITING_CONFIRM);
        return;
    case UPDATER_STATE_WAITING_CONFIRM:
        // upgrade bootloader, not interactive with user
        // when upgrade bootloader, no need wait confirm, just switch next state
        updater_success(UPDATER_STATE_CONFIRMED);
        return;
    case UPDATER_STATE_CONFIRMED:
        // update ui
        ui_updater(ctx->updater->get_name_fn());
        updater_success(UPDATER_STATE_VERIFY);
        return;
    case UPDATER_STATE_CANCELLED:
        // not interactive with user
        // ctx->updater->cancelled_fn();
        // ui_home();
        // updater_finished();
        return;
    case UPDATER_STATE_VERIFY:
        ctx->updater->verify_image_fn();
        // update ui
        ui_updater_update("Verifying", ctx->updater->get_processed_fn());
        return;
    case UPDATER_STATE_INSTALL:
        // begin install, disable long press power key restart device
        pm_battery_long_press_reset(false);
        ctx->updater->install_image_fn();
        // update ui
        ui_updater_update("Installing", ctx->updater->get_processed_fn());
        return;
    case UPDATER_STATE_ERROR:
        // update ui
        fs_remove(&firmware_ctx.fs, ctx->updating->path);
        ui_updater_reset();
        ui_jump_to(ui_restart(updater_error_format(ctx->updating->error), COLOR_RED));
        updater_finished();
        return;
    case UPDATER_STATE_FINISHED:
        {
            fs_remove(&firmware_ctx.fs, ctx->updating->path);
            ui_updater_reset();
            updater_finished();
            ui_jump_to(ui_restart("Update success", COLOR_GREEN));
            return;
        }
    default:
        // not reachable
        assert(false);
    }
}

static updater_t unknow_image_type_updater = {
    .get_error_fn = updater_get_error,
};

static updating_context_t* updating_context_create(void) {
    updating_context_t* ctx = pvPortMalloc(sizeof(updating_context_t));
    memset(ctx, 0, sizeof(updating_context_t));
    return ctx;
}

void updater_update_image(const char* path, bool reboot_on_success) {
    updater_context_t* ctx = &updater_ctx;
    ctx->updating = updating_context_create();
    strcpy(ctx->updating->path, path);
    ctx->updating->reboot_on_success = reboot_on_success;
    // use unknow image type updater for default
    // the special updater will overwrite it
    updater_register(&unknow_image_type_updater);

    if (!fs_is_file(&firmware_ctx.fs, path)) {
        updater_failed(UPDATER_ERROR_INVALID_PATH);
        return;
    }
    ctx->updating->fp = pvPortMalloc(sizeof(fs_file_t));
    int r = fs_file_open(&firmware_ctx.fs, ctx->updating->fp, path, FS_O_RDONLY);
    if (r < 0) {
        updater_failed(UPDATER_ERROR_INVALID_PATH);
        return;
    }
    // firmware vendor header
    uint8_t data[4 * 1024] = {0};
    r = fs_file_read(ctx->updating->fp, data, sizeof(data));
    if (r < sizeof(data)) {
        updater_failed(UPDATER_ERROR_READ_FAILED);
        return;
    }
    // reset cursor to start
    fs_file_seek(ctx->updating->fp, 0, FS_SEEK_SET);

    ctx->updating->image_type = image_get_type(data);

    switch (ctx->updating->image_type) {
    default:
    case IMAGE_TYPE_UNKNOWN:
        // only support bootloader, not support other image type
        LOG_DEBUG(MODULE, "unknown image type of :%s", path);
        updater_failed(UPDATER_ERROR_UNKNOWN_IMAGE_TYPE);
        return;
    case IMAGE_TYPE_BOOTLOADER:
        // use firmware update bootloader
        LOG_DEBUG(MODULE, "updating bootloader...");
        update_bootloader();
        break;
    }
    if (ctx->state == UPDATER_STATE_ERROR) {
        return;
    }

    LOG_DEBUG(MODULE, "cur_ver: %s, new_ver: %s", ctx->updating->cur_ver, ctx->updating->new_ver);
    bool version_match = false;
    image_version_t cur_ver = {0}, new_ver = {0};
    if (image_version_parse(ctx->updating->cur_ver, &cur_ver) != 0) {
        updater_failed(UPDATER_ERROR_INVALID_VERSION);
        return;
    }
    if (image_version_parse(ctx->updating->new_ver, &new_ver) != 0) {
        updater_failed(UPDATER_ERROR_INVALID_VERSION);
        return;
    }
    version_match = image_version_compare(&new_ver, &cur_ver) > 0;

    if (!version_match) {
        LOG_ERROR(MODULE, "Version not match");
        // for develop mode, skip
#if PRODUCTION
        updater_failed(UPDATER_ERROR_INVALID_VERSION);
        return;
#endif
    }
    updater_begin(UPDATER_STATE_NOTIFY_APP);
}

void updater_begin(updater_state_t state) {
    updater_success(state);
}

void updater_register(updater_t* updater) {
    updater_ctx.updater = updater;
}

void updater_failed(updater_error_t error) {
    updater_context_t* ctx = &updater_ctx;
    ctx->state = UPDATER_STATE_ERROR;
    ctx->updating->error = error;
}

void updater_success(updater_state_t state) {
    updater_context_t* ctx = &updater_ctx;
    ctx->state = state;
}

void updater_finished() {
    // clean `updater` resource
    updater_context_t* ctx = &updater_ctx;
    if (ctx->updater->abort_fn)
        ctx->updater->abort_fn();

    // clean updater resource
    if (ctx->updating->fp) {
        fs_file_close(ctx->updating->fp);
        vPortFree(ctx->updating->fp);
        ctx->updating->fp = NULL;
        ctx->updating = NULL;
    }
    vPortFree(ctx->updating);
    ctx->updating = NULL;
    ctx->state = UPDATER_STATE_NONE;
    // when finished enable long press power key restart device
    pm_battery_long_press_reset(true);
}

const char* updater_error_format(updater_error_t error) {
    switch (error) {
    case UPDATER_ERROR_SUCCESS:
        return "Success";
    case UPDATER_ERROR_INVALID_PATH:
        return "Invalid path";
    case UPDATER_ERROR_READ_FAILED:
        return "Image binary file read failed";
    case UPDATER_ERROR_UNKNOWN_IMAGE_TYPE:
        return "Unknow image type of file";
    case UPDATER_ERROR_INVALID_STATE:
        return "Invalid state";
    case UPDATER_ERROR_INVALID_VERSION:
        return "Invalid version";
    case UPDATER_ERROR_IMAGE_VERIFY_FAILED:
        return "Image binary file verify failed";
    case UPDATER_ERROR_IMAGE_INSTALL_FAILED:
        return "Image binary install failed";
    default:
        // not reachable
        assert(false);
    }
}

const char* updater_get_name(void) {
    return image_get_desc_name(updater_ctx.updating->image_type);
}

const char* updater_get_cur_ver(void) {
    return updater_ctx.updating->cur_ver;
}

const char* updater_get_new_ver(void) {
    return updater_ctx.updating->new_ver;
}

const char* updater_get_error(void) {
    updater_context_t* ctx = &updater_ctx;
    return updater_error_format(ctx->updating->error);
}

int updater_get_processed(void) {
    fs_file_t* fp = updater_ctx.updating->fp;

    uint32_t processed = fs_file_tell(fp);
    uint32_t totoal = fs_file_size(fp);
    return (processed * 100) / totoal;
}

void updater_canceled(void) {
    LOG_DEBUG(MODULE, "user cancelled");
}

/// ui
void ui_init(void) {
    lv_obj_t* obj = lv_obj_create(lv_scr_act());
    lv_obj_set_style_bg_color(obj, lv_color_hex(COLOR_BLACK), LV_PART_MAIN);
    lv_obj_set_size(obj, lv_pct(100), lv_pct(100));
    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(obj, CONTENT_PAD, LV_PART_MAIN);
    lv_obj_set_style_pad_top(obj, BAR_HEIGHT, LV_PART_MAIN);

    ui_ctx.main = obj;
}
lv_obj_t* ui_main_area(void) {
    assert(ui_ctx.main);
    return ui_ctx.main;
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

    ui_ctx.updater = pvPortMalloc(sizeof(ui_updater_t));
    memset(ui_ctx.updater, 0, sizeof(ui_updater_t));
    ui_ctx.updater->op = obj;

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
    ui_ctx.updater->bar = bar;
}

void ui_updater_reset(void) {
    if (ui_ctx.updater) {
        vPortFree(ui_ctx.updater);
        ui_ctx.updater = NULL;
    }
}

void ui_updater_update(const char* op, int pct) {
    ui_updater_t* updater = ui_ctx.updater;

    if (strcmp(op, lv_label_get_text(updater->bar)) != 0) {
        lv_label_set_text(updater->op, op);
    }
    lv_bar_set_value(updater->bar, pct, LV_ANIM_OFF);
}

static void restart_timer_cb(lv_timer_t* timer) {
    (void)timer;
    // restart device
    restart();
}

void ui_restart(const char* desc, uint32_t desc_color) {
    lv_obj_t* content = ui_main_area();
    // logo
    lv_obj_t* obj = lv_img_create(content);
    lv_img_set_src(obj, "A:/res/logo.png");
    lv_obj_center(obj);

    // description
    obj = lv_label_create(content);
    lv_label_set_text(obj, desc);
    lv_obj_set_style_text_font(obj, FONT_BOLD_LARGE, LV_PART_MAIN);
    lv_obj_set_style_text_color(obj, lv_color_hex(desc_color), LV_PART_MAIN);
    lv_obj_align(obj, LV_ALIGN_BOTTOM_MID, 0, -64);

    // `Restarting...`
    obj = lv_label_create(content);
    lv_label_set_text(obj, "Restarting...");
    lv_obj_align(obj, LV_ALIGN_BOTTOM_MID, 0, 0);

    // add a timer to restart device, 3s
    lv_timer_t* timer = lv_timer_create(restart_timer_cb, 3000, NULL);
    lv_timer_set_repeat_count(timer, 1);
}

/// updater_boot
#define CHUNK_SIZE (16 * 1024)

typedef struct {
    image_bootloader_t* boot; // cached image header, read from file

    SHA256_CTX sha256;

    uint32_t sector_index;      // sector index for install
    uint32_t sector_offset;     // the current sector offset
    uint32_t sector_free_space; // the current sector free space
} boot_updater_context_t;

static inline void failed(updater_error_t ucode, const char* msg) {
    updater_failed(ucode);
}

static void verify_async(void) {
    // vender header have verified
    // image header have verified

    updating_context_t* ctx = updater_ctx.updating;
    boot_updater_context_t* fctx = ctx->user_data;

    uint8_t chunk[CHUNK_SIZE] = {0};

    if (fs_file_tell(ctx->fp) == 0) {
        // setup hasher
        LOG_DEBUG(MODULE, "setup sha256 hasher for verify bootloader");
        sha256_Init(&fctx->sha256);
        sha256_Update(&fctx->sha256, fctx->boot->vector_table, sizeof(fctx->boot->vector_table));
        image_header_hash_update(&fctx->sha256, &fctx->boot->header);
        // seek to code offset
        fs_file_seek(ctx->fp, sizeof(image_bootloader_t), FS_SEEK_SET);
        return;
    }

    int r = fs_file_read(ctx->fp, chunk, CHUNK_SIZE);
    if (r < 0) {
        failed(UPDATER_ERROR_READ_FAILED, "Read firmware binary failed");
        return;
    }
    // hash update code
    sha256_Update(&fctx->sha256, chunk, r);

    if (fs_file_tell(ctx->fp) != fs_file_size(ctx->fp)) {
        return;
    }

    // have read all code
    uint8_t digest[SHA256_DIGEST_LENGTH];
    sha256_Final(&fctx->sha256, digest);
    LOG_HEXDUMP_DEBUG(MODULE, "", digest, sizeof(digest));

    image_print_header(&fctx->boot->header);

    // verify digest
    if (memcmp(digest, fctx->boot->header.digest, sizeof(digest)) != 0) {
        LOG_DEBUG(MODULE, "bootloader digest dismatch");
        LOG_HEXDUMP_DEBUG(MODULE, "digest in header", fctx->boot->header.digest, sizeof(digest));
        failed(UPDATER_ERROR_IMAGE_VERIFY_FAILED, "Verify Bootloader binary failed");
        return;
    }
    LOG_DEBUG(MODULE, "bootloader digest verify success");

    // verify signature
    if (image_header_verify(&fctx->boot->header) != sectrue) {
        LOG_DEBUG(MODULE, "bootloader signature verify failed");
        failed(UPDATER_ERROR_IMAGE_VERIFY_FAILED, "Verify Bootloader binary failed");
        return;
    }
    LOG_DEBUG(MODULE, "bootloader signature verify success");

    // seek to start
    fs_file_seek(ctx->fp, 0, FS_SEEK_SET);
    updater_success(UPDATER_STATE_INSTALL);
}

static void install_async(void) {
    updating_context_t* ctx = updater_ctx.updating;
    boot_updater_context_t* fctx = ctx->user_data;

    if (fs_file_tell(ctx->fp) == 0) {
        // setup sector info
        uint8_t sector = BOOTLOADER_SECTORS[fctx->sector_index];
        fctx->sector_free_space = flash_sector_size(sector);
        fctx->sector_offset = 0;
        if (sectrue != flash_erase(sector)) {
            failed(UPDATER_ERROR_IMAGE_INSTALL_FAILED, "Erase bootloader flash area failed");
            return;
        }
        hal_delay(1);
    }

    uint8_t chunk[CHUNK_SIZE] = {0};
    memset(chunk, 0, sizeof(chunk));
    int r = fs_file_read(ctx->fp, chunk, sizeof(chunk));
    if (r < 0) {
        failed(UPDATER_ERROR_IMAGE_INSTALL_FAILED, "Read bootloader binary file failed");
        return;
    }

    if (sectrue != flash_unlock_write()) {
        failed(UPDATER_ERROR_IMAGE_INSTALL_FAILED, "Unlock bootloader flash area failed");
        return;
    }

    uint8_t* p = chunk;
    uint8_t* end = p + r;
    while (p < end) {
        uint8_t sector = BOOTLOADER_SECTORS[fctx->sector_index];
        if (fctx->sector_free_space == 0) {
            fctx->sector_index++;
            if (fctx->sector_index >= BOOTLOADER_SECTORS_COUNT) {
                failed(UPDATER_ERROR_IMAGE_INSTALL_FAILED, "Bootloader binary not flash area");
                return;
            }
            // update sector
            sector = BOOTLOADER_SECTORS[fctx->sector_index];
            if (sectrue != flash_erase(sector)) {
                failed(UPDATER_ERROR_IMAGE_INSTALL_FAILED, "Erase bootloader flash area failed");
                return;
            }
            fctx->sector_free_space = flash_sector_size(sector);
            hal_delay(1);
            fctx->sector_offset = 0;
            // erase fnction do lock flsh, need unlock again
            if (sectrue != flash_unlock_write()) {
                failed(UPDATER_ERROR_IMAGE_INSTALL_FAILED, "Unlock bootloader flash area failed");
                return;
            }
        }
        if (sectrue != flash_write_words(sector, fctx->sector_offset, (uint32_t*)p)) {
            failed(UPDATER_ERROR_IMAGE_INSTALL_FAILED, "Bootloader binary write failed");
            return;
        }
        p += 32;
        fctx->sector_free_space -= 32;
        fctx->sector_offset += 32;
    }

    if (sectrue != flash_lock_write()) {
        failed(UPDATER_ERROR_IMAGE_INSTALL_FAILED, "Unlock bootloader flash area failed");
        return;
    }

    if (fs_file_tell(ctx->fp) != fs_file_size(ctx->fp)) {
        return;
    }

    // erase others
    int unused_sector_index = fctx->sector_index + 1;
    if (unused_sector_index < BOOTLOADER_SECTORS_COUNT) {
        if (sectrue !=
            flash_erase_sectors(
                &BOOTLOADER_SECTORS[unused_sector_index], BOOTLOADER_SECTORS_COUNT - unused_sector_index, NULL
            )) {
            LOG_DEBUG(MODULE, "Erase unused secotrs failed");
        }
    }

    updater_success(UPDATER_STATE_FINISHED);
}

static void __abort(void) {
    updating_context_t* ctx = updater_ctx.updating;
    boot_updater_context_t* fctx = ctx->user_data;
    if (!fctx) {
        return;
    }

    if (fctx->boot) {
        vPortFree(fctx->boot);
        fctx->boot = NULL;
    }

    vPortFree(fctx);
    ctx->user_data = NULL;
}

static updater_t updater = {
    .get_name_fn = updater_get_name,
    .get_cur_ver_fn = updater_get_cur_ver,
    .get_new_ver_fn = updater_get_new_ver,
    .get_error_fn = updater_get_error,
    .get_processed_fn = updater_get_processed,
    .verify_image_fn = verify_async,
    .install_image_fn = install_async,
    .abort_fn = __abort,
    .cancelled_fn = updater_canceled,
};

void update_bootloader(void) {
    updating_context_t* ctx = updater_ctx.updating;
    // step 0. setup current version
    const image_bootloader_t* const bl = BOOTLOADER;
    image_version_format(bl->header.version, ctx->cur_ver);

    // step 1. load header
    fs_file_seek(ctx->fp, 0, FS_SEEK_SET);
    boot_updater_context_t* fctx = pvPortMalloc(sizeof(boot_updater_context_t));
    memset(fctx, 0, sizeof(boot_updater_context_t));
    fctx->boot = pvPortMalloc(sizeof(image_bootloader_t));
    memset(fctx->boot, 0, sizeof(image_bootloader_t));
    ctx->user_data = fctx;

    int r = fs_file_read(ctx->fp, fctx->boot, sizeof(image_bootloader_t));
    if (r != sizeof(image_bootloader_t)) {
        LOG_DEBUG(MODULE, "Read bootloader filed");
        failed(UPDATER_ERROR_READ_FAILED, "Read bootloader failed");
        // have not register updater, free manually
        vPortFree(fctx->boot);
        vPortFree(fctx);
        ctx->user_data = NULL;
        return;
    }

    if (fctx->boot->header.magic != BOOTLOADER_IMAGE_MAGIC) {
        LOG_DEBUG(MODULE, "Invalid bootloader magic number");
        failed(UPDATER_ERROR_IMAGE_VERIFY_FAILED, "Invalid bootloader binary file");
        // have not register updater, free manually
        vPortFree(fctx->boot);
        vPortFree(fctx);
        ctx->user_data = NULL;
        return;
    }

    if (fctx->boot->header.code_size > BOOTLOADER_IMAGE_MAX_SIZE ||
        fctx->boot->header.code_size != fs_file_size(ctx->fp)) {
        LOG_DEBUG(MODULE, "Invalid bootloader code size");
        failed(UPDATER_ERROR_IMAGE_VERIFY_FAILED, "Invalid bootloader binary file");
        // have not register updater, free manually
        vPortFree(fctx->boot);
        vPortFree(fctx);
        ctx->user_data = NULL;
        return;
    }

    // seek to start
    fs_file_seek(ctx->fp, 0, FS_SEEK_SET);
    // step 2. setup new version
    image_version_format(fctx->boot->header.version, ctx->new_ver);

    // step 3. register updater
    updater_register(&updater);
}

static void power_off_btn_cb(lv_event_t* e) {
    (void)e;
    pm_power_off();
}
// provide `_fatal` for common
__attribute__((noreturn)) void __fatal(const char* lines[]) {
    {
        const char** __lines__ = lines;
        while (*__lines__) {
            LOG_DEBUG(MODULE, "%s", *__lines__);
            __lines__++;
        }
    }
    LOG_DEBUG(MODULE, "CFSR = 0x%08lx", SCB->CFSR);
    LOG_DEBUG(MODULE, "HFSR = 0x%08lx", SCB->HFSR);
    ui_init();
    lv_obj_t* content = ui_main_area();
    lv_obj_clean(content);

    lv_obj_add_flag(content, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(content, power_off_btn_cb, LV_EVENT_CLICKED, NULL);

    // title
    lv_obj_t* obj = lv_label_create(content);
    lv_label_set_text(obj, "System problem detected");
    lv_obj_set_width(obj, lv_pct(100));
    lv_obj_set_style_text_color(obj, lv_color_hex(COLOR_WHITE), LV_PART_MAIN);
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
        lv_obj_set_style_text_color(obj, lv_color_hex(COLOR_WHITE), LV_PART_MAIN);
        lines++;
    }

    obj = lv_label_create(content);
    lv_label_set_text(obj, "Tap to shutdown");
    lv_obj_set_style_text_color(obj, lv_color_hex(COLOR_WHITE), LV_PART_MAIN);
    lv_obj_set_style_text_font(obj, FONT_BOLD, LV_PART_MAIN);
    lv_obj_align(obj, LV_ALIGN_BOTTOM_MID, 0, 0);

    while (1) {
        lv_timer_handler();
        hal_delay(1);
    }
}
