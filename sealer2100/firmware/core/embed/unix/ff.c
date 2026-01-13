/// use posix api implement fatfs api
#include "ff.h"

// undef `DIR` symbol for posix api
#undef DIR

#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>

#include "profile.h"


// use `core/src/trezor` directly
#define SYS_DATA_DIR CURRENT_DIR RES_DIR
#define USER_DATA_DIR profile_user_data_path()

/* Character code support macros */
#define IsUpper(c)		((c) >= 'A' && (c) <= 'Z')
#define IsLower(c)		((c) >= 'a' && (c) <= 'z')
#define IsDigit(c)		((c) >= '0' && (c) <= '9')
#define IsSeparator(c)	((c) == '/' || (c) == '\\')
#define IsTerminator(c)	((UINT)(c) < (FF_USE_LFN ? ' ' : '!'))
#define IsSurrogate(c)	((c) >= 0xD800 && (c) <= 0xDFFF)
#define IsSurrogateH(c)	((c) >= 0xD800 && (c) <= 0xDBFF)
#define IsSurrogateL(c)	((c) >= 0xDC00 && (c) <= 0xDFFF)

static FATFS* FatFs[FF_VOLUMES] = {0};	/* Pointer to the filesystem objects (logical drives) */

static int get_ldnumber (	/* Returns logical drive number (-1:invalid drive number or null pointer) */
	const TCHAR** path		/* Pointer to pointer to the path name */
)
{
	const TCHAR *tp, *tt;
	TCHAR tc;
	int i;
	int vol = -1;
#if FF_STR_VOLUME_ID		/* Find string volume ID */
	const char *sp;
	char c;
#endif

	tt = tp = *path;
	if (!tp) return vol;	/* Invalid path name? */
	do tc = *tt++; while (!IsTerminator(tc) && tc != ':');	/* Find a colon in the path */

	if (tc == ':') {	/* DOS/Windows style volume ID? */
		i = FF_VOLUMES;
		if (IsDigit(*tp) && tp + 2 == tt) {	/* Is there a numeric volume ID + colon? */
			i = (int)*tp - '0';	/* Get the LD number */
		}
#if FF_STR_VOLUME_ID == 1	/* Arbitrary string is enabled */
		else {
			i = 0;
			do {
				sp = VolumeStr[i]; tp = *path;	/* This string volume ID and path name */
				do {	/* Compare the volume ID with path name */
					c = *sp++; tc = *tp++;
					if (IsLower(c)) c -= 0x20;
					if (IsLower(tc)) tc -= 0x20;
				} while (c && (TCHAR)c == tc);
			} while ((c || tp != tt) && ++i < FF_VOLUMES);	/* Repeat for each id until pattern match */
		}
#endif
		if (i < FF_VOLUMES) {	/* If a volume ID is found, get the drive number and strip it */
			vol = i;		/* Drive number */
			*path = tt;		/* Snip the drive prefix off */
		}
		return vol;
	}
#if FF_STR_VOLUME_ID == 2		/* Unix style volume ID is enabled */
	if (*tp == '/') {			/* Is there a volume ID? */
		while (*(tp + 1) == '/') tp++;	/* Skip duplicated separator */
		i = 0;
		do {
			tt = tp; sp = VolumeStr[i]; /* Path name and this string volume ID */
			do {	/* Compare the volume ID with path name */
				c = *sp++; tc = *(++tt);
				if (IsLower(c)) c -= 0x20;
				if (IsLower(tc)) tc -= 0x20;
			} while (c && (TCHAR)c == tc);
		} while ((c || (tc != '/' && !IsTerminator(tc))) && ++i < FF_VOLUMES);	/* Repeat for each ID until pattern match */
		if (i < FF_VOLUMES) {	/* If a volume ID is found, get the drive number and strip it */
			vol = i;		/* Drive number */
			*path = tt;		/* Snip the drive prefix off */
		}
		return vol;
	}
#endif
	/* No drive prefix is found */
#if FF_FS_RPATH != 0
	vol = CurrVol;	/* Default drive is current drive */
#else
	vol = 0;		/* Default drive is 0 */
#endif
	return vol;		/* Return the default drive */
}

static void get_full_path(int vol, const TCHAR* path, char* full_path) {
    while (IsSeparator(*path)) path++;	/* Strip separators */
    if (vol == 0) {
        // the system data vol
        strcpy(full_path, SYS_DATA_DIR);
        strcat(full_path, "/");
        strcat(full_path, path);
    } else if (vol == 1) {
        // the user data vol
        strcpy(full_path, USER_DATA_DIR);
        strcat(full_path, "/");
        strcat(full_path, path);
    }
}

