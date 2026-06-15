#include "lfs_port.h"

#if LFS_USE_NAND == 1

#include "lfs.h"
#include "log.h"
#include "nand_flash.h"

#define MODULE                  "lfs port nand"

#define LFS_BLOCK_SIZE          NAND_FLASH_BLOCK_SIZE            // erase unit size
#define LFS_BLOCK_COUNT         NAND_FLASH_BLOCK_TOTAL_COUNT     // block count
#define LFS_PROG_SIZE           NAND_FLASH_PAGE_DATA_SIZE        // flash smallest progm size
#define LFS_READ_SIZE           NAND_FLASH_PAGE_DATA_SIZE        // use page size
#define LFS_CACHE_SIZE          (NAND_FLASH_PAGE_DATA_SIZE * 8)  // use page size
#define LFS_METADATA_MAX        (NAND_FLASH_PAGE_DATA_SIZE * 4)  // metadata max, 8KB
#define LFS_LOOKAHEAD_SIZE      (LFS_BLOCK_COUNT / 8)            // lookahead all block

#define PAGE_FROM_OFFSET(off)   ((off) / NAND_FLASH_PAGE_DATA_SIZE)
#define COLUMN_FROM_OFFSET(off) ((off) % NAND_FLASH_PAGE_DATA_SIZE)

// static uint8_t lfs_read_buffer[LFS_READ_SIZE];
// static uint8_t lfs_prog_buffer[LFS_PROG_SIZE];
// static uint8_t lfs_lookahead_buffer[LFS_LOOKAHEAD_SIZE];


static int
lfs_read_cb(const struct lfs_config* c, lfs_block_t block, lfs_off_t off, void* buffer, lfs_size_t size) {
    (void)c;
    assert(off % LFS_READ_SIZE == 0);
    assert(size % LFS_READ_SIZE == 0);
    uint8_t* p = buffer;
    while (size > 0) {
        uint32_t page = PAGE_FROM_OFFSET(off);
        uint32_t column = COLUMN_FROM_OFFSET(off);
        nand_flash_read(block, page, column, p, LFS_READ_SIZE);
        p += LFS_READ_SIZE;
        off += LFS_READ_SIZE;
        size -= LFS_READ_SIZE;
    }
    return 0;
}

static int lfs_prog_cb(
    const struct lfs_config* c, lfs_block_t block, lfs_off_t off, const void* buffer, lfs_size_t size
) {
    (void)c;
    assert(off % LFS_PROG_SIZE == 0);
    assert(size % LFS_PROG_SIZE == 0);
    const uint8_t* p = buffer;
    while (size > 0) {
        uint32_t page = PAGE_FROM_OFFSET(off);
        uint32_t column = COLUMN_FROM_OFFSET(off);
        nand_flash_write(block, page, column, p, LFS_PROG_SIZE);
        p += LFS_PROG_SIZE;
        off += LFS_PROG_SIZE;
        size -= LFS_PROG_SIZE;
    }
    return 0;
}

static int lfs_erase_cb(const struct lfs_config* c, lfs_block_t block) {
    (void)c;
    nand_flash_erase(block);
    return 0;
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
    .metadata_max = LFS_METADATA_MAX,
    .inline_max = 512,
    .block_cycles = 500,

    // .read_buffer      = lfs_read_buffer,
    // .prog_buffer      = lfs_prog_buffer,
    // .lookahead_buffer = lfs_lookahead_buffer,
};

const struct lfs_config* lfs_port_nand_config(void) {
    return &cfg;
}

#endif
