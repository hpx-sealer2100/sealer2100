#include <stdbool.h>

#include "fs.h"
#include "lvgl.h"
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
