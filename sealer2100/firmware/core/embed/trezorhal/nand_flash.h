#ifndef __NAND_FLASH_H__
#define __NAND_FLASH_H__

#include "stdint.h"
#include <stdint.h>

#define NAND_FLASH_ECC_ON 1

/**
 * Note: only for test. This code is use NAND without FTL at now.
 * TODO: FTL
 */

/**
 * Description Requirement
 * Minimum number of valid blocks (NVB) 4016
 * Total available blocks per die 4096
 * First spare area location Byte 2048
 * Bad-block mark 00h(use non FFh to check)
 */
#define NAND_FLASH_BLOCK_TOTAL_COUNT       4096
#define NAND_FLASH_VALID_BLOCK_COUNT       4016
#define NAND_FLASH_MAX_INVALID_BLOCK_COUNT 80
#define NAND_FLASH_PAGE_PER_BLOCK_COUNT    64
#define NAND_FLASH_PAGE_COUNT              (NAND_FLASH_BLOCK_TOTAL_COUNT * NAND_FLASH_PAGE_PER_BLOCK_COUNT)

#define NAND_FLASH_PAGE_DATA_SIZE          2048
#define NAND_FLASH_PAGE_SIZE               (NAND_FLASH_PAGE_DATA_SIZE + 128)
#define NAND_FLASH_BLOCK_SIZE              (NAND_FLASH_PAGE_PER_BLOCK_COUNT * NAND_FLASH_PAGE_DATA_SIZE)

#if NAND_FLASH_ECC_ON
  #define NAND_FLASH_PAGE_SPARE_SIZE 64
#else
  #define NAND_FLASH_PAGE_SPARE_SIZE 128
#endif

enum {
    NAND_FLASH_CMD_WRITE_DISABLE = 0x04,        // write disable
    NAND_FLASH_CMD_WRITE_ENABLE = 0x06,         // write enable
    NAND_FLASH_CMD_READ_ID = 0x9F,              // read id
    NAND_FLASH_CMD_GET_FEATURE = 0x0F,          // get feature register
    NAND_FLASH_CMD_SET_FEATURE = 0x1F,          // set feature register
    NAND_FLASH_CMD_PAGE_READ = 0x13,            // page read to cache
    NAND_FLASH_CMD_READ = 0x03,                 // read from cache
    NAND_FLASH_CMD_READ_X2 = 0x3B,              // read from cache x2
    NAND_FLASH_CMD_READ_X4 = 0x6B,              // read from cache x4
    NAND_FLASH_CMD_READ_DTR = 0xEE,             // read from cache x4 DTR mode
    NAND_FLASH_CMD_PARAMETER_READ = 0x13,       // read parameter page
    NAND_FLASH_CMD_UID_READ = 0x13,             // read uid page
    NAND_FLASH_CMD_PROGRAM_LOAD = 0x02,         // program load page
    NAND_FLASH_CMD_PROGRAM_LOAD_X4 = 0x32,      // program load page x4
    NAND_FLASH_CMD_PROGRAM_LOAD_RAND = 0x84,    // program load random page
    NAND_FLASH_CMD_PROGRAM_LOAD_RAND_X4 = 0xC4, // program execute page
    NAND_FLASH_CMD_PROGRAM_EXECUTE = 0x10,      // program execute page
    NAND_FLASH_CMD_BLOCK_ERASE = 0xD8,          // block erase
    NAND_FLASH_CMD_RESET = 0xFF, // reset, will reset PAGE READ/PROGRAM/ERASE operation. will reset status
                                 // register bits P_FAIL/E_FAIL/WEL/OIP/ECCS/ECCSE.

    NAND_FLASH_CMD_POWER_ON_RESET = 0x99, // Retrieve status register and data in cache to power on status.
};

typedef uint8_t nand_flash_cmd_t;

enum {
    NAND_FLASH_FEATURE_PROTECTION = 0xA0, // protection, write/read
    NAND_FLASH_FEATURE_0 = 0xB0,          // feature 0, write/read
    NAND_FLASH_FEATURE_1 = 0xD0,          // feature 1, write/read
    NAND_FLASH_FEATURE_STATUS_0 = 0xC0,   // status 0, read only
    NAND_FLASH_FEATURE_STATUS_1 = 0xF0,   // status 1, read only
};

