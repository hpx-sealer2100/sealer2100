/*
 * This file is part of the TREZOR project, https://trezor.io/
 *
 * Copyright (c) SatoshiLabs
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "embed/extmod/trezorobj.h"
#include "py/mperrno.h"
#include "py/objstr.h"

// clang-format off
#include "fs.h"
#include "log.h"
// clang-format on
#define MODULE "trezorio.fs"

/// package: trezorio.fs

// clang-format off
/// FS_ERR_OK: int                  # (0) No error
/// FS_ERR_IO: int                  # (-5) Error during device operation
/// FS_ERR_CORRUPT: int             # (-84) Corrupted
/// FS_ERR_NOENT: int               # (-2) No directory entry
/// FS_ERR_EXIST: int               # (-17) Entry already exists
/// FS_ERR_NOTDIR: int              # (-20) Entry is not a dir
/// FS_ERR_ISDIR: int               # (-21) Entry is a dir
/// FS_ERR_NOTEMPTY: int            # (-39)  Dir is not empty
/// FS_ERR_BADF: int                # (-9) Bad file number
/// FS_ERR_FBIG: int                # (-27) File too large
/// FS_ERR_INVAL: int               # (-22) Invalid parameter
/// FS_ERR_NOSPC: int               # (-28) No space left on device
/// FS_ERR_NOMEM: int               # (-12) No more memory available
/// FS_ERR_NOATTR: int              # (-61) No data/attr available
/// FS_ERR_NAMETOOLONG: int         # (-36) File name too long
/// FS_ERR_NOSYS: int               # (-38) Function not implemented
// clang-format on

/// class FSError(OSError):
///     pass
MP_DEFINE_EXCEPTION(FSError, OSError)

// we use negative POSIX errno values for FSError
#define FS_ERROR_CODE(n) (n)
#define FS_ROM_INT(n) MP_ROM_INT(FS_ERROR_CODE(n))

#define FS_RAISE(exc_type, num)                                          \
  {                                                                         \
    nlr_raise(mp_obj_new_exception_arg1(                                    \
        &mp_type_##exc_type, MP_OBJ_NEW_SMALL_INT(FS_ERROR_CODE(num)))); \
  }

// we have mounted the filesystem
extern fs_t* __fs__;

static mp_obj_t fs_info_to_tuple(const fs_info_t* info) {
  mp_obj_tuple_t* tuple = MP_OBJ_TO_PTR(mp_obj_new_tuple(3, NULL));
  char attrs[] = "-----";
  if (info->type == FS_TYPE_DIR) {
    attrs[3] = 'd';
  }
  tuple->items[0] = mp_obj_new_int_from_uint(info->size);
  tuple->items[1] = mp_obj_new_str(attrs, 5);
  tuple->items[2] = mp_obj_new_str(info->name, strlen(info->name));
  return MP_OBJ_FROM_PTR(tuple);
}

/// class FSFile:
///     """
///     Class encapsulating file
///     """
typedef struct _mp_obj_FSFile_t {
  mp_obj_base_t base;
  fs_file_t fp;
} mp_obj_FSFile_t;

/// def __enter__(self) -> FSFile:
///     """
///     Return an open file object
///     """

/// from types import TracebackType
/// def __exit__(
///     self, type: type[BaseException] | None,
///     value: BaseException | None,
///     traceback: TracebackType | None,
/// ) -> None:
///     """
///     Close an open file object
///     """
STATIC mp_obj_t mod_trezorio_FSFile___exit__(size_t n_args,
                                                const mp_obj_t *args) {
  mp_obj_FSFile_t *o = MP_OBJ_TO_PTR(args[0]);
  int ret = fs_file_close(&o->fp);
  if (ret < 0) {
    FS_RAISE(FSError, ret);
  }
  return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mod_trezorio_FSFile___exit___obj,
                                           4, 4,
                                           mod_trezorio_FSFile___exit__);

/// def close(self) -> None:
///     """
///     Close an open file object
///     """
STATIC mp_obj_t mod_trezorio_FSFile_close(mp_obj_t self) {
  mp_obj_FSFile_t *o = MP_OBJ_TO_PTR(self);
  int ret = fs_file_close(&o->fp);
  if (ret < 0) {
    FS_RAISE(FSError, ret);
  }
  return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_trezorio_FSFile_close_obj,
                                 mod_trezorio_FSFile_close);

/// def read(self, data: bytearray) -> int:
///     """
///     Read data from the file
///     """
STATIC mp_obj_t mod_trezorio_FSFile_read(mp_obj_t self, mp_obj_t data) {
  mp_obj_FSFile_t *o = MP_OBJ_TO_PTR(self);
  mp_buffer_info_t buf = {0};
  mp_get_buffer_raise(data, &buf, MP_BUFFER_WRITE);
  int ret = fs_file_read(&o->fp, buf.buf, buf.len);
  if (ret < 0) {
    FS_RAISE(FSError, ret);
  }
  return mp_obj_new_int_from_uint(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mod_trezorio_FSFile_read_obj,
                                 mod_trezorio_FSFile_read);

/// def write(self, data: bytes | bytearray) -> int:
///     """
///     Write data to the file
///     """
STATIC mp_obj_t mod_trezorio_FSFile_write(mp_obj_t self, mp_obj_t data) {
  mp_obj_FSFile_t *o = MP_OBJ_TO_PTR(self);
  mp_buffer_info_t buf = {0};
  mp_get_buffer_raise(data, &buf, MP_BUFFER_READ);
  int ret = fs_file_write(&o->fp, buf.buf, buf.len);
  if (ret < 0) {
    FS_RAISE(FSError, ret);
  }
  if (ret != buf.len) {
    /* no space left on device or free clusters recorded in FSInfo fell to 0 */
    FS_RAISE(FSError, FS_ERR_NOSPC);
  }
  return mp_obj_new_int_from_uint(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mod_trezorio_FSFile_write_obj,
                                 mod_trezorio_FSFile_write);

