#include "fs.h"

#include <stdarg.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include "log.h"
#include "uart_log.h"

#define MODULE "fs"


void fs_init(fs_t *fs) {
    memset(fs, 0, sizeof(fs_t));
}

int fs_file_open(fs_t *fs, fs_file_t *file, const char *path, int flags) {
    FS_DO_CHECK_PARAM_NULL(fs);
    FS_DO_CHECK_PARAM_NULL(file);
    FS_DO_CHECK_PARAM_NULL(path);

    FS_DO_CHECK_IMPLEMENTED(fs, file_open_cb);
    // fill file descriptor
    file->fs = fs;
    return fs->file_open_cb(file, path, flags);
}

int fs_file_close(fs_file_t *file) {
    FS_DO_CHECK_PARAM_NULL(file);
    FS_DO_CHECK_PARAM_NULL(file->fs);
    FS_DO_CHECK_PARAM_NULL(file->fd);

    fs_t *fs = file->fs;

    FS_DO_CHECK_IMPLEMENTED(fs, file_close_cb);
    return fs->file_close_cb(file);
}

int fs_file_read(fs_file_t *file, void *buf, uint32_t len) {
    FS_DO_CHECK_PARAM_NULL(file);
    FS_DO_CHECK_PARAM_NULL(file->fs);
    FS_DO_CHECK_PARAM_NULL(file->fd);
    FS_DO_CHECK_PARAM_NULL(buf);

    fs_t *fs = file->fs;

    FS_DO_CHECK_IMPLEMENTED(fs, file_read_cb);

    if (len == 0) {
        return FS_ERR_OK;
    }

    return fs->file_read_cb(file, buf, len);
}

int fs_file_write(fs_file_t *file, const void *buf, uint32_t len) {
    FS_DO_CHECK_PARAM_NULL(file);
    FS_DO_CHECK_PARAM_NULL(file->fs);
    FS_DO_CHECK_PARAM_NULL(file->fd);
    FS_DO_CHECK_PARAM_NULL(buf);

    fs_t *fs = file->fs;

    FS_DO_CHECK_IMPLEMENTED(fs, file_write_cb);
    if (len == 0) {
        return FS_ERR_OK;
    }
    return fs->file_write_cb(file, buf, len);
}

int fs_file_truncate(fs_file_t *file, uint32_t size) {
    FS_DO_CHECK_PARAM_NULL(file);
    FS_DO_CHECK_PARAM_NULL(file->fs);
    FS_DO_CHECK_PARAM_NULL(file->fd);

    fs_t *fs = file->fs;

    FS_DO_CHECK_IMPLEMENTED(fs, file_truncate_cb);
    return fs->file_truncate_cb(file, size);
}

int fs_file_sync(fs_file_t *file) {
    FS_DO_CHECK_PARAM_NULL(file);
    FS_DO_CHECK_PARAM_NULL(file->fs);
    FS_DO_CHECK_PARAM_NULL(file->fd);

    fs_t *fs = file->fs;

    FS_DO_CHECK_IMPLEMENTED(fs, file_sync_cb);
    return fs->file_sync_cb(file);
}

int fs_file_seek(fs_file_t *file, int pos, fs_whence_t whence) {
    FS_DO_CHECK_PARAM_NULL(file);
    FS_DO_CHECK_PARAM_NULL(file->fs);
    FS_DO_CHECK_PARAM_NULL(file->fd);

    fs_t *fs = file->fs;

    FS_DO_CHECK_IMPLEMENTED(fs, file_seek_cb);
    return fs->file_seek_cb(file, pos, whence);
}

int fs_file_tell(fs_file_t *file) {
    FS_DO_CHECK_PARAM_NULL(file);
    FS_DO_CHECK_PARAM_NULL(file->fs);
    FS_DO_CHECK_PARAM_NULL(file->fd);

    fs_t *fs = file->fs;

    FS_DO_CHECK_IMPLEMENTED(fs, file_tell_cb);
    return fs->file_tell_cb(file);
}

