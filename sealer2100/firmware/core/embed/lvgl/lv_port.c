#include "lv_port.h"

#include <stdbool.h>
#include <stdint.h>

#include "display_defs.h"
#include "fs.h"
#include "log.h"
#include "lv_conf.h"
#include "lvgl.h"
#include "mipi_lcd.h"
#include "sdram.h"
#include "touch.h"

#define MODULE "lv port"

volatile bool directly_flush_to_ltdc_buffer = false;

void lvgl_set_direct_copy(bool enable) {
    directly_flush_to_ltdc_buffer = enable;
}

static void disp_flush(lv_disp_drv_t* disp_drv, const lv_area_t* area, lv_color_t* color_p) {
    extern bool ltdc_buffer_idle(void);
    extern void* ltdc_buffer_rendered(void);
    extern void* ltdc_buffer_rendering(void);
    extern void ltdc_buffer_switch(void);
    uint16_t x, y, w, h;
    x = area->x1; y = area->y1;
    w = area->x2 - area->x1 + 1;
    h = area->y2 - area->y1 + 1;
    if (directly_flush_to_ltdc_buffer) {
        dma2d_copy_buffer(color_p, ltdc_buffer_rendered(), x, y, w, h);
    } else {
        while (!ltdc_buffer_idle());
        dma2d_copy_buffer(color_p, ltdc_buffer_rendering(), x, y, w, h);
        ltdc_buffer_switch();
    }
    lv_disp_flush_ready(disp_drv);
}

static void ts_read(lv_indev_drv_t* indev_drv, lv_indev_data_t* data) {
    static lv_coord_t last_x = 0;
    static lv_coord_t last_y = 0;
    static lv_indev_state_t last_sate = LV_INDEV_STATE_REL;

    int pos;
    pos = touch_read();
    /*Save the pressed coordinates and the state*/
    if (pos & TOUCH_START) {
        last_x = (pos >> 12) & 0xfff;
        last_y = pos & 0xfff;
        last_sate = LV_INDEV_STATE_PR;
    } else if (pos & TOUCH_END) {
        last_x = (pos >> 12) & 0xfff;
        last_y = pos & 0xfff;
        last_sate = LV_INDEV_STATE_REL;
    } else if (pos & TOUCH_MOVE) {
        last_x = (pos >> 12) & 0xfff;
        last_y = pos & 0xfff;
        last_sate = LV_INDEV_STATE_PR;
    }

    /*Set the last pressed coordinates*/
    data->state = last_sate;
    data->point.x = last_x;
    data->point.y = last_y;
    data->continue_reading = false;
}

void lv_port_disp_init(void) {
    static lv_disp_draw_buf_t disp_buf;
    #define LVGL_BUF_SIZE (SDRAM_LVGL_SIZE/2)
    lv_color_t* fb1 = (void*)SDRAM_LVGL_ADDRESS;
    lv_color_t* fb2 = (void*)(SDRAM_LVGL_ADDRESS + LVGL_BUF_SIZE);

    lv_disp_draw_buf_init(&disp_buf, fb1, fb2, LVGL_BUF_SIZE/sizeof(lv_color_t));

    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.draw_buf = &disp_buf;
    disp_drv.flush_cb = disp_flush;
    disp_drv.hor_res = DISPLAY_RESX;
    disp_drv.ver_res = DISPLAY_RESY;
    lv_disp_drv_register(&disp_drv);
}

void lv_port_indev_init(void) {
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = ts_read;
    indev_drv.long_press_time = 2000;
    lv_indev_drv_register(&indev_drv);
}

// lvgl file system operations
#define FS_FROM_DRV(drv) ((fs_t*)drv->user_data)

static bool lv_fs_ready_cb(lv_fs_drv_t* drv) {
    (void)drv;
    return true;
}

static void* lv_fs_open_cb(lv_fs_drv_t* drv, const char* path, lv_fs_mode_t mode) {
    fs_t* fs = FS_FROM_DRV(drv);
    int flags = 0;

    flags = mode == LV_FS_MODE_RD                   ? FS_O_RDONLY
          : mode == LV_FS_MODE_WR                   ? FS_O_WRONLY
          : mode == (LV_FS_MODE_WR | LV_FS_MODE_RD) ? FS_O_RDWR
                                                    : 0;
    fs_file_t *f = lv_mem_alloc(sizeof(fs_file_t));
    if (f == NULL) {
        return NULL;
    }
    if (FS_ERR_OK != fs_file_open(fs, f, path, flags)) {
        lv_mem_free(f);
        return NULL;
    }
    return f;
}