FRESULT f_open (FIL* fp, const TCHAR* path, BYTE mode) {
    int vol = get_ldnumber(&path);
    if (vol < 0) return FR_INVALID_DRIVE;
    char full_path[FF_MAX_LFN];
    get_full_path(vol, path, full_path);
    int flag = 0;
    if(mode & FA_READ) flag |= O_RDONLY;
    if(mode & FA_WRITE) flag |= O_WRONLY;
    if(mode & FA_CREATE_NEW) flag |= O_CREAT;
    if(mode & FA_CREATE_ALWAYS) flag |= O_CREAT | O_TRUNC;
    fp->fd = open(full_path, flag, 0644);

    return fp->fd >= 0 ? FR_OK : FR_NO_FILE;
}
FRESULT f_close (FIL* fp) {
    close(fp->fd);
    return FR_OK;
}
FRESULT f_read (FIL* fp, void* buff, UINT btr, UINT* br) {
    ssize_t r = read(fp->fd, buff, btr);
    if (r < 0) return FR_DISK_ERR;
    *br = (UINT)r;
    return FR_OK;
}
FRESULT f_write (FIL* fp, const void* buff, UINT btw, UINT* bw) {
    ssize_t r = write(fp->fd, buff, btw);
    if (r < 0) return FR_DISK_ERR;
    *bw = (UINT)r;
    return FR_OK;
}
FRESULT f_lseek (FIL* fp, FSIZE_t ofs) {
    return lseek(fp->fd, ofs, SEEK_SET) < 0 ? FR_DISK_ERR : FR_OK;
}
FRESULT f_truncate (FIL* fp) {
    f_lseek(fp, 0);
    ftruncate(fp->fd, 0);
    return FR_OK;
}
FRESULT f_sync (FIL* fp) {
    return FR_OK;
}