int fs_file_size(fs_file_t *file) {
    FS_DO_CHECK_PARAM_NULL(file);
    FS_DO_CHECK_PARAM_NULL(file->fs);
    FS_DO_CHECK_PARAM_NULL(file->fd);

    fs_t *fs = file->fs;

    FS_DO_CHECK_IMPLEMENTED(fs, file_size_cb);
    return fs->file_size_cb(file);
}

int fs_stat(fs_t *fs, const char *path, fs_info_t *info) {
    FS_DO_CHECK_PARAM_NULL(fs);
    FS_DO_CHECK_PARAM_NULL(path);
    FS_DO_CHECK_PARAM_NULL(info);

    FS_DO_CHECK_IMPLEMENTED(fs, stat_cb);
    return fs->stat_cb(fs, path, info);
}

int fs_remove(fs_t *fs, const char *path) {
    FS_DO_CHECK_PARAM_NULL(fs);
    FS_DO_CHECK_PARAM_NULL(path);

    FS_DO_CHECK_IMPLEMENTED(fs, remove_cb);
    return fs->remove_cb(fs, path);
}

int fs_rename(fs_t *fs, const char *old_path, const char *new_path) {
    FS_DO_CHECK_PARAM_NULL(fs);
    FS_DO_CHECK_PARAM_NULL(old_path);
    FS_DO_CHECK_PARAM_NULL(new_path);

    FS_DO_CHECK_IMPLEMENTED(fs, rename_cb);
    return fs->rename_cb(fs, old_path, new_path);
}

int fs_mkdir(fs_t *fs, const char *path) {
    FS_DO_CHECK_PARAM_NULL(fs);
    FS_DO_CHECK_PARAM_NULL(path);

    FS_DO_CHECK_IMPLEMENTED(fs, mkdir_cb);
    return fs->mkdir_cb(fs, path);
}

int fs_dir_open(fs_t *fs, fs_dir_t *dir, const char *path) {
    FS_DO_CHECK_PARAM_NULL(fs);
    FS_DO_CHECK_PARAM_NULL(dir);
    FS_DO_CHECK_PARAM_NULL(path);

    FS_DO_CHECK_IMPLEMENTED(fs, dir_open_cb);
    // fill directory descriptor
    dir->fs = fs;
    return fs->dir_open_cb(dir, path);
}

int fs_dir_close(fs_dir_t *dir) {
    FS_DO_CHECK_PARAM_NULL(dir);
    FS_DO_CHECK_PARAM_NULL(dir->fs);
    FS_DO_CHECK_PARAM_NULL(dir->dd);

    fs_t *fs = dir->fs;

    FS_DO_CHECK_IMPLEMENTED(fs, dir_close_cb);
    return fs->dir_close_cb(dir);
}

int fs_dir_read(fs_dir_t *dir, fs_info_t *info) {
    FS_DO_CHECK_PARAM_NULL(dir);
    FS_DO_CHECK_PARAM_NULL(dir->fs);
    FS_DO_CHECK_PARAM_NULL(dir->dd);
    FS_DO_CHECK_PARAM_NULL(info);

    fs_t *fs = dir->fs;

    FS_DO_CHECK_IMPLEMENTED(fs, dir_read_cb);
    return fs->dir_read_cb(dir, info);
}

int fs_fs_stat(fs_t *fs, fs_fsinfo_t *info) {
    FS_DO_CHECK_PARAM_NULL(fs);
    FS_DO_CHECK_PARAM_NULL(info);

    FS_DO_CHECK_IMPLEMENTED(fs, fs_stat_cb);
    return fs->fs_stat_cb(fs, info);
}

int fs_format(fs_t *fs) {
    FS_DO_CHECK_PARAM_NULL(fs);

    FS_DO_CHECK_IMPLEMENTED(fs, format_cb);
    return fs->format_cb(fs);
}

