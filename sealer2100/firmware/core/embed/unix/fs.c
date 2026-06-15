/*
 * Linux 模拟器上的 FS 实现：直接用 stdio/POSIX
 *
 */

#include "fs.h"

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdarg.h>

#include "profile.h"


// use `core/src/trezor` directly
#define SYS_DATA_DIR CURRENT_DIR RES_DIR
#define USER_DATA_DIR profile_user_data_path()

#define IsSeparator(c)	((c) == '/' || (c) == '\\')    

/*===========================================================
 * 工具函数：flags -> fopen 模式
 *===========================================================*/

static const char* fs_flags_to_mode(int flags) {
    int rd = (flags & FS_O_RDONLY) == FS_O_RDONLY;
    int wr = (flags & FS_O_WRONLY) == FS_O_WRONLY;
    int rw = (flags & FS_O_RDWR) == FS_O_RDWR;
    int c = (flags & FS_O_CREAT);
    int t = (flags & FS_O_TRUNC);
    int a = (flags & FS_O_APPEND);

    // 只实现常用部分，够模拟用即可
    if (rw) {
        if (c && t)
            return "w+b"; // 创建并清空 读写
        else if (a)
            return "a+b"; // 追加 读写
        else
            return "r+b"; // 读写，文件必须存在
    } else if (wr) {
        if (c && t)
            return "wb"; // 创建并清空 写
        else if (a)
            return "ab"; // 追加 写
        else if (c)
            return "wb"; // 简化：有 C 没 T 也用 wb
        else
            return "wb"; // 简化处理
    } else {             // 默认只读
        (void)rd;        // 防止未使用警告
        return "rb";
    }
}

static void fs_get_full_path(const char* path, char* full_path) {
    // convert device path to unix path
    while (IsSeparator(*path)) path++;	/* Strip separators */
    // /user/xxx -> `USER_DATA_DIR`/user/xxx
    if (strcmp(path, "user") == 0) {
        // only /user
        sprintf(full_path, "%s/%s", USER_DATA_DIR, path);
    } else if (strncmp(path, "user/", 5) == 0) {
        // begin with /user/
        sprintf(full_path, "%s/%s", USER_DATA_DIR, path);
    } else {
        sprintf(full_path, "%s/%s", SYS_DATA_DIR, path);
    }
}

/*===========================================================
 * FS 初始化 / 反初始化
 *===========================================================*/

void fs_init(fs_t* fs) {
    memset(fs, 0, sizeof(*fs));
    fs->letter = 'U'; // 随便一个盘符，占位用
    fs->port = NULL;  // Linux 模拟不需要 port
}

void fs_deinit(fs_t* fs) {
    /* Linux 模拟无特别资源要释放 */
    (void)fs;
}

/*===========================================================
 * 文件操作
 *===========================================================*/

int fs_file_open(fs_t* fs, fs_file_t* file, const char* path, int flags) {
    FS_DO_CHECK_PARAM_NULL(file);
    FS_DO_CHECK_PARAM_NULL(path);

    char full_path[256];
    fs_get_full_path(path, full_path);
    const char* mode = fs_flags_to_mode(flags);
    FILE* fp = fopen(full_path, mode);
    if (!fp) {
        return -errno;
    }

    file->fs = fs;
    file->fd = fp;
    return FS_ERR_OK;
}

int fs_file_close(fs_file_t* file) {
    FS_DO_CHECK_PARAM_NULL(file);
    FILE* fp = (FILE*)file->fd;
    if (!fp)
        return FS_ERR_BADF;

    if (fclose(fp) != 0) {
        return -errno;
    }
    file->fd = NULL;
    return FS_ERR_OK;
}

int fs_file_read(fs_file_t* file, void* buf, uint32_t size) {
    FS_DO_CHECK_PARAM_NULL(file);
    FS_DO_CHECK_PARAM_NULL(buf);
    FILE* fp = (FILE*)file->fd;
    if (!fp)
        return FS_ERR_BADF;

    size_t n = fread(buf, 1, size, fp);
    if (n < size && ferror(fp)) {
        return FS_ERR_IO;
    }
    return (int)n; // 返回实际读取字节数，0 表示 EOF
}