/// def seek(self, offset: int) -> None:
///     """
///     Move file pointer of the file object
///     """
STATIC mp_obj_t mod_trezorio_FSFile_seek(mp_obj_t self, mp_obj_t offset) {
  mp_obj_FSFile_t *o = MP_OBJ_TO_PTR(self);
  int ofs = trezor_obj_get_uint(offset);
  int ret = fs_file_seek(&o->fp, ofs, FS_SEEK_SET);
  if (ret < 0) {
    FS_RAISE(FSError, ret);
  }
  return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mod_trezorio_FSFile_seek_obj,
                                 mod_trezorio_FSFile_seek);

/// def truncate(self, size: int = None) -> None:
///     """
///     Truncate the file
///     :param size: New size of the file, if not set, truncate to current position
///     """
STATIC mp_obj_t mod_trezorio_FSFile_truncate(size_t n_args, const mp_obj_t *args) {
  mp_arg_check_num(n_args, 0, 1, 2, false);
  mp_obj_FSFile_t *o = MP_OBJ_TO_PTR(args[0]);
  uint32_t size = fs_file_tell(&o->fp);
  if (n_args == 2 && mp_const_none != args[1]) {
    size = trezor_obj_get_uint(args[1]);
  }
  int ret = fs_file_truncate(&o->fp, size);
  if (ret < 0) {
    FS_RAISE(FSError, ret);
  }
  return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mod_trezorio_FSFile_truncate_obj, 1, 2,
                                 mod_trezorio_FSFile_truncate);

/// def sync(self) -> None:
///     """
///     Flush cached data of the writing file
///     """
STATIC mp_obj_t mod_trezorio_FSFile_sync(mp_obj_t self) {
  mp_obj_FSFile_t *o = MP_OBJ_TO_PTR(self);
  int ret = fs_file_sync(&o->fp);
  if (ret < 0) {
    FS_RAISE(FSError, ret);
  }
  return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_trezorio_FSFile_sync_obj,
                                 mod_trezorio_FSFile_sync);

STATIC const mp_rom_map_elem_t mod_trezorio_FSFile_locals_dict_table[] = {
    {MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&mp_identity_obj)},
    {MP_ROM_QSTR(MP_QSTR___exit__),
     MP_ROM_PTR(&mod_trezorio_FSFile___exit___obj)},
    {MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&mod_trezorio_FSFile_close_obj)},
    {MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&mod_trezorio_FSFile_read_obj)},
    {MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&mod_trezorio_FSFile_write_obj)},
    {MP_ROM_QSTR(MP_QSTR_seek), MP_ROM_PTR(&mod_trezorio_FSFile_seek_obj)},
    {MP_ROM_QSTR(MP_QSTR_truncate), MP_ROM_PTR(&mod_trezorio_FSFile_truncate_obj)},
    {MP_ROM_QSTR(MP_QSTR_sync), MP_ROM_PTR(&mod_trezorio_FSFile_sync_obj)},
};
STATIC MP_DEFINE_CONST_DICT(mod_trezorio_FSFile_locals_dict,
                            mod_trezorio_FSFile_locals_dict_table);

