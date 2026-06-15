#include "fatfs_diskio.h"

#include "emmc.h"

static DRESULT emmc_sector_count(DWORD *count) {
    EMMC_CardInfoTypeDef card_info = {0};
    emmc_get_card_info(&card_info);
    *count = card_info.LogBlockNbr;
    return RES_OK;
}
static DRESULT emmc_sector_size(DWORD *size) {
    EMMC_CardInfoTypeDef card_info = {0};
    emmc_get_card_info(&card_info);
    *size = card_info.LogBlockSize;
    return RES_OK;
}
static DRESULT emmc_block_size(DWORD *size) {
    *size = 1;
    return RES_OK;
}

static DRESULT emmc_read(BYTE pdrv, BYTE* buff, LBA_t sector, UINT count) {
    (void)pdrv;
    DRESULT res = RES_ERROR;

    if (emmc_read_blocks((uint8_t*)buff, (uint32_t)(sector), count, EMMC_TIMEOUT) == MMC_OK) {
        res = RES_OK;
    }
    return res;
}

static DRESULT emmc_write(BYTE pdrv, const BYTE* buff, LBA_t sector, UINT count) {
    (void)pdrv;
    DRESULT res = RES_ERROR;

    if (emmc_write_blocks((uint8_t*)buff, (uint32_t)(sector), count, EMMC_TIMEOUT) == MMC_OK) {
        res = RES_OK;
    }
    return res;
}

static const fatfs_diskio_t emmc_diskio = {
    .read = emmc_read,
    .write = emmc_write,
    .sector_count = emmc_sector_count,
    .sector_size = emmc_sector_size,
    .block_size = emmc_block_size,
};

const fatfs_diskio_t* fatfs_port_emmc_diskio(void) {
    return &emmc_diskio;
}