int fs_file_write(fs_file_t* file, const void* buf, uint32_t size) {
    FS_DO_CHECK_PARAM_NULL(file);
    FS_DO_CHECK_PARAM_NULL(buf);
    FILE* fp = (FILE*)file->fd;
    if (!fp)
        return FS_ERR_BADF;

    size_t n = fwrite(buf, 1, size, fp);
    if (n < size) {
        return FS_ERR_IO;
    }
    return (int)n; // 返回实际写入字节数
}

int fs_file_truncate(fs_file_t* file, uint32_t size) {
    FS_DO_CHECK_PARAM_NULL(file);
    FILE* fp = (FILE*)file->fd;
    if (!fp)
        return FS_ERR_BADF;

    int fd = fileno(fp);
    if (fd < 0)
        return FS_ERR_IO;

    if (ftruncate(fd, size) != 0) {
        return FS_ERR_IO;
    }
    return FS_ERR_OK;
}

int fs_file_sync(fs_file_t* file) {
    FS_DO_CHECK_PARAM_NULL(file);
    FILE* fp = (FILE*)file->fd;
    if (!fp)
        return FS_ERR_BADF;

    if (fflush(fp) != 0) {
        return FS_ERR_IO;
    }
    return FS_ERR_OK;
}

int fs_file_seek(fs_file_t* file, int pos, fs_whence_t whence) {
    FS_DO_CHECK_PARAM_NULL(file);
    FILE* fp = (FILE*)file->fd;
    if (!fp)
        return FS_ERR_BADF;

    int origin;
    switch (whence) {
    case FS_SEEK_SET:
        origin = SEEK_SET;
        break;
    case FS_SEEK_CUR:
        origin = SEEK_CUR;
        break;
    case FS_SEEK_END:
        origin = SEEK_END;
        break;
    default:
        return FS_ERR_INVAL;
    }

    if (fseek(fp, pos, origin) != 0) {
        return FS_ERR_IO;
    }
    long p = ftell(fp);
    if (p < 0)
        return FS_ERR_IO;
    return (int)p;
}

int fs_file_tell(fs_file_t* file) {
    FS_DO_CHECK_PARAM_NULL(file);
    FILE* fp = (FILE*)file->fd;
    if (!fp)
        return FS_ERR_BADF;

    long p = ftell(fp);
    if (p < 0)
        return FS_ERR_IO;
    return (int)p;
}

int fs_file_size(fs_file_t* file) {
    FS_DO_CHECK_PARAM_NULL(file);
    FILE* fp = (FILE*)file->fd;
    if (!fp)
        return FS_ERR_BADF;

    long cur = ftell(fp);
    if (cur < 0)
        return FS_ERR_IO;

    if (fseek(fp, 0, SEEK_END) != 0)
        return FS_ERR_IO;
    long end = ftell(fp);
    if (end < 0)
        return FS_ERR_IO;

    if (fseek(fp, cur, SEEK_SET) != 0)
        return FS_ERR_IO;
    return (int)end;
}

/*===========================================================
 * 路径相关：stat / remove / rename / mkdir
 *===========================================================*/

int fs_stat(fs_t* fs, const char* path, fs_info_t* info) {
    (void)fs;
    FS_DO_CHECK_PARAM_NULL(path);
    FS_DO_CHECK_PARAM_NULL(info);
    char full_path[256];
    fs_get_full_path(path, full_path);

    struct stat st;
    if (stat(full_path, &st) != 0) {
        if (errno == ENOENT)
            return FS_ERR_NOENT;
        return FS_ERR_IO;
    }

    if (S_ISREG(st.st_mode))
        info->type = FS_TYPE_REG;
    else if (S_ISDIR(st.st_mode))
        info->type = FS_TYPE_DIR;
    else
        info->type = FS_TYPE_REG; // 其他类型统统当普通文件

    info->size = (uint32_t)st.st_size;

    const char* name = fs_base_name(full_path);
    strncpy(info->name, name, FS_NAME_MAX);
    info->name[FS_NAME_MAX] = '\0';

    return FS_ERR_OK;
}