typedef uint8_t nand_flash_feature_reg_t;

enum {
    NAND_FLASH_STATUS_BUSY = (1 << 0),           // busy
    NAND_FLASH_STATUS_WRITE_ENABLED = (1 << 1),  // write enabled
    NAND_FLASH_STATUS_ERASE_FAILED = (1 << 2),   // erase failed
    NAND_FLASH_STATUS_PROGRAM_FAILED = (1 << 3), // program failed
    NAND_FLASH_ECCS0 = (1 << 4),                 // ecc status 0
    NAND_FLASH_ECCS1 = (1 << 5),                 // ecc status 1
    NAND_FLASH_BP_ALL = 0x38,                    // protect all blocks
};

#define NAND_FLASH_FTL_MAGIC 0x304C5446 // "FTL0" in little endian

typedef enum {
    ECC_STATUS_NO_ERROR = 0,  // 0 0 x x : 未检测到位错误
    ECC_STATUS_CORRECTED_1_4, // 0 1 0 0 : 1~4 位已纠正
    ECC_STATUS_CORRECTED_5,   // 0 1 0 1 : 5 位已纠正
    ECC_STATUS_CORRECTED_6,   // 0 1 1 0 : 6 位已纠正
    ECC_STATUS_CORRECTED_7,   // 0 1 1 1 : 7 位已纠正
    ECC_STATUS_CORRECTED_8,   // 1 1 x x : 8 位已纠正
    ECC_STATUS_UNCORRECTABLE, // 1 0 x x : 超出能力，未纠正
    ECC_STATUS_UNKNOWN        // 保留，用于初始/无法解析的情况
} ecc_status_t;

typedef enum {
    BLOCK_STATE_UNKNOWN,     // 初始状态 / 未扫描
    BLOCK_STATE_GOOD,        // 正常可用
    BLOCK_STATE_WEAK,        // 弱块：ECC 频繁接近上限，建议不再放重要数据
    BLOCK_STATE_FACTORY_BAD, // 出厂标记为坏块（BBM 非 FFh），永不使用
    BLOCK_STATE_BAD_RUNTIME, // 运行中判定为坏块（ECC 超限或校验失败）
    BLOCK_STATE_RESERVED,    // 预留块，用于坏块替换或元数据
} block_state_t;

typedef struct {
    uint16_t id;                // block id
    block_state_t state;        // block state
    uint16_t erase_count;       // erase count
    uint32_t ecc_correct_count; // ecc correct count
    uint8_t max_ecc_bits;       // max ecc bits corrected
    uint8_t weak_page_count;    // weak count
} nand_block_t;

typedef struct {
    uint32_t magic;
    uint32_t version;
    uint16_t l2p[NAND_FLASH_VALID_BLOCK_COUNT];         // logic block to physical block mapping
    uint16_t spare[NAND_FLASH_MAX_INVALID_BLOCK_COUNT]; // spare block mapping
    // nand_block_t blocks[NAND_FLASH_BLOCK_TOTAL_COUNT]; // block status array
} nand_ftl_t;

void nand_flash_init(void);
int nand_flash_id(uint8_t* mid, uint8_t* did);
int nand_flash_write_enable(void);
int nand_flash_write_disable(void);
int nand_flash_set_feature(nand_flash_feature_reg_t feature, uint8_t value);
int nand_flash_get_feature(nand_flash_feature_reg_t feature, uint8_t* value);
int nand_flash_set_feature_bits(nand_flash_feature_reg_t feature, uint8_t bits);
int nand_flash_clear_feature_bits(nand_flash_feature_reg_t feature, uint8_t bits);

int nand_flash_erase(uint32_t block);
int nand_flash_read(uint32_t block, uint32_t page, uint32_t column, uint8_t* data, uint32_t size);
int nand_flash_write(uint32_t block, uint32_t page, uint32_t column, const uint8_t* data, uint32_t size);

void nand_test(void);

#endif
