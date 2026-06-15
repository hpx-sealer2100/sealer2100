#include "lfs_port.h"

#if LFS_USE_EMMC == 1

#include "emmc.h"
#include "lfs.h"
#include "log.h"

/**
 * note: lfs v2.11.2 use EMMC very slow
 *       because lfs metadata compaction and blocks scan
 */

#define MODULE             "lfs port emmc"
#define EMMC_BLOCK_SIZE    512      // erase unit size
#define EMMC_BLOCK_COUNT   61112320 // block count

#define LFS_BLOCK_SIZE     EMMC_BLOCK_SIZE       // erase unit size
#define LFS_BLOCK_COUNT    EMMC_BLOCK_COUNT      // block count
#define LFS_PROG_SIZE      EMMC_BLOCK_SIZE       // flash smallest progm size
#define LFS_READ_SIZE      EMMC_BLOCK_SIZE       // use page size
#define LFS_CACHE_SIZE     EMMC_BLOCK_SIZE       // use page size
#define LFS_LOOKAHEAD_SIZE (1024*32)             // lookahead 32k*8 block

// static uint8_t lfs_read_buffer[LFS_READ_SIZE];
// static uint8_t lfs_prog_buffer[LFS_PROG_SIZE];
// static uint8_t lfs_lookahead_buffer[LFS_LOOKAHEAD_SIZE];

static int
lfs_read_cb(const struct lfs_config* c, lfs_block_t block, lfs_off_t off, void* buffer, lfs_size_t size) {
    (void)c;
    assert(off % LFS_READ_SIZE == 0);
    assert(size % LFS_READ_SIZE == 0);
    uint32_t count = size / LFS_READ_SIZE;
    int ret = emmc_read_blocks((uint8_t*)buffer, (uint32_t)(block), count, EMMC_TIMEOUT);
    return ret == MMC_OK ? LFS_ERR_OK : LFS_ERR_IO;
}

static int lfs_prog_cb(
    const struct lfs_config* c, lfs_block_t block, lfs_off_t off, const void* buffer, lfs_size_t size
) {
    (void)c;
    assert(off % LFS_PROG_SIZE == 0);
    assert(size % LFS_PROG_SIZE == 0);
    uint32_t count = size / LFS_PROG_SIZE;
    int ret = emmc_write_blocks((uint8_t*)buffer, (uint32_t)(block), count, EMMC_TIMEOUT);
    return ret == MMC_OK ? LFS_ERR_OK : LFS_ERR_IO;
}

static int lfs_erase_cb(const struct lfs_config* c, lfs_block_t block) {
    (void)c;
    return LFS_ERR_OK;
}

static int lfs_sync_cb(const struct lfs_config* c) {
    (void)(c);
    return 0;
}

static const struct lfs_config cfg = {
    // block device operations
    .read = lfs_read_cb,
    .prog = lfs_prog_cb,
    .erase = lfs_erase_cb,
    .sync = lfs_sync_cb,

    // block device configuration
    .read_size = LFS_READ_SIZE,
    .prog_size = LFS_PROG_SIZE,
    .block_size = LFS_BLOCK_SIZE,
    .block_count = LFS_BLOCK_COUNT,
    .cache_size = LFS_CACHE_SIZE,
    .lookahead_size = LFS_LOOKAHEAD_SIZE,
    .block_cycles = 500,

    // .read_buffer      = lfs_read_buffer,
    // .prog_buffer      = lfs_prog_buffer,
    // .lookahead_buffer = lfs_lookahead_buffer,
};

const struct lfs_config* lfs_port_emmc_config(void) {
    return &cfg;
}
#endif
