#ifndef __FS_H__
#define __FS_H__

#include <stdbool.h>
#include <stdint.h>

#ifndef FS_USE_LFS
#define FS_USE_LFS 1
#endif
#ifndef FS_USE_FATFS
#define FS_USE_FATFS 1
#endif

#if FS_USE_LFS == 1
#include "fsport/lfs_port.h"
#endif
#if FS_USE_FATFS == 1
#include "fsport/fatfs_diskio.h"
#endif

#define FS_NAME_MAX 256

#define FS_DO_CHECK_PARAM_NULL(param) \
 do { \
    if (!(param)) { \
        return FS_ERR_INVAL; \
    } \
 } while (0)

 #define FS_DO_CHECK_PARAM(condition) \
 do { \
    if (!(condition)) { \
        return FS_ERR_INVAL; \
    } \
 } while (0)

 #define FS_DO_CHECK_MEMORY(ptr) \
 do { \
    if (!(ptr)) { \
        return FS_ERR_NOMEM; \
    } \
 } while (0)

#define FS_DO_CHECK_IMPLEMENTED(fs, func) \
 do { \
    if (!(fs)->func) { \
        return FS_ERR_NOSYS; \
    } \
 } while (0)

typedef enum {
    FS_ERR_OK = 0,            // No error
    FS_ERR_IO = -5,           // Error during device operation
    FS_ERR_CORRUPT = -84,     // Corrupted
    FS_ERR_NOENT = -2,        // No directory entry
    FS_ERR_EXIST = -17,       // Entry already exists
    FS_ERR_NOTDIR = -20,      // Entry is not a dir
    FS_ERR_ISDIR = -21,       // Entry is a dir
    FS_ERR_NOTEMPTY = -39,    // Dir is not empty
    FS_ERR_BADF = -9,         // Bad file number
    FS_ERR_FBIG = -27,        // File too large
    FS_ERR_INVAL = -22,       // Invalid parameter
    FS_ERR_NOSPC = -28,       // No space left on device
    FS_ERR_NOMEM = -12,       // No more memory available
    FS_ERR_NOATTR = -61,      // No data/attr available
    FS_ERR_NAMETOOLONG = -36, // File name too long
    FS_ERR_NOSYS = -38,       // Function not implemented
} fs_error_t;

// File types.
typedef enum {
    FS_TYPE_REG = 0x01, // Regular file
    FS_TYPE_DIR = 0x02, // Directory
} fs_type_t;

// File info.
typedef struct {
    fs_type_t type;             // File type
    uint32_t size;              // File size
    char name[FS_NAME_MAX + 1]; // File name
} fs_info_t;

// File system info.
typedef struct {
    const char* impl;     // File system implementation
    uint32_t block_size;  // Block size
    uint64_t block_count; // Total blocks
    uint64_t block_used;  // Used blocks
} fs_fsinfo_t;

// File open flags.
enum fs_open_flags {
    FS_O_RDONLY = 1,      // Open a file as read only
    FS_O_WRONLY = 2,      // Open a file as write only
    FS_O_RDWR = 3,        // Open a file as read and write
    FS_O_CREAT = 0x0100,  // Create a file if it does not exist
    FS_O_EXCL = 0x0200,   // Fail if a file already exists
    FS_O_TRUNC = 0x0400,  // Truncate the existing file to zero size
    FS_O_APPEND = 0x0800, // Move to end of file on every write
};

// Seek modes.
typedef enum {
    FS_SEEK_SET = 0x00, // Set the position from absolutely (from the start of file)
    FS_SEEK_CUR = 0x01, // Set the position from the current position
    FS_SEEK_END = 0x02, // Set the position from the end of the file
} fs_whence_t;

typedef struct _fs_t fs_t;

// File descriptor.
typedef struct {
    fs_t* fs; // File system
    void* fd; // File descriptor in port file system
} fs_file_t;

// Directory descriptor.
typedef struct {
    fs_t* fs; // File system
    void* dd; // Directory descriptor in port file system
} fs_dir_t;

/**
 * File operation methods.
 */

