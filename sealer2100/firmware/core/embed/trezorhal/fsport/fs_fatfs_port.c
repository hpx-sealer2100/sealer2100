#include <assert.h>
#include <stdint.h>
#include <string.h>

// clang-format off
#include "ff.h"
#include "diskio.h"
#include "emmc.h"
// clang-format on
#include "fs.h"
#include "fatfs_diskio.h"
#include "log.h"
#include "secure_heap.h"

#define MODULE "fatfs port"

#define PARTITION_0_BLOCKS (2 * 1024 * 1024) // 512 bytes/block, 1G

PARTITION VolToPart[FF_VOLUMES] = {
    {0, 1},
};

// helper function
static BYTE fs_flags_to_fa_mode(int flags)
{
    BYTE mode = 0;

    switch (flags & 0x3) {
    case FS_O_RDONLY:
        mode |= FA_READ;
        break;
    case FS_O_WRONLY:
        mode |= FA_WRITE;
        break;
    case FS_O_RDWR:
        mode |= (FA_READ | FA_WRITE);
        break;
    default:
        mode |= FA_READ;
        break;
    }

    const int creat  = (flags & FS_O_CREAT)  != 0;
    const int excl   = (flags & FS_O_EXCL)   != 0;
    const int trunc  = (flags & FS_O_TRUNC)  != 0;
    const int append = (flags & FS_O_APPEND) != 0;

    if (!creat && !append && !trunc) {
        mode |= FA_OPEN_EXISTING;
    } else if (creat && excl) {
        mode |= FA_CREATE_NEW;
    } else if (creat && trunc) {
        mode |= FA_CREATE_ALWAYS;
    } else if (creat && !trunc && !excl && !append) {
        mode |= FA_OPEN_ALWAYS;
    } else if (append) {
        mode |= FA_OPEN_APPEND;
        if (!(mode & FA_WRITE)) {
            mode |= FA_WRITE;
        }
    } else {
        mode |= FA_OPEN_EXISTING;
    }

    return mode;
}

static int fr_to_fs_err(FRESULT fr)
{
    switch (fr) {
    case FR_OK:
        return FS_ERR_OK;
    case FR_DISK_ERR:
    case FR_NOT_READY:
    case FR_WRITE_PROTECTED:
        return FS_ERR_IO;
    case FR_NO_FILE:
    case FR_NO_PATH:
        return FS_ERR_NOENT;
    case FR_INVALID_NAME:
        return FS_ERR_NAMETOOLONG;
    case FR_INVALID_PARAMETER:
        return FS_ERR_INVAL;
    case FR_EXIST:
        return FS_ERR_EXIST;
    case FR_DENIED:
        return FS_ERR_NOSYS;
    case FR_INVALID_OBJECT:
        return FS_ERR_BADF;
    case FR_LOCKED:
        return FS_ERR_BADF;
    case FR_NO_FILESYSTEM:
        return FS_ERR_CORRUPT;
    case FR_NOT_ENOUGH_CORE:
        return FS_ERR_NOMEM;
    case FR_TOO_MANY_OPEN_FILES:
        return FS_ERR_NOMEM;
    case FR_INT_ERR:
        return FS_ERR_CORRUPT;
    case FR_MKFS_ABORTED:
        return FS_ERR_IO;
    case FR_TIMEOUT:
        return FS_ERR_IO;
    case FR_INVALID_DRIVE:
        return FS_ERR_INVAL;
    default:
        return FS_ERR_IO;
    }
}


// File system operations

static int fs_fatfs_format(fs_t* fs) {
  assert(fs && fs->port);
  // 1 partitions
  FATFS *fatfs = ((fatfs_port_t*)fs->port)->fatfs;

  LBA_t plist[] = {100};
  BYTE work[FF_MAX_SS];
  MKFS_PARM mk_para = {
      .fmt = FM_FAT32,
  };

  FRESULT err = f_fdisk(0, plist, work);
  if (err != FR_OK) {
    LOG_ERROR(MODULE, "fdisk failed, err = %d", err);
    return fr_to_fs_err(err);
  }

  // format partition 0
  err = f_mkfs("", &mk_para, work, sizeof(work));
  if (err != FR_OK) {
    LOG_ERROR(MODULE, "mkfs failed, err = %d", err);
    return fr_to_fs_err(err);
  }

  err = f_mount(fatfs, "", 1);
  if (err != FR_OK) {
    LOG_ERROR(MODULE, "mount failed, err = %d", err);
    return fr_to_fs_err(err);
  }

  err = f_setlabel("System");
  if (err != FR_OK) {
    LOG_ERROR(MODULE, "setlabel failed, err = %d", err);
    return fr_to_fs_err(err);
  }

  return FS_ERR_OK;
}

