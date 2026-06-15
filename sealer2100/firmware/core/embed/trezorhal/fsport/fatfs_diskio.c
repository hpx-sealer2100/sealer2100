#include "fatfs_diskio.h"

// clang-format off
#include "ff.h"

// clang-format on

static FATFS fatfs;
static fatfs_port_t fatfs_port;

// fatfs diskio
DSTATUS disk_initialize(BYTE pdrv) {
    return RES_OK;
}

DSTATUS disk_status(BYTE pdrv) {
    (void)pdrv;
    return RES_OK;
}

DRESULT disk_read(BYTE pdrv, BYTE* buff, LBA_t sector, UINT count) {
    (void)pdrv;
    return fatfs_port.diskio->read(pdrv, buff, sector, count);
}

DRESULT disk_write(BYTE pdrv, const BYTE* buff, LBA_t sector, UINT count) {
    (void)pdrv;
    return fatfs_port.diskio->write(pdrv, buff, sector, count);
}

DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void* buff) {
    (void)pdrv;
    switch (cmd) {
    case CTRL_SYNC:
        return RES_OK;
    case GET_SECTOR_COUNT:
        return fatfs_port.diskio->sector_count((DWORD*)buff);
    case GET_SECTOR_SIZE:
        return fatfs_port.diskio->sector_size((DWORD*)buff);
    case GET_BLOCK_SIZE:
        return fatfs_port.diskio->block_size((DWORD*)buff);
    default:
        return RES_PARERR;
    }
}

fatfs_port_t* fatfs_port_init(const fatfs_diskio_t *diskio) {
    fatfs_port.fatfs = &fatfs;
    fatfs_port.diskio = diskio;
    return &fatfs_port;
}