STATIC const mp_obj_type_t mod_trezorio_FSFile_type = {
    {&mp_type_type},
    .name = MP_QSTR_FSFile,
    .locals_dict = (void *)&mod_trezorio_FSFile_locals_dict,
};

/// class FSDir(Iterator[tuple[int, str, str]]):
///     """
///     Class encapsulating directory
///     """
typedef struct _mp_obj_FSDir_t {
  mp_obj_base_t base;
  fs_dir_t dp;
} mp_obj_FSDir_t;

/// def close(self) -> None:
///     """
///     Close the directory
///     """
STATIC mp_obj_t mod_trezorio_FSDir_close(mp_obj_t self) {
  mp_obj_FSDir_t *o = MP_OBJ_TO_PTR(self);
  fs_dir_close(&(o->dp));
  return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_trezorio_FSDir_close_obj,
                                 mod_trezorio_FSDir_close);



/// def __next__(self) -> tuple[int, str, str]:
///     """
///     Read an entry in the directory
///     """
STATIC mp_obj_t mod_trezorio_FSDir_iternext(mp_obj_t self) {
  mp_obj_FSDir_t *o = MP_OBJ_TO_PTR(self);
  fs_info_t info = {0};

  int ret = fs_dir_read(&(o->dp), &info);
  if (ret < 0) {
    fs_dir_close(&(o->dp));
    FS_RAISE(FSError, ret);
  }
  if (ret == 0) {  // stop on end of dir
    fs_dir_close(&(o->dp));
    return MP_OBJ_STOP_ITERATION;
  }
  return fs_info_to_tuple(&info);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_trezorio_FSDir_iternext_obj,
                                 mod_trezorio_FSDir_iternext);

STATIC const mp_obj_type_t mod_trezorio_FSDir_type = {
    {&mp_type_type},
    .name = MP_QSTR_FSDir,
    .getiter = mp_identity_getiter,
    .iternext = mod_trezorio_FSDir_iternext,
};

/// mock:global

/// def open(path: str, flags: str) -> FSFile:
///     """
///     Open or create a file
///     """
STATIC mp_obj_t mod_trezorio_fs_open(mp_obj_t path, mp_obj_t flags) {
  mp_buffer_info_t _path = {0}, _flags = {0};
  mp_get_buffer_raise(path, &_path, MP_BUFFER_READ);
  mp_get_buffer_raise(flags, &_flags, MP_BUFFER_READ);
  const char *mode_s = _flags.buf;

  bool r = false, w = false, x = false, a = false, p = false;
  while (*mode_s) {
    switch (*mode_s++) {
      case 'r': r = true; break;
      case 'w': w = true; break;
      case 'x': x = true; break;
      case 'a': a = true; break;
      case '+': p = true; break;
    }
  }

    // 简单冲突处理: 如果有多个主模式，按优先级取一个
    if (x + w + a + r > 1) {
        if (x)      { w = a = r = false; }
        else if (w) { x = a = r = false; }
        else if (a) { x = w = r = false; }
        else if (r) { x = w = a = false; }
    }

    int mode = 0;

    // 读写/只读/只写
    if (p) {
        mode |= FS_O_RDWR;
    } else if (w || a || x) {
        mode |= FS_O_WRONLY;
    } else { // 只有 r
        mode |= FS_O_RDONLY;
    }

    if (w) {
        // "w" / "w+"
        mode |= FS_O_CREAT | FS_O_TRUNC;
    } else if (a) {
        // "a" / "a+"
        mode |= FS_O_CREAT | FS_O_APPEND;
    } else if (x) {
        // "x" / "x+"
        mode |= FS_O_CREAT | FS_O_EXCL;
    } else if (r) {
        // "r" / "r+"
    }

  fs_file_t fp = {0};
  int res = fs_file_open(__fs__, &fp, _path.buf, mode);
  if (res < 0) {
    FS_RAISE(FSError, res);
  }
  mp_obj_FSFile_t *f = m_new_obj(mp_obj_FSFile_t);
  f->base.type = &mod_trezorio_FSFile_type;
  f->fp = fp;
  return f;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mod_trezorio_fs_open_obj,
                                 mod_trezorio_fs_open);