static lv_fs_res_t lv_fs_close_cb(lv_fs_drv_t* drv, void* file) {
    (void)drv;
    fs_file_t *f = (fs_file_t*)file;

    int ret = fs_file_close(f);
    lv_mem_free(f);
    if (ret < 0) {
      return LV_FS_RES_UNKNOWN;
    }
    return LV_FS_RES_OK;
}

static lv_fs_res_t lv_fs_read_cb(lv_fs_drv_t* drv, void* file, void* buffer, uint32_t size, uint32_t* readed) {
    (void)drv;
    fs_file_t *f = (fs_file_t*)file;

    int ret = fs_file_read(f, buffer, size);
    if (ret < 0) {
      return LV_FS_RES_UNKNOWN;
    }
    *readed = ret;
    return LV_FS_RES_OK;
}

static lv_fs_res_t
lv_fs_write_cb(lv_fs_drv_t* drv, void* file, const void* buffer, uint32_t size, uint32_t* written) {
    (void)drv;
    fs_file_t *f = (fs_file_t*)file;

    int ret = fs_file_write(f, buffer, size);
    if (ret < 0) {
      return LV_FS_RES_UNKNOWN;
    }
    *written = ret;
    return LV_FS_RES_OK;
}

static lv_fs_res_t lv_fs_seek_cb(lv_fs_drv_t* drv, void* file, uint32_t off, lv_fs_whence_t whence) {
    (void)drv;
    fs_file_t *f = (fs_file_t*)file;

    int ret = fs_file_seek(f, off, (fs_whence_t)whence);
    if (ret < 0) {
      return LV_FS_RES_UNKNOWN;
    }
    return LV_FS_RES_OK;
}

static lv_fs_res_t lv_fs_tell_cb(lv_fs_drv_t* drv, void* file, uint32_t* pos) {
    (void)drv;
    fs_file_t *f = (fs_file_t*)file;

    int ret = fs_file_tell(f);
    if (ret < 0) {
      return LV_FS_RES_UNKNOWN;
    }
    *pos = ret;
    return LV_FS_RES_OK;
}

static void* lv_fs_dir_open_cb(lv_fs_drv_t* drv, const char* path) {
    fs_t* fs = FS_FROM_DRV(drv);
    fs_dir_t *dir = lv_mem_alloc(sizeof(fs_dir_t));
    if (dir == NULL) {
        return NULL;
    }
    if (FS_ERR_OK != fs_dir_open(fs, dir, path)) {
        lv_mem_free(dir);
        return NULL;
    }
    return dir;
}

static lv_fs_res_t lv_fs_dir_close_cb(lv_fs_drv_t* drv, void* dir) {
    (void)drv;
    fs_dir_t *d = (fs_dir_t*)dir;

    int ret = fs_dir_close(d);
    lv_mem_free(d);
    if (ret < 0) {
      return LV_FS_RES_UNKNOWN;
    }
    return LV_FS_RES_OK;
}

static lv_fs_res_t lv_fs_dir_read_cb(lv_fs_drv_t* drv, void* dir, char* fn) {
    (void)drv;
    fs_dir_t *d = (fs_dir_t*)dir;

    while(1) {
      fs_info_t info = {0};
      int ret = fs_dir_read(d, &info);
      if (ret > 0) {
        if (info.type == FS_TYPE_DIR) {
          fn[0] = '/';
          strcpy(fn + 1, info.name);
        } else {
          strcpy(fn, info.name);
        }
      } else if (ret == 0) {
        fn[9] = '\0';
      } else {
        return LV_FS_RES_UNKNOWN;
      }
      if (!strcmp(fn, "/.") || !strcmp(fn, "/..")) {
        continue;
      }
      break;
    }

    return LV_FS_RES_OK;
}

static lv_fs_drv_t lv_fs_drv;
void lv_port_fs_init(fs_t* fs) {
    lv_fs_drv_init(&lv_fs_drv);

    lv_fs_drv.letter = 'A';
    lv_fs_drv.cache_size = 0;
    lv_fs_drv.user_data = fs;

    // file system operations
    lv_fs_drv.ready_cb = lv_fs_ready_cb;
    lv_fs_drv.open_cb = lv_fs_open_cb;
    lv_fs_drv.close_cb = lv_fs_close_cb;
    lv_fs_drv.read_cb = lv_fs_read_cb;
    lv_fs_drv.write_cb = lv_fs_write_cb;
    lv_fs_drv.seek_cb = lv_fs_seek_cb;
    lv_fs_drv.tell_cb = lv_fs_tell_cb;
    lv_fs_drv.dir_open_cb = lv_fs_dir_open_cb;
    lv_fs_drv.dir_close_cb = lv_fs_dir_close_cb;
    lv_fs_drv.dir_read_cb = lv_fs_dir_read_cb;

    lv_fs_drv_register(&lv_fs_drv);
}
