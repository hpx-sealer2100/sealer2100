#include <stdint.h>

#include STM32_HAL_H

#include "fs.h"
#include "log.h"
#include "mpu.h"
#include "sha2.h"
#include "uart_log.h"

#if !defined (MODULE)
#define MODULE "upgrader"
#endif

#define UPGRADER_BINARY "/upgrader/upgrader.bin"

extern fs_t *__fs__;

void jump_to_upgrader(uint32_t address)
{
    uint32_t appStack;
    uint32_t appResetHandlerAddr;
    typedef void (*pFunc)(void);
    pFunc    appResetHandler;

    // 1. 禁止全局中断
    __disable_irq();

    // 2. 读取 app 向量表里的栈顶和 Reset_Handler
    appStack          = *(volatile uint32_t *)(address + 0);  // MSP 初始值
    appResetHandlerAddr = *(volatile uint32_t *)(address + 4);  // Reset_Handler 地址
    appResetHandler   = (pFunc)appResetHandlerAddr;

    // 可以选择关闭 SysTick 中断，但不清 LOAD/VAL
    SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;

    // 4. 关闭/复位 NVIC 中断（可选，看你需要）
    for (uint8_t i = 0; i < 8; i++) {
        NVIC->ICER[i] = 0xFFFFFFFF;
        NVIC->ICPR[i] = 0xFFFFFFFF;
    }

    // 5. 设置向量表偏移（VTOR）到 app 的向量表
    SCB->VTOR = address;

    // 6. 设置 MSP 为 app 的初始栈
    __set_MSP(appStack);
    __set_PSP(appStack);  // 可选，防止后面用到 PSP 时不一致

    // 7. 开中断（也可以让 app 的 Reset_Handler 自己开）
    // __enable_irq();

    // 8. 跳转到 app 的 Reset_Handler
    appResetHandler();

    while (1) {
    }
}

void try_startup_upgrader(void) {
  // check is upgrader binary exist
  if (!fs_is_file(__fs__, UPGRADER_BINARY)) {
    LOG_DEBUG(MODULE, "Upgrader binary not found, skip upgrade");
    return;
  }

  LOG_DEBUG(MODULE, "Start upgrader...");
  fs_file_t f;
  int ret = fs_file_open(__fs__, &f, UPGRADER_BINARY, FS_O_RDONLY);
  if (ret) {
    LOG_DEBUG(MODULE, "Open upgrader binary failed");
    return;
  }
  uint8_t *p = (uint8_t*)SDRAM_CODE_ADDRESS;
  // copy upgrader binary to sdram
  while ((ret = fs_file_read(&f, p, 16*1024)) > 0) {
    p += ret;
  }
  fs_file_close(&f);
  LOG_DEBUG(MODULE, "Upgrader binary copy done");
  // remove upgrader binary from fs, avoid recursive call
  fs_remove(__fs__, UPGRADER_BINARY);

  // check upgrader hash
  SHA256_CTX ctx;
  uint8_t digest[SHA256_DIGEST_LENGTH] = {0};
  sha256_Init(&ctx);
  sha256_Update(&ctx, (const uint8_t*)SDRAM_CODE_ADDRESS, p - (uint8_t*)SDRAM_CODE_ADDRESS);
  sha256_Final(&ctx, digest);
  LOG_HEXDUMP_DEBUG(MODULE, "Upgrader hash", digest, SHA256_DIGEST_LENGTH);
#if defined(PRODUCTION) && PRODUCTION
  static const uint8_t upgrader_sha256_hash[] = {
    0x66, 0x0c, 0x5e, 0x4c, 0x35, 0x00, 0x7b, 0xe4, 0x2c, 0xad, 0xb2, 0x84, 0xf1, 0x5d, 0xa1, 0x47,
    0x28, 0xfd, 0xa7, 0xd9, 0x64, 0x47, 0x82, 0x8f, 0x84, 0x32, 0x23, 0x23, 0x60, 0x09, 0x00, 0xb0
  };
  if (memcmp(digest, upgrader_sha256_hash, SHA256_DIGEST_LENGTH) != 0) {
    LOG_DEBUG(MODULE, "Upgrader hash verify failed");
    return;
  }
#endif
  LOG_DEBUG(MODULE, "Upgrader hash verify success");

  uart_log_flush();
  uart_log_deinit();
  // disable bus fault interrupt
  SCB->SHCSR &= ~SCB_SHCSR_BUSFAULTENA_Msk;
  // upgrader run in SDRAM, it can't config mpu again
  // so we enable mpu update
  mpu_update_enable();
  jump_to_upgrader(SDRAM_CODE_ADDRESS);
}