// Open a file
//
// The mode that the file is opened in is determined by the flags, which
// are values from the enum fs_open_flags that are bitwise-ored together.
//
// Returns a negative error code on failure.
typedef int (*fs_file_open_cb_t)(fs_file_t* file, const char* path, int flags);

// Close a file
//
// Returns a negative error code on failure.
typedef int (*fs_file_close_cb_t)(fs_file_t* file);

// Read from a file
//
// Returns a negative error code on failure.
typedef int (*fs_file_read_cb_t)(fs_file_t* file, void* buf, uint32_t size);

// Write to a file
//
// Returns a negative error code on failure.
typedef int (*fs_file_write_cb_t)(fs_file_t* file, const void* buf, uint32_t size);

// Truncate a file
//
// Returns a negative error code on failure.
typedef int (*fs_file_truncate_cb_t)(fs_file_t* file, uint32_t size);

// Sync a file
//
// Returns a negative error code on failure.
typedef int (*fs_file_sync_cb_t)(fs_file_t* file);

// Seek a file
//
// Returns a negative error code on failure.
typedef int (*fs_file_seek_cb_t)(fs_file_t* file, int pos, fs_whence_t whence);

// Tell the current position of a file
//
// Returns a negative error code on failure.
typedef int (*fs_file_tell_cb_t)(fs_file_t* file);

// Tell the size of a file
//
// Returns a negative error code on failure.
typedef int (*fs_file_size_cb_t)(fs_file_t* file);

// File or directory info
//
// Returns a negative error code on failure.
typedef int (*fs_stat_cb_t)(fs_t* fs, const char* path, fs_info_t* info);

// Remove a file or directory
//
// If removing a directory, the directory must be empty.
// Returns a negative error code on failure.
typedef int (*fs_remove_cb_t)(fs_t* fs, const char* path);

// Rename or move a file or directory
//
// Returns a negative error code on failure.
typedef int (*fs_rename_cb_t)(fs_t* fs, const char* old_path, const char* new_path);

// Make a directory
//
// Returns a negative error code on failure.
typedef int (*fs_mkdir_cb_t)(fs_t* fs, const char* path);

// Open a directory
//
// Returns a negative error code on failure.
typedef int (*fs_dir_open_cb_t)(fs_dir_t* dir, const char* path);

// Close a directory
//
// Returns a negative error code on failure.
typedef int (*fs_dir_close_cb_t)(fs_dir_t* dir);

// Read a directory entry
//
// Returns a negative error code on failure.
typedef int (*fs_dir_read_cb_t)(fs_dir_t* dir, fs_info_t* info);

// Get file system info
//
// Returns a negative error code on failure.
typedef int (*fs_fs_stat_cb_t)(fs_t* fs, fs_fsinfo_t* info);

// Format a file system
//
// Returns a negative error code on failure.
typedef int (*fs_format_cb_t)(fs_t* fs);

// File system driver.
struct _fs_t {
    char letter; // File system letter
    fs_file_open_cb_t file_open_cb;
    fs_file_close_cb_t file_close_cb;
    fs_file_read_cb_t file_read_cb;
    fs_file_write_cb_t file_write_cb;
    fs_file_truncate_cb_t file_truncate_cb;
    fs_file_sync_cb_t file_sync_cb;
    fs_file_seek_cb_t file_seek_cb;
    fs_file_tell_cb_t file_tell_cb;
    fs_file_size_cb_t file_size_cb;
    fs_stat_cb_t stat_cb;
    fs_remove_cb_t remove_cb;
    fs_rename_cb_t rename_cb;
    fs_mkdir_cb_t mkdir_cb;
    fs_dir_open_cb_t dir_open_cb;
    fs_dir_close_cb_t dir_close_cb;
    fs_dir_read_cb_t dir_read_cb;
    fs_fs_stat_cb_t fs_stat_cb;
    fs_format_cb_t format_cb;

    void* port;  // the port file system context pointer
};

#if FS_USE_LFS == 1
// Register lfs file system driver.
//
void fs_lfs_register(fs_t* fs, lfs_port_t* port);
#endif

#if FS_USE_FATFS == 1
// Register fatfs file system driver.
//
void fs_fatfs_register(fs_t* fs, fatfs_port_t* port);
#endif



// Initialize a file system driver with default values.
//
void fs_init(fs_t* fs);