const char *fs_get_ext(const char *name) {
    if (!name) {
        return "";
    }

    for(size_t i = strlen(name); i > 0; i--) {
        if(name[i] == '.') {
            return &name[i + 1];
        }
        else if(name[i] == '/' || name[i] == '\\') {
            return ""; /*No extension if a '\' or '/' found*/
        }
    }

    return ""; /*Empty string if no '.' in the file name.*/
}

char* fs_dir_name(char* path) {
    if (!path) {
        return "";
    }

    size_t len = strlen(path);
    if(len == 0) return path;

    len--; /*Go before the trailing '\0'*/

    /*Ignore trailing '/' or '\'*/
    while(path[len] == '/' || path[len] == '\\') {
        path[len] = '\0';
        if(len > 0)
            len--;
        else
            return path;
    }

    size_t i = 0;
    for(i = len; i > 0; i--) {
        if(path[i] == '/' || path[i] == '\\') break;
    }

    if(i > 0) path[i] = '\0';

    return path;
}

const char* fs_base_name(const char* path) {
    if (!path) {
        return "";
    }

    size_t len = strlen(path);
    if(len == 0) return path;

    len--; /*Go before the trailing '\0'*/

    /*Ignore trailing '/' or '\'*/
    while(path[len] == '/' || path[len] == '\\') {
        if(len > 0)
            len--;
        else
            return path;
    }

    size_t i;
    for(i = len; i > 0; i--) {
        if(path[i] == '/' || path[i] == '\\') break;
    }

    /*No '/' or '\' in the path so return with path itself*/
    if(i == 0) return path;

    return &path[i + 1];
}

void fs_path_join(char* dst, const char* path, ...) {
  va_list ap;
  const char *p;
  size_t len = 0;

  // 初始化 dst 为空串
  if (!dst)
      return;
  dst[0] = '\0';

  if (!path)
      return;

  va_start(ap, path);
  p = path;

  while (p) {
      // 跳过空字符串
      if (*p == '\0') {
          p = va_arg(ap, const char *);
          continue;
      }

      // 如果 dst 目前为空，直接拷贝（但要去掉 p 开头多余的 '/'）
      if (len == 0) {
          // 跳过前导多余 '/'
          while (*p == '/' && *(p + 1) == '/')
              p++;

          strcpy(dst, p);
          len = strlen(dst);
      } else {
          int dst_has_slash  = (len > 0 && dst[len - 1] == '/');
          int path_has_slash = (p[0] == '/');

          if (dst_has_slash && path_has_slash) {
              // 两边都有 '/', 跳过 path 的 '/'
              p++;
          } else if (!dst_has_slash && !path_has_slash) {
              // 两边都没有 '/', 需要补一个
              dst[len++] = '/';
              dst[len] = '\0';
          }
          // 现在连接剩余的部分
          strcat(dst, p);
          len = strlen(dst);
      }
      p = va_arg(ap, const char *);
  }
  va_end(ap);
}




// tests functions
typedef enum {
    SIZE_TYPE_BYTE,
    SIZE_TYPE_KB,
    SIZE_TYPE_MB,
    SIZE_TYPE_GB,
} size_type_t;

typedef struct {
    size_type_t type;
    int size;
} xsize_t;

static xsize_t xsize_from_bytes(int size) {
    xsize_t xsize;
    if (size < 1024) {
        xsize.type = SIZE_TYPE_BYTE;
        xsize.size = size;
        return xsize;
    } else if (size < 1024 * 1024) {
        xsize.type = SIZE_TYPE_KB;
        xsize.size = size / 1024;
        return xsize;
    } else if (size < 1024 * 1024 * 1024) {
        xsize.type = SIZE_TYPE_MB;
        xsize.size = size / 1024 / 1024;
        return xsize;
    } else {
        xsize.type = SIZE_TYPE_GB;
        xsize.size = size / 1024;
        return xsize;
    }
}