static int fs_fatfs_file_open(fs_file_t* file, const char* path, int flags) {
    FIL* f = pvPortMalloc(sizeof(FIL));
    FS_DO_CHECK_MEMORY(f);

    BYTE mode = fs_flags_to_fa_mode(flags);

    FRESULT err = f_open(f, path, mode);
    if (err != FR_OK) {
      vPortFree(f);
      f = NULL;
      return fr_to_fs_err(err);
    }
    file->fd = f;
    return FS_ERR_OK;
}

static int fs_fatfs_file_close(fs_file_t* file) {

    FRESULT err = f_close((FIL*)file->fd);
    if (err == FR_OK) {
      vPortFree(file->fd);
      file->fd = NULL;
    }
    return fr_to_fs_err(err);
}

static int fs_fatfs_file_read(fs_file_t* file, void* buf, uint32_t size) {
    UINT readed = 0;
    FRESULT err = f_read((FIL*)file->fd, buf, size, &readed);
    if (err != FR_OK) {
      return fr_to_fs_err(err);
    }
    return readed;
}

static int fs_fatfs_file_write(fs_file_t* file, const void* buf, uint32_t size) {
    UINT writed = 0;
    FRESULT err = f_write((FIL*)file->fd, buf, size, &writed);
    if (err != FR_OK) {
      return fr_to_fs_err(err);
    }
    return writed;
}

static int fs_fatfs_file_truncate(fs_file_t* file, uint32_t size) {
    FRESULT err = f_lseek((FIL*)file->fd, size);
    if (err != FR_OK) {
      return fr_to_fs_err(err);
    }
    err = f_truncate((FIL*)file->fd);
    if (err != FR_OK) {
      return fr_to_fs_err(err);
    }
    return FS_ERR_OK;
}

static int fs_fatfs_file_sync(fs_file_t* file) {
    FRESULT err = f_sync((FIL*)file->fd);
    if (err != FR_OK) {
      return fr_to_fs_err(err);
    }
    return FS_ERR_OK;
}



static int fs_fatfs_file_seek(fs_file_t* file, int pos, fs_whence_t whence) {
    int seeked = 0;
    switch (whence) {
      case FS_SEEK_SET:
        seeked = pos;
        break;
      case FS_SEEK_CUR:
        seeked = f_tell((FIL*)file->fd);
        seeked += pos;
        break;
      case FS_SEEK_END:
        seeked = f_size((FIL*)file->fd);
        seeked += pos;
        break;

    }
    FRESULT err = f_lseek((FIL*)file->fd, seeked);
    if (err != FR_OK) {
      return fr_to_fs_err(err);
    }
    return seeked;
}

static int fs_fatfs_file_tell(fs_file_t* file) {
    return f_tell((FIL*)file->fd);
}
static int fs_fatfs_file_size(fs_file_t* file) {
    return f_size((FIL*)file->fd);
}

static int fs_fatfs_stat(fs_t* fs, const char* path, fs_info_t* info) {
    (void)fs;
    FILINFO finfo;
    FRESULT err = f_stat(path, &finfo);
    if (err != FR_OK) {
      return fr_to_fs_err(err);
    }
    info->size = finfo.fsize;
    info->type = (finfo.fattrib & AM_DIR) ? FS_TYPE_DIR : FS_TYPE_REG;
    strcpy(info->name, finfo.fname);
    return FS_ERR_OK;
}

static int fs_fatfs_remove(fs_t* fs, const char* path) {
    (void)fs;
    FRESULT err = f_unlink(path);
    if (err != FR_OK) {
      return fr_to_fs_err(err);
    }
    return FS_ERR_OK;
}