int fs_remove(fs_t* fs, const char* path) {
    (void)fs;
    FS_DO_CHECK_PARAM_NULL(path);
    char full_path[256];
    fs_get_full_path(path, full_path);

    if (remove(full_path) != 0) {
        if (errno == ENOENT)
            return FS_ERR_NOENT;
        if (errno == ENOTEMPTY)
            return FS_ERR_NOTEMPTY;
        return FS_ERR_IO;
    }
    return FS_ERR_OK;
}

int fs_rename(fs_t* fs, const char* old_path, const char* new_path) {
    (void)fs;
    FS_DO_CHECK_PARAM_NULL(old_path);
    FS_DO_CHECK_PARAM_NULL(new_path);
    char old_full_path[256];
    char new_full_path[256];
    fs_get_full_path(old_path, old_full_path);
    fs_get_full_path(new_path, new_full_path);

    if (rename(old_full_path, new_full_path) != 0) {
        if (errno == ENOENT)
            return FS_ERR_NOENT;
        if (errno == EEXIST)
            return FS_ERR_EXIST;
        return FS_ERR_IO;
    }
    return FS_ERR_OK;
}

int fs_mkdir(fs_t* fs, const char* path) {
    (void)fs;
    FS_DO_CHECK_PARAM_NULL(path);
    char full_path[256];
    fs_get_full_path(path, full_path);

    if (mkdir(full_path, 0755) != 0) {
        if (errno == EEXIST)
            return FS_ERR_OK;
        return FS_ERR_IO;
    }
    return FS_ERR_OK;
}

/*===========================================================
 * 目录操作
 *===========================================================*/

int fs_dir_open(fs_t* fs, fs_dir_t* dir, const char* path) {
    FS_DO_CHECK_PARAM_NULL(dir);
    FS_DO_CHECK_PARAM_NULL(path);
    char full_path[256];
    fs_get_full_path(path, full_path);

    DIR* dp = opendir(full_path);
    if (!dp) {
        if (errno == ENOENT)
            return FS_ERR_NOENT;
        return FS_ERR_IO;
    }

    dir->fs = fs;
    dir->dd = dp;
    return FS_ERR_OK;
}

int fs_dir_close(fs_dir_t* dir) {
    FS_DO_CHECK_PARAM_NULL(dir);
    DIR* dp = (DIR*)dir->dd;
    if (!dp)
        return FS_ERR_BADF;

    closedir(dp);
    dir->dd = NULL;
    return FS_ERR_OK;
}

/*
 * fs_dir_read:
 *   返回 FS_ERR_OK      -> 填充了 1 个 fs_info_t
 *   返回 0              -> 目录读取完毕（无更多条目）
 *   返回 <0 (其他错误)  -> 见错误码
 */
int fs_dir_read(fs_dir_t* dir, fs_info_t* info) {
    FS_DO_CHECK_PARAM_NULL(dir);
    FS_DO_CHECK_PARAM_NULL(info);

    DIR* dp = (DIR*)dir->dd;
    if (!dp)
        return FS_ERR_BADF;

    struct dirent* ent = readdir(dp);
    if (!ent) {
        // 读到末尾
        return 0;
    }

    strncpy(info->name, ent->d_name, FS_NAME_MAX);
    info->name[FS_NAME_MAX] = '\0';

    // 类型：有些平台 d_type 可能是 DT_UNKNOWN，此时可以用 stat 再查一次
    if (ent->d_type == DT_DIR)
        info->type = FS_TYPE_DIR;
    else if (ent->d_type == DT_REG)
        info->type = FS_TYPE_REG;
    else
        info->type = FS_TYPE_REG;

    // size 简化为 0；如需要可拼出 path 再 stat 一次
    info->size = 0;

    return 1;
}

/*===========================================================
 * 文件系统整体信息 / 格式化
 *===========================================================*/

int fs_fs_stat(fs_t* fs, fs_fsinfo_t* info) {
    (void)fs;
    FS_DO_CHECK_PARAM_NULL(info);

    memset(info, 0, sizeof(*info));
    // 简单标明实现类型
    info->impl = "linux-stdio";

    // 如果你定义了 total/used 字段，可以不填，或填 0
    return FS_ERR_OK;
}

int fs_format(fs_t* fs) {
    (void)fs;
    // Linux 模拟通常不支持真正格式化
    return FS_ERR_NOSYS;
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