static const char* xsize_stringfy(xsize_t xsize) {
    static char buf[128];
    switch (xsize.type) {
    case SIZE_TYPE_BYTE:
        snprintf(buf, sizeof(buf), "%d B", xsize.size);
        return buf;
    case SIZE_TYPE_KB:
        snprintf(buf, sizeof(buf), "%d KB", xsize.size);
        return buf;
    case SIZE_TYPE_MB:
        snprintf(buf, sizeof(buf), "%d MB", xsize.size);
        return buf;
    case SIZE_TYPE_GB:
        snprintf(buf, sizeof(buf), "%d GB", xsize.size);
        return buf;
    default:
        return "unknown";
    }
}

#define SIZE_STR(size) xsize_stringfy(xsize_from_bytes(size))
static void test_list_dir(fs_t* fs, const char* path) {
    LOG_DEBUG(MODULE, "list dir: %s", path);
    fs_dir_t dir;
    int err = fs_dir_open(fs, &dir, path);
    if (err) {
        LOG_ERROR(MODULE, "open failed: %d", err);
        return;
    }

    while (1) {
        fs_info_t info;
        err = fs_dir_read(&dir, &info);
        if (err == 0) {
            // end of directory
            LOG_DEBUG(MODULE, "list complete of %s", path);
            break;
        }
        if (err < 0) {
            LOG_ERROR(MODULE, "dir read failed: %d of %s", err, path);
            break;
        }
        char full_path[128];
        fs_path_join(full_path, path, info.name, NULL);
        if (info.type == FS_TYPE_DIR) {
            if (strcmp(info.name, ".") == 0 || strcmp(info.name, "..") == 0) {
                // skip self and parent directory
                continue;
            }
            // recurse into subdirectory
            test_list_dir(fs, full_path);
        }
        if (info.type == FS_TYPE_REG) {
            LOG_DEBUG(MODULE, "file: %s, size: %s", full_path, SIZE_STR(info.size));
            uart_log_flush();
        }
    }
    fs_dir_close(&dir);
}

void fs_test_list_all_files(fs_t* fs) {
    test_list_dir(fs, "/");
}

void fs_speed_test(fs_t* fs) {
#define TEST_FILE_SIZE (1024 * 1024 * 10ul) // 10MB
#define TEST_FILE_NAME "/test.bin"
    // fs_mkdir(fs, "/.dummy");
    // fs_remove(fs, "/.dummy");

    unsigned char block[16 * 1024];

    // write test file
    fs_file_t f;
    int err = fs_file_open(fs, &f, TEST_FILE_NAME, FS_O_RDWR | FS_O_CREAT);
    if (err) {
        LOG_ERROR(MODULE, "fs_file_open failed: %d", err);
        return;
    }

    extern uint32_t HAL_GetTick(void);
    uint32_t now = HAL_GetTick();
    for (int i = 0; i < TEST_FILE_SIZE / sizeof(block); i++) {
        uint8_t b = i % 256;
        memset(block, b, sizeof(block));
        err = fs_file_write(&f, block, sizeof(block));
        if (err < 0) {
            LOG_ERROR(MODULE, "fs_file_write failed: %d", err);
            return;
        }
    }
    uint32_t end = HAL_GetTick();
    uint32_t duration = end - now;
    uint64_t speed = TEST_FILE_SIZE;
    speed *= 1000;
    speed /= duration;
    LOG_INFO(MODULE, "write duration: %d ms", duration);
    LOG_INFO(MODULE, "write speed: %ld B/s", speed);

    fs_file_seek(&f, 0, FS_SEEK_SET);

    // read test file
    now = HAL_GetTick();
    for (int i = 0; i < TEST_FILE_SIZE / sizeof(block); i++) {
        err = fs_file_read(&f, block, sizeof(block));
        if (err < 0) {
            LOG_ERROR(MODULE, "fs_file_read failed: %d", err);
            return;
        }
    }
    end = HAL_GetTick();
    duration = end - now;
    speed = TEST_FILE_SIZE;
    speed *= 1000;
    speed /= duration;
    LOG_INFO(MODULE, "read duration: %d ms", duration);
    LOG_INFO(MODULE, "read speed: %ld B/s", speed);
    fs_file_close(&f);
    fs_remove(fs, TEST_FILE_NAME);
}