static int fs_fatfs_rename(fs_t* fs, const char* old_path, const char* new_path) {
    (void)fs;
    FRESULT err = f_rename(old_path, new_path);
    if (err != FR_OK) {
      return fr_to_fs_err(err);
    }
    return FS_ERR_OK;
}

static int fs_fatfs_mkdir(fs_t* fs, const char* path) {
    (void)fs;
    FRESULT err = f_mkdir(path);
    if (err != FR_OK) {
      return fr_to_fs_err(err);
    }
    return FS_ERR_OK;
}

static int fs_fatfs_dir_open(fs_dir_t* dir, const char* path) {
    DIR* dd = pvPortMalloc(sizeof(DIR));
    FS_DO_CHECK_MEMORY(dd);
    FRESULT err = f_opendir(dd, path);
    if (err != FR_OK) {
      vPortFree(dd);
      dd = NULL;
      return fr_to_fs_err(err);
    }
    dir->dd = dd;
    return FS_ERR_OK;
}

static int fs_fatfs_dir_close(fs_dir_t* dir) {
    FRESULT err = f_closedir((DIR*)dir->dd);
    if (err == FR_OK) {
      vPortFree(dir->dd);
      dir->dd = NULL;
    }
    return fr_to_fs_err(err);
}

static int fs_fatfs_dir_read(fs_dir_t* dir, fs_info_t* info) {
    DIR* dd = (DIR*)dir->dd;
    if (dd == NULL) {
      return FS_ERR_BADF;
    }
    FILINFO finfo;
    FRESULT err = f_readdir(dd, &finfo);
    if (err != FR_OK) {
      return fr_to_fs_err(err);
    }
    if (finfo.fname[0] == 0) {
        // read complete
        return 0;
    }
    info->size = finfo.fsize;
    info->type = (finfo.fattrib & AM_DIR) ? FS_TYPE_DIR : FS_TYPE_REG;
    strcpy(info->name, finfo.fname);
    // success
    return 1;
}

static int fs_fatfs_fs_stat(fs_t* fs, fs_fsinfo_t* info) {
    (void)fs;
    DWORD value = 0;

    info->impl = "fatfs";
    disk_ioctl(0, GET_SECTOR_SIZE, &value);
    info->block_size = value;

    FATFS* fatfs = NULL;
    FRESULT err = f_getfree("", &value, &fatfs);
    if (err != FR_OK) {
      return fr_to_fs_err(err);
    }
    info->block_count = fatfs->n_fatent - 2;
    info->block_used = info->block_count - value;
    info->block_count *= fatfs->csize;
    info->block_used *= fatfs->csize;

    return FS_ERR_OK;
}

void fs_fatfs_register(fs_t *fs, fatfs_port_t* port) {
   assert(fs);
   fs->port = port;

   fs->file_open_cb = fs_fatfs_file_open;
   fs->file_close_cb = fs_fatfs_file_close;
   fs->file_read_cb = fs_fatfs_file_read;
   fs->file_write_cb = fs_fatfs_file_write;
   fs->file_truncate_cb = fs_fatfs_file_truncate;
   fs->file_sync_cb = fs_fatfs_file_sync;
   fs->file_seek_cb = fs_fatfs_file_seek;
   fs->file_tell_cb = fs_fatfs_file_tell;
   fs->file_size_cb = fs_fatfs_file_size;
   fs->stat_cb = fs_fatfs_stat;
   fs->remove_cb = fs_fatfs_remove;
   fs->rename_cb = fs_fatfs_rename;
   fs->mkdir_cb = fs_fatfs_mkdir;
   fs->dir_open_cb = fs_fatfs_dir_open;
   fs->dir_close_cb = fs_fatfs_dir_close;
   fs->dir_read_cb = fs_fatfs_dir_read;
   fs->fs_stat_cb = fs_fatfs_fs_stat;
   fs->format_cb = fs_fatfs_format;

   FRESULT ret = f_mount(port->fatfs, "", 1);
   if (ret == FR_NO_FILESYSTEM) {
    ret = fs_fatfs_format(fs);
   }
   assert(ret == FR_OK);
}