// Open a file
//
// Returns a negative error code on failure.
int fs_file_open(fs_t* fs, fs_file_t* file, const char* path, int flags);

// Close a file
//
// Returns a negative error code on failure.
int fs_file_close(fs_file_t* file);

// Read from a file
//
// Returns a negative error code on failure.
int fs_file_read(fs_file_t* file, void* buf, uint32_t size);

// Write to a file
//
// Returns a negative error code on failure.
int fs_file_write(fs_file_t* file, const void* buf, uint32_t size);

// Truncate a file
//
// Returns a negative error code on failure.
int fs_file_truncate(fs_file_t* file, uint32_t size);

// Sync a file
//
// Returns a negative error code on failure.
int fs_file_sync(fs_file_t* file);

// Seek a file
//
// The change in position is determined by the offset and whence flag.
// Returns the new position of the file, or a negative error code on failure.
int fs_file_seek(fs_file_t* file, int pos, fs_whence_t whence);

// Tell the current position of a file
//
// Equivalent to fs_file_seek(file, 0, LV_FS_SEEK_CUR)
// Returns the position of the file, or a negative error code on failure.
int fs_file_tell(fs_file_t* file);

// Tell the size of a file
//
// Similar to fs_file_seek(file, 0, LV_FS_SEEK_END)
// Returns the size of the file, or a negative error code on failure.
int fs_file_size(fs_file_t* file);

// Get file or directory info
//
// Returns a negative error code on failure.
int fs_stat(fs_t* fs, const char* path, fs_info_t* info);

// Remove a file or directory
//
// If removing a directory, the directory must be empty.
// Returns a negative error code on failure.
int fs_remove(fs_t* fs, const char* path);

// Rename or move a file or directory
//
// Returns a negative error code on failure.
int fs_rename(fs_t* fs, const char* old_path, const char* new_path);

// Make a directory
//
// Returns a negative error code on failure.
int fs_mkdir(fs_t* fs, const char* path);

// Open a directory
//
// Returns a negative error code on failure.
int fs_dir_open(fs_t* fs, fs_dir_t* dir, const char* path);

// Close a directory
//
// Returns a negative error code on failure.
int fs_dir_close(fs_dir_t* dir);

// Read a directory entry
//
// Returns a negative error code on failure.
int fs_dir_read(fs_dir_t* dir, fs_info_t* info);

// File system level info
//
// Returns a negative error code on failure.
int fs_fs_stat(fs_t* fs, fs_fsinfo_t* info);

// Format a file system
//
// Returns a negative error code on failure.
int fs_format(fs_t* fs);

// Get the extension of a filename
//
// Returns a pointer to the beginning of the extension in the filename.
const char* fs_get_ext(const char* name);

// Get the directory name of a path (e.g. U:/folder/file -> U:/folder)
//
// Note: This function will truncate the path to the directory name.
// Returns a pointer to the beginning of the directory name in the path.
char* fs_dir_name(char* path);

// Get the last element of a path (e.g. U:/folder/file -> file)
//
// Returns a pointer to the beginning of the last element in the path.
const char* fs_base_name(const char* path);

// Join multiple paths into a single path
//
// Returns a negative error code on failure.
void fs_path_join(char* dst, const char* path, ...);

// Check if a file or directory exists
//
// Returns true if the file or directory exists, false otherwise.
static inline bool fs_is_exists(fs_t* fs, const char* path) {
    fs_info_t info;
    return fs_stat(fs, path, &info) == 0;
}

// Check if a file exists
//
// Returns true if the file exists, false otherwise.
static inline bool fs_is_file(fs_t* fs, const char* path) {
    fs_info_t info;
    return fs_stat(fs, path, &info) == 0 && info.type == FS_TYPE_REG;
}

// Check if a directory exists
//
// Returns true if the directory exists, false otherwise.
static inline bool fs_is_dir(fs_t* fs, const char* path) {
    fs_info_t info;
    return fs_stat(fs, path, &info) == 0 && info.type == FS_TYPE_DIR;
}

// tests functions

void fs_test_list_all_files(fs_t* fs);
void fs_speed_test(fs_t* fs);
#endif