/// def dir_open(path: str) -> FSDir:
///     """
///     Open a directory (return generator)
///     """
STATIC mp_obj_t mod_trezorio_fs_dir_open(mp_obj_t path) {
  mp_buffer_info_t _path = {0};
  mp_get_buffer_raise(path, &_path, MP_BUFFER_READ);
  fs_dir_t dp = {0};
  int res = fs_dir_open(__fs__, &dp, _path.buf);
  if (res < 0) {
    FS_RAISE(FSError, res);
  }
  mp_obj_FSDir_t *d = m_new_obj(mp_obj_FSDir_t);
  d->base.type = &mod_trezorio_FSDir_type;
  d->dp = dp;
  return d;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_trezorio_fs_dir_open_obj,
                                 mod_trezorio_fs_dir_open);

/// def mkdir(path: str) -> None:
///     """
///     Create a sub directory
///     """
STATIC mp_obj_t mod_trezorio_fs_mkdir(mp_obj_t path) {
  mp_buffer_info_t _path = {0};
  mp_get_buffer_raise(path, &_path, MP_BUFFER_READ);
  fs_info_t info = {0};
  int ret = fs_stat(__fs__, _path.buf, &info);
  if (ret == FS_ERR_OK) {
    if (info.type == FS_TYPE_DIR) {
      return mp_const_none;
    } else {
      FS_RAISE(FSError, FS_ERR_NOTDIR);
      return mp_const_none;
    }
  }

  ret = fs_mkdir(__fs__, _path.buf);

  if (ret < 0) {
    FS_RAISE(FSError, ret);
  }
  return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_trezorio_fs_mkdir_obj, mod_trezorio_fs_mkdir);

/// def remove(path: str) -> None:
///     """
///     Delete an existing file or directory
///     """
STATIC mp_obj_t mod_trezorio_fs_remove(mp_obj_t path) {
  mp_buffer_info_t _path = {0};
  mp_get_buffer_raise(path, &_path, MP_BUFFER_READ);
  int res = fs_remove(__fs__, _path.buf);
  if (res < 0 && res != FS_ERR_NOENT) {
    FS_RAISE(FSError, res);
  }
  return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_trezorio_fs_remove_obj,
                                 mod_trezorio_fs_remove);

/// def stat(path: str) -> tuple[int, str, str]:
///     """
///     Get file status
///     """
STATIC mp_obj_t mod_trezorio_fs_stat(mp_obj_t path) {
  mp_buffer_info_t _path = {0};
  mp_get_buffer_raise(path, &_path, MP_BUFFER_READ);
  fs_info_t info = {0};
  int res = fs_stat(__fs__, _path.buf, &info);
  if (res != FS_ERR_OK) {
    FS_RAISE(FSError, res);
  }
  return fs_info_to_tuple(&info);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_trezorio_fs_stat_obj,
                                 mod_trezorio_fs_stat);

/// def rename(oldpath: str, newpath: str) -> None:
///     """
///     Rename/Move a file or directory
///     """
STATIC mp_obj_t mod_trezorio_fs_rename(mp_obj_t oldpath, mp_obj_t newpath) {
  mp_buffer_info_t _oldpath = {0}, _newpath = {0};
  mp_get_buffer_raise(oldpath, &_oldpath, MP_BUFFER_READ);
  mp_get_buffer_raise(newpath, &_newpath, MP_BUFFER_READ);
  int res = fs_rename(__fs__, _oldpath.buf, _newpath.buf);
  if (res < 0) {
    FS_RAISE(FSError, res);
  }
  return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mod_trezorio_fs_rename_obj,
                                 mod_trezorio_fs_rename);

/// def mount() -> None:
///     """
///     Mount the filesystem.
///     """
STATIC mp_obj_t mod_trezorio_fs_mount() {
  return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mod_trezorio_fs_mount_obj,
                                 mod_trezorio_fs_mount);

/// def unmount() -> None:
///     """
///     Unmount the filesystem.
///     """
STATIC mp_obj_t mod_trezorio_fs_unmount() {
  return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mod_trezorio_fs_unmount_obj,
                                 mod_trezorio_fs_unmount);

/// def is_mounted() -> bool:
///    """
///    Check if the filesystem is mounted.
///    """
STATIC mp_obj_t mod_trezorio_fs_is_mounted() {
  return mp_const_true;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mod_trezorio_fs_is_mounted_obj,
                                 mod_trezorio_fs_is_mounted);

/// def format() -> None:
///     """
///     Format the filesystem.
///     """
STATIC mp_obj_t mod_trezorio_fs_format() {
  return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mod_trezorio_fs_format_obj,
                                 mod_trezorio_fs_format);


