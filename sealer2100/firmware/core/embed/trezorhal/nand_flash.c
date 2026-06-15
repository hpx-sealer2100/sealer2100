#include "nand_flash.h"

#include <string.h>
#include <stdint.h>

#include "common.h"
#include "stm32h7xx_hal.h"
#include "log.h"
#include "stm32h7xx_hal_qspi.h"
#include "uart_log.h"

#define MODULE "nand flash"

#define RA_FROM_BLOCK_PAGE(block, page) (block * NAND_FLASH_PAGE_PER_BLOCK_COUNT + page)
#define AUTO_POLLING_INTERVAL 1000

static QSPI_HandleTypeDef hqspi;

// internal functions
static int nand_flash_busy_wait(void) {
  QSPI_CommandTypeDef command = {0};
  command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
  command.Instruction = NAND_FLASH_CMD_GET_FEATURE;
  command.DataMode = QSPI_DATA_1_LINE;
  command.AddressMode = QSPI_ADDRESS_1_LINE;
  command.Address = NAND_FLASH_FEATURE_STATUS_0;

  QSPI_AutoPollingTypeDef config;
  /* Configure automatic polling mode to wait for write enabling ---- */
  config.Match = 0;
  config.Mask = NAND_FLASH_STATUS_BUSY;
  config.MatchMode = QSPI_MATCH_MODE_AND;
  config.StatusBytesSize = 1;
  config.Interval = AUTO_POLLING_INTERVAL;
  config.AutomaticStop = QSPI_AUTOMATIC_STOP_ENABLE;

  if (HAL_QSPI_AutoPolling(&hqspi, &command, &config,
                           HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
    return -1;
  }
  return 0;
}

static int nand_flash_power_on_reset(void) {
  QSPI_CommandTypeDef command = {0};
  command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
  command.Instruction = NAND_FLASH_CMD_POWER_ON_RESET;

  if (HAL_QSPI_Command(&hqspi, &command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
    return -1;
  }
  return 0;
}

static int nand_flash_reset(void) {
  QSPI_CommandTypeDef command = {0};
  command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
  command.Instruction = NAND_FLASH_CMD_RESET;

  if (HAL_QSPI_Command(&hqspi, &command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
    return -1;
  }
  return 0;
}

static int nand_flash_read_page(uint32_t ra) {
  QSPI_CommandTypeDef command = {0};
  command.Instruction = NAND_FLASH_CMD_PAGE_READ;
  command.Address = ra;

  command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
  command.AddressSize = QSPI_ADDRESS_24_BITS;
  command.AddressMode = QSPI_ADDRESS_1_LINE;

  if (HAL_QSPI_Command(&hqspi, &command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) !=
      HAL_OK) {
    return -1;
  }
  return 0;
}

static int nand_flash_read_from_cache(uint32_t ca,  uint8_t *data, uint32_t size) {
  QSPI_CommandTypeDef command = {0};
  // command.Instruction = NAND_FLASH_CMD_READ_X4;
  command.Instruction = NAND_FLASH_CMD_READ_DTR;
  command.Address = ca;
  command.NbData = size;

  command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
  command.DataMode = QSPI_DATA_4_LINES;
  command.DummyCycles = 8;

  // x4
  // command.AddressMode = QSPI_ADDRESS_1_LINE;
  // command.AddressSize = QSPI_ADDRESS_16_BITS;

  // dtr
  command.DdrMode = QSPI_DDR_MODE_ENABLE;
  command.AddressMode = QSPI_ADDRESS_4_LINES;
  command.AddressSize = QSPI_ADDRESS_32_BITS;

  if (HAL_QSPI_Command(&hqspi, &command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) !=
      HAL_OK) {
    return -1;
  }

  if (HAL_QSPI_Receive(&hqspi, data, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
    return -1;
  }

  return 0;
}

static int nand_flash_read_spara(uint32_t ra, uint8_t oob[128]) {
  // step 0. read page to cache
  nand_flash_read_page(ra);

  // step 1. wait read page to page done
  nand_flash_busy_wait();

  // step 2. read data from cache
  return nand_flash_read_from_cache(2048, oob, 128);
}

static int nand_flash_program_load(uint32_t ca, const uint8_t *data, uint32_t size) {
  QSPI_CommandTypeDef command = {0};
  command.Instruction = NAND_FLASH_CMD_PROGRAM_LOAD_X4;
  command.Address = ca;
  command.NbData = size;

  command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
  command.AddressMode = QSPI_ADDRESS_1_LINE;
  command.AddressSize = QSPI_ADDRESS_16_BITS;
  command.DataMode = QSPI_DATA_4_LINES;
  if (HAL_QSPI_Command(&hqspi, &command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
    return -1;
  }
  if (HAL_QSPI_Transmit(&hqspi, (uint8_t *)data, size) != HAL_OK) {
    return -1;
  }
  return 0;
}

static int nand_flash_program_execute(uint32_t ra) {
  QSPI_CommandTypeDef command = {0};
  command.Instruction = NAND_FLASH_CMD_PROGRAM_EXECUTE;
  command.Address = ra;

  command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
  command.AddressSize = QSPI_ADDRESS_24_BITS;
  command.AddressMode = QSPI_ADDRESS_1_LINE;

  if (HAL_QSPI_Command(&hqspi, &command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
    return -1;
  }

  return 0;
}

static int nand_flash_block_erase(uint32_t ra) {
  QSPI_CommandTypeDef command = {0};
  command.Instruction = NAND_FLASH_CMD_BLOCK_ERASE;
  command.Address = ra;

  command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
  command.AddressSize = QSPI_ADDRESS_24_BITS;
  command.AddressMode = QSPI_ADDRESS_1_LINE;

  if (HAL_QSPI_Command(&hqspi, &command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
    return -1;
  }

  return 0;
}

static int nand_unlock_all_block(void) {
  nand_flash_clear_feature_bits(NAND_FLASH_FEATURE_PROTECTION, NAND_FLASH_BP_ALL);
  return 0;
}

static int nand_lock_all_block(void) {
  nand_flash_set_feature_bits(NAND_FLASH_FEATURE_PROTECTION, NAND_FLASH_BP_ALL);
  return 0;
}


// export functions
void nand_flash_init(void) {
    // initialize nand flash
    /**
     * QUADSPI GPIO Configuration
     * PG6     ------> QUADSPI_BK1_NCS
     * PF6     ------> QUADSPI_BK1_IO3
     * PF7     ------> QUADSPI_BK1_IO2
     * PD11    ------> QUADSPI_BK1_IO0
     * PB2     ------> QUADSPI_CLK
     * PF9     ------> QUADSPI_BK1_IO1
     */
  __HAL_RCC_QSPI_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  GPIO_InitTypeDef gpio = {0};

  gpio.Pin = GPIO_PIN_6;
  gpio.Mode = GPIO_MODE_AF_PP;
  gpio.Pull = GPIO_NOPULL;
  gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  gpio.Alternate = GPIO_AF10_QUADSPI;
  HAL_GPIO_Init(GPIOG, &gpio);

  gpio.Pin = GPIO_PIN_6|GPIO_PIN_7;
  gpio.Mode = GPIO_MODE_AF_PP;
  gpio.Pull = GPIO_NOPULL;
  gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  gpio.Alternate = GPIO_AF9_QUADSPI;
  HAL_GPIO_Init(GPIOF, &gpio);

  gpio.Pin = GPIO_PIN_9;
  gpio.Mode = GPIO_MODE_AF_PP;
  gpio.Pull = GPIO_NOPULL;
  gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  gpio.Alternate = GPIO_AF10_QUADSPI;
  HAL_GPIO_Init(GPIOF, &gpio);

  gpio.Pin = GPIO_PIN_2;
  gpio.Mode = GPIO_MODE_AF_PP;
  gpio.Pull = GPIO_NOPULL;
  gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  gpio.Alternate = GPIO_AF9_QUADSPI;
  HAL_GPIO_Init(GPIOB, &gpio);

  gpio.Pin = GPIO_PIN_11;
  gpio.Mode = GPIO_MODE_AF_PP;
  gpio.Pull = GPIO_NOPULL;
  gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  gpio.Alternate = GPIO_AF9_QUADSPI;
  HAL_GPIO_Init(GPIOD, &gpio);

  hqspi.Instance = QUADSPI;
  // hqspi.Init.ClockPrescaler = 2;
  hqspi.Init.ClockPrescaler = 1;
  hqspi.Init.FifoThreshold = 4;
  hqspi.Init.SampleShifting = QSPI_SAMPLE_SHIFTING_NONE;
  hqspi.Init.FlashSize = 23;
  hqspi.Init.ChipSelectHighTime = QSPI_CS_HIGH_TIME_1_CYCLE;
  hqspi.Init.ClockMode = QSPI_CLOCK_MODE_0;
  hqspi.Init.FlashID = QSPI_FLASH_ID_1;
  hqspi.Init.DualFlash = QSPI_DUALFLASH_DISABLE;
  HAL_QSPI_DeInit(&hqspi);
  HAL_QSPI_Init(&hqspi);

  // power on reset
  nand_flash_power_on_reset();
  // wait for power on reset to finish
  nand_flash_busy_wait();

  // enable QE mode
  nand_flash_set_feature_bits(NAND_FLASH_FEATURE_0, 0x01);
}

int nand_flash_id(uint8_t* mid, uint8_t* did) {
  uint8_t buf[2];
  QSPI_CommandTypeDef command = {0};
  command.Instruction = NAND_FLASH_CMD_READ_ID;
  command.NbData = 2;

  command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
  command.DataMode = QSPI_DATA_1_LINE;
  command.DummyCycles = 8;

  if (HAL_QSPI_Command(&hqspi, &command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) !=
      HAL_OK) {
    return -1;
  }

  if (HAL_QSPI_Receive(&hqspi, buf, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
    return -1;
  }

  *mid = buf[0];
  *did = buf[1];
  return 0;
}

int nand_flash_write_enable(void) {
  QSPI_CommandTypeDef command = {0};
  command.Instruction = NAND_FLASH_CMD_WRITE_ENABLE;

  command.InstructionMode = QSPI_INSTRUCTION_1_LINE;

  if (HAL_QSPI_Command(&hqspi, &command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) !=
      HAL_OK) {
    return -1;
  }

  QSPI_AutoPollingTypeDef config;
  /* Configure automatic polling mode to wait for write enabling ---- */
  config.Match = NAND_FLASH_STATUS_WRITE_ENABLED;
  config.Mask = NAND_FLASH_STATUS_WRITE_ENABLED;
  config.MatchMode = QSPI_MATCH_MODE_AND;
  config.StatusBytesSize = 1;
  config.Interval = AUTO_POLLING_INTERVAL;
  config.AutomaticStop = QSPI_AUTOMATIC_STOP_ENABLE;

  command.Instruction = 0x0F;
  command.DataMode = QSPI_DATA_1_LINE;
  command.AddressMode = QSPI_ADDRESS_1_LINE;
  command.Address = NAND_FLASH_FEATURE_STATUS_0;

  if (HAL_QSPI_AutoPolling(&hqspi, &command, &config,
                           HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
    return HAL_ERROR;
  }
  return 0;
}

int nand_flash_write_disable(void) {
  QSPI_CommandTypeDef command = {0};
  command.Instruction = NAND_FLASH_CMD_WRITE_DISABLE;

  command.InstructionMode = QSPI_INSTRUCTION_1_LINE;

  if (HAL_QSPI_Command(&hqspi, &command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) !=
      HAL_OK) {
    return -1;
  }
  QSPI_AutoPollingTypeDef config;
  /* Configure automatic polling mode to wait for write enabling ---- */
  config.Match = 0;
  config.Mask = NAND_FLASH_STATUS_WRITE_ENABLED;
  config.MatchMode = QSPI_MATCH_MODE_AND;
  config.StatusBytesSize = 1;
  config.Interval = AUTO_POLLING_INTERVAL;
  config.AutomaticStop = QSPI_AUTOMATIC_STOP_ENABLE;

  command.Instruction = 0x0F;
  command.DataMode = QSPI_DATA_1_LINE;
  command.AddressMode = QSPI_ADDRESS_1_LINE;
  command.Address = NAND_FLASH_FEATURE_STATUS_0;

  if (HAL_QSPI_AutoPolling(&hqspi, &command, &config,
                           HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
    return HAL_ERROR;
  }
  return 0;
}

int nand_flash_set_feature(nand_flash_feature_reg_t feature, uint8_t value) {
  QSPI_CommandTypeDef command = {0};
  command.Instruction = NAND_FLASH_CMD_SET_FEATURE;
  command.Address = feature;
  command.NbData = 1;

  command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
  command.AddressSize = QSPI_ADDRESS_8_BITS;
  command.AddressMode = QSPI_ADDRESS_1_LINE;
  command.DataMode = QSPI_DATA_1_LINE;

  if (HAL_QSPI_Command(&hqspi, &command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) !=
      HAL_OK) {
    return -1;
  }

  if (HAL_QSPI_Transmit(&hqspi, &value, 1) != HAL_OK) {
    return -1;
  }

  return 0;
}

int nand_flash_get_feature(nand_flash_feature_reg_t feature, uint8_t* value) {
  QSPI_CommandTypeDef command = {0};
  command.Instruction = 0x0F;
  command.Address = feature;
  command.NbData = 1;

  command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
  command.AddressSize = QSPI_ADDRESS_8_BITS;
  command.AddressMode = QSPI_ADDRESS_1_LINE;
  command.DataMode = QSPI_DATA_1_LINE;

  if (HAL_QSPI_Command(&hqspi, &command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) !=
      HAL_OK) {
    return -1;
  }

  if (HAL_QSPI_Receive(&hqspi, value, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
    return -1;
  }
  return 0;
}
int nand_flash_set_feature_bits(nand_flash_feature_reg_t feature, uint8_t bits) {
  uint8_t value = 0;
  nand_flash_get_feature(feature, &value);
  // LOG_DEBUG(MODULE, "set feature %X from %X to %X", feature, value, value|bits);
  value |= bits;
  nand_flash_set_feature(feature, value);

  // nand_flash_get_feature(feature, &value);
  // LOG_DEBUG(MODULE, "verify feature %X value %X", feature, value);
  return 0;
}

int nand_flash_clear_feature_bits(nand_flash_feature_reg_t feature, uint8_t bits) {
  uint8_t value = 0;
  nand_flash_get_feature(feature, &value);
  // LOG_DEBUG(MODULE, "clear feature %X from %X to %X", feature, value, value&~bits);
  value &= ~bits;
  nand_flash_set_feature(feature, value);

  // nand_flash_get_feature(feature, &value);
  // LOG_DEBUG(MODULE, "verify feature %X value %X", feature, value);
  return 0;
}

int nand_flash_read(uint32_t block, uint32_t page, uint32_t column, uint8_t *data, uint32_t size) {
  uint32_t ra = RA_FROM_BLOCK_PAGE(block, page);

  // step 0. read page to cache
  nand_flash_read_page(ra);

  // step 1. wait read page to page done
  nand_flash_busy_wait();

  // step 2. read data from cache
  return nand_flash_read_from_cache(column, data, size);
}

int nand_flash_write(uint32_t block, uint32_t page, uint32_t column, const uint8_t *data, uint32_t size) {
  uint32_t ra = RA_FROM_BLOCK_PAGE(block, page);

  // step 0. program load page
  nand_flash_program_load(column, data, size);

  // step 1. unlock all block
  nand_unlock_all_block();

  // step 2. write enable
  nand_flash_write_enable();

  // step 3. program execute
  nand_flash_program_execute(ra);

  // step 4. wait program page to page done
  nand_flash_busy_wait();

  // step 5. lock all block
  nand_lock_all_block();

  // step 6. check program result
  uint8_t status = 0;
  nand_flash_get_feature(NAND_FLASH_FEATURE_STATUS_0, &status);
  // LOG_DEBUG(MODULE, "nand flash status: 0x%02x", status);

  if (status & NAND_FLASH_STATUS_PROGRAM_FAILED) {
    LOG_ERROR(MODULE, "nand flash program failed");
    nand_flash_write_disable();
    nand_flash_reset();
    return -1;
  }

  return 0;
}

int nand_flash_erase(uint32_t block) {
  uint32_t ra = RA_FROM_BLOCK_PAGE(block, 0);

  // step 0. unlock all block
  nand_unlock_all_block();

  // step 1. write enable
  nand_flash_write_enable();

  // step 2. block erase
  nand_flash_block_erase(ra);

  // step 3. wait block erase done
  nand_flash_busy_wait();

  // step 4. lock all block
  nand_lock_all_block();

  // step 5. check block erase result
  uint8_t status = 0;
  nand_flash_get_feature(NAND_FLASH_FEATURE_STATUS_0, &status);
  // LOG_DEBUG(MODULE, "nand flash status: 0x%02x", status);

  if (status & NAND_FLASH_STATUS_ERASE_FAILED) {
    LOG_ERROR(MODULE, "nand flash erase failed");
    nand_flash_write_disable();
    nand_flash_reset();
    return -1;
  }
  return 0;
}

void nand_test(void) {
    LOG_DEBUG(MODULE, "nand test");
    uint8_t mid = 0, did = 0;
    uint8_t feature = 0;

    nand_flash_id(&mid, &did);
    LOG_DEBUG(MODULE, "nand flash id: mid=0x%02x, did=0x%02x", mid, did);

    // test write enable status
    nand_flash_get_feature(NAND_FLASH_FEATURE_STATUS_0, &feature);
    LOG_DEBUG(MODULE, "nand flash feature %02x: 0x%02x", NAND_FLASH_FEATURE_STATUS_0, feature);
    if (feature & NAND_FLASH_STATUS_WRITE_ENABLED) {
      LOG_ERROR(MODULE, "nand flash write enabled");
    } else {
      LOG_DEBUG(MODULE, "nand flash write disabled");
    }

    // enable write enable
    nand_flash_write_enable();
    // test write enable status again
    nand_flash_get_feature(NAND_FLASH_FEATURE_STATUS_0, &feature);
    LOG_DEBUG(MODULE, "nand flash feature %02x: 0x%02x", NAND_FLASH_FEATURE_STATUS_0, feature);
    if (feature & NAND_FLASH_STATUS_WRITE_ENABLED) {
      LOG_DEBUG(MODULE, "nand flash write enabled");
    } else {
      LOG_ERROR(MODULE, "nand flash write disabled");
    }
    // disable write enable
    nand_flash_write_disable();
    // test write enable status again
    nand_flash_get_feature(NAND_FLASH_FEATURE_STATUS_0, &feature);
    LOG_DEBUG(MODULE, "nand flash feature %02x: 0x%02x", NAND_FLASH_FEATURE_STATUS_0, feature);
    if (feature & NAND_FLASH_STATUS_WRITE_ENABLED) {
      LOG_ERROR(MODULE, "nand flash write enabled");
    } else {
      LOG_DEBUG(MODULE, "nand flash write disabled");
    }

    nand_flash_get_feature(NAND_FLASH_FEATURE_PROTECTION, &feature);
    LOG_DEBUG(MODULE, "nand flash feature %02x: 0x%02x", NAND_FLASH_FEATURE_PROTECTION, feature);
    nand_flash_get_feature(NAND_FLASH_FEATURE_0, &feature);
    LOG_DEBUG(MODULE, "nand flash feature %02x: 0x%02x", NAND_FLASH_FEATURE_0, feature);
    nand_flash_get_feature(NAND_FLASH_FEATURE_1, &feature);
    LOG_DEBUG(MODULE, "nand flash feature %02x: 0x%02x", NAND_FLASH_FEATURE_1, feature);
    nand_flash_get_feature(NAND_FLASH_FEATURE_STATUS_0, &feature);
    LOG_DEBUG(MODULE, "nand flash feature %02x: 0x%02x", NAND_FLASH_FEATURE_STATUS_0, feature);
    nand_flash_get_feature(NAND_FLASH_FEATURE_STATUS_1, &feature);
    LOG_DEBUG(MODULE, "nand flash feature %02x: 0x%02x", NAND_FLASH_FEATURE_STATUS_1, feature);


    // write/read test, in last block
    uint32_t block = NAND_FLASH_BLOCK_TOTAL_COUNT - 1;
    for (int i = 0; i < 2; i++) {
      // [0, 128] in each page
      uint8_t data[128] = {0};
      memset(data, i, sizeof(data));
      nand_flash_write(block, i, 0, data, sizeof(data));

      uint8_t data2[128] = {0};
      nand_flash_read(block, i, 0, data2, sizeof(data2));
      if (memcmp(data, data2, sizeof(data)) != 0) {
        LOG_ERROR(MODULE, "read data not match");
        LOG_HEXDUMP_DEBUG(MODULE, "read data", data2, sizeof(data2));
      } else {
        LOG_DEBUG(MODULE, "write/read at block %d page %d success", block, i);
      }

      uart_log_flush();
    }

    uint8_t oob[128] = {0};

    nand_flash_read_spara(RA_FROM_BLOCK_PAGE(block, 0), oob);
    LOG_DEBUG(MODULE, "block %d:", block);
    LOG_HEXDUMP_DEBUG(MODULE, "spara data", oob, sizeof(oob));
    uart_log_flush();

    nand_flash_read_spara(RA_FROM_BLOCK_PAGE(block, 1), oob);
    LOG_DEBUG(MODULE, "block %d:", block);
    LOG_HEXDUMP_DEBUG(MODULE, "spara data", oob, sizeof(oob));
    uart_log_flush();

    nand_flash_erase(block);

    for (int i = 0; i < 2; i++) {
      nand_flash_read_spara(RA_FROM_BLOCK_PAGE(block, i), oob);
      LOG_DEBUG(MODULE, "block %d page %d:", block, i);
      LOG_HEXDUMP_DEBUG(MODULE, "spara data", oob, sizeof(oob));
      uart_log_flush();
    }

}
