#ifndef __FATFS_PORT_H__
#define __FATFS_PORT_H__

// clang-format off
  // do not change includes order
  #include "ff.h"
  #include "diskio.h"
// clang-format on

#define FATFS_USE_NAND 0
#define FATFS_USE_EMMC 1

typedef struct {
    DRESULT (*sector_count)(DWORD *count);
    DRESULT (*sector_size)(DWORD *size);
    DRESULT (*block_size)(DWORD *size);
    DRESULT (*write)(BYTE pdrv, const BYTE* buff, LBA_t sector, UINT count);
    DRESULT (*read)(BYTE pdrv, BYTE* buff, LBA_t sector, UINT count);
} fatfs_diskio_t;

typedef struct {
    FATFS *fatfs;
    const fatfs_diskio_t *diskio;
} fatfs_port_t;

fatfs_port_t* fatfs_port_init(const fatfs_diskio_t *diskio);


#if FATFS_USE_EMMC == 1
const fatfs_diskio_t* fatfs_port_emmc_diskio(void);
#endif

#endif