STATIC const mp_rom_map_elem_t mod_trezorio_fs_globals_table[] = {
    {MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_fs)},
    {MP_ROM_QSTR(MP_QSTR_FSFile), MP_ROM_PTR(&mod_trezorio_FSFile_type)},
    {MP_ROM_QSTR(MP_QSTR_FSDir), MP_ROM_PTR(&mod_trezorio_FSDir_type)},
    {MP_ROM_QSTR(MP_QSTR_FSError), MP_ROM_PTR(&mp_type_FSError)},

    {MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&mod_trezorio_fs_open_obj)},
    {MP_ROM_QSTR(MP_QSTR_dir_open), MP_ROM_PTR(&mod_trezorio_fs_dir_open_obj)},
    {MP_ROM_QSTR(MP_QSTR_mkdir), MP_ROM_PTR(&mod_trezorio_fs_mkdir_obj)},
    {MP_ROM_QSTR(MP_QSTR_remove), MP_ROM_PTR(&mod_trezorio_fs_remove_obj)},
    {MP_ROM_QSTR(MP_QSTR_rename), MP_ROM_PTR(&mod_trezorio_fs_rename_obj)},
    {MP_ROM_QSTR(MP_QSTR_stat), MP_ROM_PTR(&mod_trezorio_fs_stat_obj)},
    {MP_ROM_QSTR(MP_QSTR_mount), MP_ROM_PTR(&mod_trezorio_fs_mount_obj)},
    {MP_ROM_QSTR(MP_QSTR_unmount), MP_ROM_PTR(&mod_trezorio_fs_unmount_obj)},
    {MP_ROM_QSTR(MP_QSTR_is_mounted),
     MP_ROM_PTR(&mod_trezorio_fs_is_mounted_obj)},
    {MP_ROM_QSTR(MP_QSTR_format), MP_ROM_PTR(&mod_trezorio_fs_format_obj)},

    // error codes
    {MP_ROM_QSTR(MP_QSTR_FS_ERR_OK), FS_ROM_INT(FS_ERR_OK)},
    {MP_ROM_QSTR(MP_QSTR_FS_ERR_IO), FS_ROM_INT(FS_ERR_IO)},
    {MP_ROM_QSTR(MP_QSTR_FS_ERR_CORRUPT), FS_ROM_INT(FS_ERR_CORRUPT)},
    {MP_ROM_QSTR(MP_QSTR_FS_ERR_NOENT), FS_ROM_INT(FS_ERR_NOENT)},
    {MP_ROM_QSTR(MP_QSTR_FS_ERR_EXIST), FS_ROM_INT(FS_ERR_EXIST)},
    {MP_ROM_QSTR(MP_QSTR_FS_ERR_NOTDIR), FS_ROM_INT(FS_ERR_NOTDIR)},
    {MP_ROM_QSTR(MP_QSTR_FS_ERR_ISDIR), FS_ROM_INT(FS_ERR_ISDIR)},
    {MP_ROM_QSTR(MP_QSTR_FS_ERR_NOTEMPTY), FS_ROM_INT(FS_ERR_NOTEMPTY)},
    {MP_ROM_QSTR(MP_QSTR_FS_ERR_BADF), FS_ROM_INT(FS_ERR_BADF)},
    {MP_ROM_QSTR(MP_QSTR_FS_ERR_FBIG), FS_ROM_INT(FS_ERR_FBIG)},
    {MP_ROM_QSTR(MP_QSTR_FS_ERR_INVAL), FS_ROM_INT(FS_ERR_INVAL)},
    {MP_ROM_QSTR(MP_QSTR_FS_ERR_NOSPC), FS_ROM_INT(FS_ERR_NOSPC)},
    {MP_ROM_QSTR(MP_QSTR_FS_ERR_NOMEM), FS_ROM_INT(FS_ERR_NOMEM)},
    {MP_ROM_QSTR(MP_QSTR_FS_ERR_NOATTR), FS_ROM_INT(FS_ERR_NOATTR)},
    {MP_ROM_QSTR(MP_QSTR_FS_ERR_NAMETOOLONG), FS_ROM_INT(FS_ERR_NAMETOOLONG)},
    {MP_ROM_QSTR(MP_QSTR_FS_ERR_NOSYS), FS_ROM_INT(FS_ERR_NOSYS)},
};
STATIC MP_DEFINE_CONST_DICT(mod_trezorio_fs_globals,
                            mod_trezorio_fs_globals_table);

STATIC const mp_obj_module_t mod_trezorio_fs_module = {
    .base = {&mp_type_module},
    .globals = (mp_obj_dict_t *)&mod_trezorio_fs_globals,
};