FRESULT f_opendir (struct FAT_DIR* dp, const TCHAR* path) {
    int vol = get_ldnumber(&path);
    if (vol < 0) return FR_INVALID_DRIVE;
    char full_path[FF_MAX_LFN];
    get_full_path(vol, path, full_path);
    dp->dp = opendir(full_path);
    if (!dp->dp) return FR_NO_FILE;
    // cache vol and path
    dp->vol = vol;
    strcpy(dp->path, path);
    return FR_OK;
}
FRESULT f_closedir (struct FAT_DIR* dp) {
    closedir(dp->dp);
    // clear cache dir
    dp->dp = NULL;
    dp->vol = -1;
    memset(dp->path, 0, sizeof(dp->path));
    return FR_OK;
}
FRESULT f_readdir (struct FAT_DIR* dp, FILINFO* fno) {
    struct dirent *entry;
    while ((entry = readdir(dp->dp)) != NULL) {
        // skip `..` and `.`
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        char full_path[FF_MAX_LFN] = {0};
        get_full_path(dp->vol, dp->path, full_path);
        if (full_path[strlen(full_path) - 1] != '/') {
            strcat(full_path, "/");
        }
        strcat(full_path, entry->d_name);

        struct stat st;
        if (stat(full_path, &st) < 0) {
            return FR_DISK_ERR;
        }
        fno->fsize = st.st_size;
        if (S_ISDIR(st.st_mode)) {
            fno->fattrib |= AM_DIR;
        }
        if (entry->d_name[0] == '.') {
            fno->fattrib |= AM_HID;
        }
        if ((st.st_mode & S_IXUSR) && !(st.st_mode & S_IWUSR)) {
            fno->fattrib |= AM_RDO;
        }
        strcpy(fno->fname, entry->d_name);

        break;
    }

    return FR_OK;
}
// FRESULT f_findfirst (DIR* dp, FILINFO* fno, const TCHAR* path, const TCHAR* pattern);	/* Find first file */
// FRESULT f_findnext (DIR* dp, FILINFO* fno);							/* Find next file */
FRESULT f_mkdir (const TCHAR* path){
    int vol = get_ldnumber(&path);
    if (vol < 0) return FR_INVALID_DRIVE;
    char full_path[FF_MAX_LFN];
    get_full_path(vol, path, full_path);
    if (access(full_path, F_OK) == 0) {
        return FR_EXIST;
    }
    return mkdir(full_path, 0755) < 0 ? FR_DISK_ERR : FR_OK;
}
FRESULT f_unlink (const TCHAR* path) {
    int vol = get_ldnumber(&path);
    if (vol < 0) return FR_INVALID_DRIVE;
    char full_path[FF_MAX_LFN];
    get_full_path(vol, path, full_path);
    return unlink(full_path) < 0 ? FR_DISK_ERR : FR_OK;
}
FRESULT f_rename (const TCHAR* path_old, const TCHAR* path_new) {
    int vol = get_ldnumber(&path_old);
    if (vol < 0) return FR_INVALID_DRIVE;
    char full_path_old[FF_MAX_LFN];
    get_full_path(vol, path_old, full_path_old);
    char full_path_new[FF_MAX_LFN];
    get_full_path(vol, path_new, full_path_new);
    return rename(full_path_old, full_path_new) < 0 ? FR_DISK_ERR : FR_OK;
}
FRESULT f_stat (const TCHAR* path, FILINFO* fno) {
    int vol = get_ldnumber(&path);
    if (vol < 0) return FR_INVALID_DRIVE;
    char full_path[FF_MAX_LFN];
    get_full_path(vol, path, full_path);
    struct stat st;
    if (stat(full_path, &st) < 0) {
        return FR_DISK_ERR;
    }
    fno->fsize = st.st_size;
    if (S_ISDIR(st.st_mode)) {
        fno->fattrib |= AM_DIR;
    }
    if (path[0] == '.') {
        fno->fattrib |= AM_HID;
    }
    if ((st.st_mode & S_IXUSR) && !(st.st_mode & S_IWUSR)) {
        fno->fattrib |= AM_RDO;
    }
    return FR_OK;
}
// FRESULT f_chmod (const TCHAR* path, BYTE attr, BYTE mask);			/* Change attribute of a file/dir */
// FRESULT f_utime (const TCHAR* path, const FILINFO* fno);			/* Change timestamp of a file/dir */
// FRESULT f_chdir (const TCHAR* path);								/* Change current directory */
// FRESULT f_chdrive (const TCHAR* path);								/* Change current drive */
// FRESULT f_getcwd (TCHAR* buff, UINT len);							/* Get current directory */
// FRESULT f_getfree (const TCHAR* path, DWORD* nclst, FATFS** fatfs);	/* Get number of free clusters on the drive */
// FRESULT f_getlabel (const TCHAR* path, TCHAR* label, DWORD* vsn);	/* Get volume label */
FRESULT f_setlabel (const TCHAR* label){
    return FR_OK;
}
// FRESULT f_forward (FIL* fp, UINT(*func)(const BYTE*,UINT), UINT btf, UINT* bf);	/* Forward data to the stream */
// FRESULT f_expand (FIL* fp, FSIZE_t fsz, BYTE opt);					/* Allocate a contiguous block to the file */
FRESULT f_mount (FATFS* fs, const TCHAR* path, BYTE opt) {
    int vol = get_ldnumber(&path);
    if (vol < 0) return FR_INVALID_DRIVE;

    // mp fatfs module use `fs_type` for test mount state
    fs->fs_type = 1;
    // register fs
    FatFs[vol] = fs;
    // 0: system data
    if (vol == 0) return FR_OK;

    // 1: user data
    const char* user_data_path = profile_user_data_path();
    // check `user_data_path` dir, if not exist, create it
    if (access(user_data_path, F_OK) != 0) {
        if (mkdir(user_data_path, 0755) != 0) {
            fs->fs_type = 0;
            return FR_DISK_ERR;
        }
    }

    return FR_OK;
}
FRESULT f_mkfs (const TCHAR* path, const MKFS_PARM* opt, void* work, UINT len){
    return FR_OK;
}
// FRESULT f_fdisk (BYTE pdrv, const LBA_t ptbl[], void* work);		/* Divide a physical drive into some partitions */
// FRESULT f_setcp (WORD cp);											/* Set current code page */
// int f_putc (TCHAR c, FIL* fp);										/* Put a character to the file */
// int f_puts (const TCHAR* str, FIL* cp);								/* Put a string to the file */
// int f_printf (FIL* fp, const TCHAR* str, ...);						/* Put a formatted string to the file */
// TCHAR* f_gets (TCHAR* buff, int len, FIL* fp);						/* Get a string from the file */

FSIZE_t f_tell(FIL *fp) {
    return lseek(fp->fd, 0, SEEK_CUR);
}

FSIZE_t f_size(FIL *fp) {
    struct stat st;
    fstat(fp->fd, &st);
    return st.st_size;
}