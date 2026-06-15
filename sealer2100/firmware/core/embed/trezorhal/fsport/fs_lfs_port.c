#include "fs.h"
#if FS_USE_LFS == 1
#include <assert.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>

#include "fs.h"
#include "log.h"

#define MODULE "fs lfs port"
#define LFS_FROM_FS(fs)         (((lfs_port_t*)(fs)->port)->lfs)
#define CFG_FROM_FS(fs)        (((lfs_port_t*)(fs)->port)->cfg)
// File system operations

static int fs_lfs_file_open(fs_file_t* file, const char* path, int flags) {
    lfs_t* lfs = LFS_FROM_FS(file->fs);
    lfs_file_t* f = pvPortMalloc(sizeof(lfs_file_t));
    FS_DO_CHECK_MEMORY(f);

    int err = lfs_file_open(lfs, f, path, flags);

    if (err < 0) {
        vPortFree(f);
        f = NULL;
        return err;
    }

    file->fd = f;
    return err;
}

static int fs_lfs_file_close(fs_file_t* file) {
    lfs_t* lfs = LFS_FROM_FS(file->fs);

    int err = lfs_file_close(lfs, file->fd);
    if (err == FS_ERR_OK) {
        vPortFree(file->fd);
        file->fd = NULL;
    }
    return err;
}

static int fs_lfs_file_read(fs_file_t* file, void* buffer, uint32_t size) {
    lfs_t* lfs = LFS_FROM_FS(file->fs);

    int err = lfs_file_read(lfs, file->fd, buffer, size);
    return err;
}

static int fs_lfs_file_write(fs_file_t* file, const void* buffer, uint32_t size) {
    lfs_t* lfs = LFS_FROM_FS(file->fs);

    int err = lfs_file_write(lfs, file->fd, buffer, size);
    return err;
}
static int fs_lfs_file_truncate(fs_file_t* file, uint32_t size) {
    lfs_t* lfs = LFS_FROM_FS(file->fs);

    int err = lfs_file_truncate(lfs, file->fd, size);
    return err;
}

static int fs_lfs_file_sync(fs_file_t* file) {
    lfs_t* lfs = LFS_FROM_FS(file->fs);

    int err = lfs_file_sync(lfs, file->fd);
    return err;
}

static int fs_lfs_file_seek(fs_file_t* file, int off, fs_whence_t whence) {
    lfs_t* lfs = LFS_FROM_FS(file->fs);

    int err = lfs_file_seek(lfs, file->fd, off, whence);
    return err;
}

static int fs_lfs_file_tell(fs_file_t* file) {
    lfs_t* lfs = LFS_FROM_FS(file->fs);

    int err = lfs_file_tell(lfs, file->fd);
    return err;
}

static int fs_lfs_file_size(fs_file_t* file) {
    lfs_t* lfs = LFS_FROM_FS(file->fs);

    int err = lfs_file_size(lfs, file->fd);
    return err;
}

static int fs_lfs_stat(fs_t* fs, const char* path, fs_info_t* info) {
    lfs_t* lfs = LFS_FROM_FS(fs);
    struct lfs_info stat;
    int err = lfs_stat(lfs, path, &stat);
    if (err == FS_ERR_OK) {
        info->type = stat.type;
        info->size = stat.size;
        strcpy(info->name, stat.name);
    }
    return err;
}

static int fs_lfs_remove(fs_t* fs, const char* path) {
    lfs_t* lfs = LFS_FROM_FS(fs);
    int err = lfs_remove(lfs, path);
    return err;
}

static int fs_lfs_rename(fs_t* fs, const char* old_path, const char* new_path) {
    lfs_t* lfs = LFS_FROM_FS(fs);
    int err = lfs_rename(lfs, old_path, new_path);
    return err;
}

static int fs_lfs_mkdir(fs_t* fs, const char* path) {
    lfs_t* lfs = LFS_FROM_FS(fs);
    int err = lfs_mkdir(lfs, path);
    return err;
}

static int fs_lfs_dir_open(fs_dir_t* dir, const char* path) {
    lfs_t* lfs = LFS_FROM_FS(dir->fs);
    lfs_dir_t* dd = pvPortMalloc(sizeof(lfs_dir_t));
    FS_DO_CHECK_MEMORY(dd);

    int err = lfs_dir_open(lfs, dd, path);

    if (err < 0) {
        vPortFree(dd);
        dd = NULL;
        return err;
    }
    dir->dd = dd;
    return err;
}

static int fs_lfs_dir_close(fs_dir_t* dir) {
    lfs_t* lfs = LFS_FROM_FS(dir->fs);
    int err = lfs_dir_close(lfs, dir->dd);
    if (err == FS_ERR_OK) {
        vPortFree(dir->dd);
        dir->dd = NULL;
    }
    return err;
}

static int fs_lfs_dir_read(fs_dir_t* dir, fs_info_t* info) {
    lfs_t* lfs = LFS_FROM_FS(dir->fs);
    struct lfs_info stat;
    int err = lfs_dir_read(lfs, dir->dd, &stat);
    if (err > 0) {
        info->type = stat.type;
        info->size = stat.size;
        strcpy(info->name, stat.name);
    }
    return err;
}

static int fs_lfs_fs_stat(fs_t* fs, fs_fsinfo_t* info) {
    lfs_t* lfs = LFS_FROM_FS(fs);
    struct lfs_fsinfo fsinfo;
    int err = lfs_fs_stat(lfs, &fsinfo);
    if (err == FS_ERR_OK) {
        info->impl = "littlefs";
        info->block_size = fsinfo.block_size;
        info->block_count = fsinfo.block_count;
    }
    err = lfs_fs_size(lfs);
    if (err >= 0) {
        info->block_used = err;
    }
    return err;
}

static int fs_lfs_format(fs_t* fs) {
    lfs_t* lfs = LFS_FROM_FS(fs);
    const struct lfs_config* cfg = CFG_FROM_FS(fs);
    lfs_unmount(lfs);
    int ret = lfs_format(lfs, cfg);
    if (ret == LFS_ERR_OK) {
        ret = lfs_mount(lfs, cfg);
    }
    return ret;
}

// Register a file system driver.
void fs_lfs_register(fs_t* fs, lfs_port_t* port) {
    assert(fs);
    assert(port);
    fs->port = port;

    // file operations
    fs->file_open_cb = fs_lfs_file_open;
    fs->file_close_cb = fs_lfs_file_close;
    fs->file_read_cb = fs_lfs_file_read;
    fs->file_write_cb = fs_lfs_file_write;
    fs->file_truncate_cb = fs_lfs_file_truncate;
    fs->file_sync_cb = fs_lfs_file_sync;
    fs->file_seek_cb = fs_lfs_file_seek;
    fs->file_tell_cb = fs_lfs_file_tell;
    fs->file_size_cb = fs_lfs_file_size;
    fs->stat_cb = fs_lfs_stat;
    fs->remove_cb = fs_lfs_remove;
    fs->rename_cb = fs_lfs_rename;
    fs->mkdir_cb = fs_lfs_mkdir;
    fs->dir_open_cb = fs_lfs_dir_open;
    fs->dir_close_cb = fs_lfs_dir_close;
    fs->dir_read_cb = fs_lfs_dir_read;
    fs->fs_stat_cb = fs_lfs_fs_stat;
    fs->format_cb = fs_lfs_format;

    int ret = lfs_mount(port->lfs, port->cfg);
    if (ret < 0) {
        ret = fs_lfs_format(fs);
    }

    assert(ret == LFS_ERR_OK);
}

#endif
