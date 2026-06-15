#include "uart_log.h"

#include <stdint.h>

#include "usbd_def.h"
#include "usbd_msc.h"
#include "nand_flash.h"

#include "log.h"

#define MODULE "nand msc"

// log control
#define ENABLE_NAND_MSC_LOG 0
#define NAND_MSC_LOG_DETAIL 0

#if !ENABLE_NAND_MSC_LOG
// disable log
#undef LOG_DEBUG
#undef LOG_HEXDUMP_DEBUG
#define LOG_DEBUG(module, fmt, ...)                log_dummy(module, fmt, ##__VA_ARGS__)
#define LOG_HEXDUMP_DEBUG(module, label, buf, len) log_dummy(module, label, buf, len)
#endif

#define STORAGE_LUN_NBR 1U
// use nand page for BLK_SIZE
#define STORAGE_BLK_SIZ NAND_FLASH_PAGE_DATA_SIZE
#define STORAGE_BLK_NBR (NAND_FLASH_PAGE_COUNT)

static int8_t STORAGE_Init(uint8_t lun);
static int8_t STORAGE_GetCapacity(uint8_t lun, uint32_t* block_num, uint32_t* block_size);
static int8_t STORAGE_IsReady(uint8_t lun);
static int8_t STORAGE_IsWriteProtected(uint8_t lun);
static int8_t STORAGE_Read(uint8_t lun, uint8_t* buf, uint32_t blk_addr, uint32_t blk_len);
static int8_t STORAGE_Write(uint8_t lun, uint8_t* buf, uint32_t blk_addr, uint32_t blk_len);
static int8_t STORAGE_Erase(uint8_t lun, uint32_t block);
static int8_t STORAGE_GetMaxLun(void);

/* USB Mass storage Standard Inquiry Data */
// clang-format off
static int8_t STORAGE_Inquirydata[] = /* 36 */
{

  /* LUN 0 */
  0x00,
  0x80,
  0x02,
  0x02,
  (STANDARD_INQUIRY_DATA_LEN - 5),
  0x00,
  0x00,
  0x00,
  'Z', 'A', 'C', 'u', 'b', 'e', ' ', ' ', /* Manufacturer : 8 bytes */
  'B', 'o', 'a', 'r', 'd', 'l', 'o', 'a', /* Product      : 16 Bytes */
  'd', 'e', 'r', ' ', ' ', ' ', ' ', ' ',
  '2', '.', '0', '0',                     /* Version      : 4 Bytes */
};

USBD_StorageTypeDef USBD_nand_fops = {
    STORAGE_Init,
    STORAGE_GetCapacity,
    STORAGE_IsReady,
    STORAGE_IsWriteProtected,
    STORAGE_Read,
    STORAGE_Write,
    STORAGE_Erase,
    STORAGE_GetMaxLun,
    STORAGE_Inquirydata,

};
// clang-formate on

int8_t STORAGE_Init(uint8_t lun) {

    return (0);
}

int8_t STORAGE_GetCapacity(uint8_t lun, uint32_t* block_num, uint32_t* block_size) {
    *block_num = STORAGE_BLK_NBR;
    *block_size = STORAGE_BLK_SIZ;
    return (0);
}

int8_t STORAGE_IsReady(uint8_t lun) {
    return (0);
}

int8_t STORAGE_IsWriteProtected(uint8_t lun) {
    return 0;
}

int8_t STORAGE_Read(uint8_t lun, uint8_t* buf, uint32_t blk_addr, uint32_t blk_len) {
    // read blocks
    LOG_DEBUG(MODULE, "msc read blocks from %d, count: %d, bytes: %d", blk_addr, blk_len, blk_len * STORAGE_BLK_SIZ);
    {
        uint32_t block = blk_addr / NAND_FLASH_PAGE_PER_BLOCK_COUNT;
        uint32_t page = blk_addr % NAND_FLASH_PAGE_PER_BLOCK_COUNT;
        LOG_DEBUG(MODULE, "msc read nand block: %d, from page: %d, count: %d", block, page, blk_len);
    }

    while (blk_len) {
        uint32_t block = blk_addr / NAND_FLASH_PAGE_PER_BLOCK_COUNT;
        uint32_t page = blk_addr % NAND_FLASH_PAGE_PER_BLOCK_COUNT;
#if NAND_MSC_LOG_DETAIL
        if (NAND_MSC_LOG_DETAIL) {
            LOG_DEBUG(MODULE, "nand read block: %d, page: %d", block, page);
        }
        uart_log_flush();
#endif
        nand_flash_read(block, page, 0, buf, STORAGE_BLK_SIZ);
        buf += STORAGE_BLK_SIZ;
        blk_addr += 1;
        blk_len -= 1;
    }

    return 0;
}
int8_t STORAGE_Write(uint8_t lun, uint8_t* buf, uint32_t blk_addr, uint32_t blk_len) {
    // write blocks
    LOG_DEBUG(MODULE, "msc write bulk blocks from %d, count: %d, bytes: %d", blk_addr, blk_len, blk_len * STORAGE_BLK_SIZ);
    {
        uint32_t block = blk_addr / NAND_FLASH_PAGE_PER_BLOCK_COUNT;
        uint32_t page = blk_addr % NAND_FLASH_PAGE_PER_BLOCK_COUNT;
        LOG_DEBUG(MODULE, "msc write nand block: %d, from page: %d, count: %d", block, page, blk_len);
    }

    while (blk_len) {
        uint32_t block = blk_addr / NAND_FLASH_PAGE_PER_BLOCK_COUNT;
        uint32_t page = blk_addr % NAND_FLASH_PAGE_PER_BLOCK_COUNT;
#if NAND_MSC_LOG_DETAIL
        if (NAND_MSC_LOG_DETAIL) {
            LOG_DEBUG(MODULE, "nand write block: %d, page: %d", block, page);
        }
        uart_log_flush();
#endif
        nand_flash_write(block, page, 0, buf, STORAGE_BLK_SIZ);
        buf += STORAGE_BLK_SIZ;
        blk_addr += 1;
        blk_len -= 1;
    }

    return 0;
}

int8_t STORAGE_Erase(uint8_t lun, uint32_t block) {
    LOG_DEBUG(MODULE, "nand flash erase block %d", block);
    return nand_flash_erase(block);
}
int8_t STORAGE_GetMaxLun(void) {
    return (STORAGE_LUN_NBR - 1);
}
