/*
 * This file is part of the Trezor project, https://trezor.io/
 *
 * Copyright (c) SatoshiLabs
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include STM32_HAL_H

#include <string.h>

#include "common.h"
#include "display.h"
#include "flash.h"
#include "rand.h"
#include "supervise.h"
#include "touch.h"
#include "device.h"
#include "power_manager.h"

#if defined(STM32F427xx) || defined(STM32F405xx)
#include "stm32f4xx_ll_utils.h"
#elif defined(STM32H747xx)
#include "stm32h7xx_ll_utils.h"
#endif


// from util.s
extern void shutdown_privileged(void);
// firmware/bootloader provide `__fatal`
extern
__attribute__((noreturn))
void __fatal(const char* lines[]);

void shutdown(void) {
  pm_power_off();
}

// void shutdown(void) {
// #ifdef USE_SVC_SHUTDOWN
//   svc_shutdown();
// #else
//   // It won't work properly unless called from the privileged mode
//   shutdown_privileged();
// #endif
// }

void restart(void) {
  // power down modules
  // pm_power_down(POWER_MODULE_BLUETOOTH);
  pm_power_down(POWER_MODULE_CAMERA);
  pm_power_down(POWER_MODULE_MOTOR);
  svc_reset_system();
}

void reboot_to_board(void) {
  device_set_run_mode(RUN_MODE_BOARDLOADER);
  restart();
}

void reboot_to_boot(void) {
  device_set_run_mode(RUN_MODE_BOOTLOADER);
  restart();
}

void __attribute__((noreturn))
__fatal_error(const char *expr, const char *msg, const char *file, int line,
              const char *func) {
  char source[256] = {0};
  char _expr[256] = {0};
  char _func[256] = {0};
  char _msg[256] = {0};

  sprintf(source, "file: %s:%d", file, line);
  sprintf(_expr, "expr: %s", expr);
  sprintf(_func, "func: %s", func);
  sprintf(_msg, "msg: %s", msg);

#ifdef BUILD_ID
  const char *id = (const char *)BUILD_ID;
  char _build_id[256] = {0};
  sprintf(_build_id, "build id: %s", id);

#endif

  const char* lines[] = {
    "FATAL ERROR:",
    _expr,
    _msg,
    source,
    _func,
#ifdef BUILD_ID
    _build_id,
#endif
    NULL
  };
  __fatal(lines);
}

void __attribute__((noreturn))
error_shutdown(const char *line1, const char *line2, const char *line3,
               const char *line4) {
  const char *lines[] = {
    line1,
    line2,
    line3,
    line4,
    NULL
  };
  __fatal(lines);
}

void error_reset(const char *line1, const char *line2, const char *line3,
                 const char *line4) {
  const char *lines[] = {
    line1,
    line2,
    line3,
    line4,
    NULL
  };
  __fatal(lines);
}

#ifndef NDEBUG
void __assert_func(const char *file, int line, const char *func,
                   const char *expr) {
  __fatal_error(expr, "assert failed", file, line, func);
}
#endif

void hal_delay(uint32_t ms) { HAL_Delay(ms); }
uint32_t hal_ticks_ms() { return HAL_GetTick(); }

// reference RM0090 section 35.12.1 Figure 413
#define USB_OTG_HS_DATA_FIFO_RAM (USB_OTG_HS_PERIPH_BASE + 0x20000U)
#define USB_OTG_HS_DATA_FIFO_SIZE (4096U)

void clear_otg_hs_memory(void) {
  // use the HAL version due to section 2.1.6 of STM32F42xx Errata sheet
  __HAL_RCC_USB_OTG_HS_CLK_ENABLE();  // enable USB_OTG_HS peripheral clock so
                                      // that the peripheral memory is
                                      // accessible
  memset_reg(
      (volatile void *)USB_OTG_HS_DATA_FIFO_RAM,
      (volatile void *)(USB_OTG_HS_DATA_FIFO_RAM + USB_OTG_HS_DATA_FIFO_SIZE),
      0);
  __HAL_RCC_USB_OTG_HS_CLK_DISABLE();  // disable USB OTG_HS peripheral clock as
                                       // the peripheral is not needed right now
}

uint32_t __stack_chk_guard = 0;

void __attribute__((noreturn)) __stack_chk_fail(void) {
  error_shutdown("Internal error", "(SS)", NULL, NULL);
}

uint8_t HW_ENTROPY_DATA[HW_ENTROPY_LEN];

void collect_hw_entropy(void) {
  // collect entropy from UUID
  uint32_t w = LL_GetUID_Word0();
  memcpy(HW_ENTROPY_DATA, &w, 4);
  w = LL_GetUID_Word1();
  memcpy(HW_ENTROPY_DATA + 4, &w, 4);
  w = LL_GetUID_Word2();
  memcpy(HW_ENTROPY_DATA + 8, &w, 4);
  // set entropy in the OTP randomness block
  if (secfalse == flash_otp_is_locked(FLASH_OTP_BLOCK_RANDOMNESS)) {
    uint8_t entropy[FLASH_OTP_BLOCK_SIZE];
    random_buffer(entropy, FLASH_OTP_BLOCK_SIZE);
    ensure(flash_otp_write(FLASH_OTP_BLOCK_RANDOMNESS, 0, entropy,
                           FLASH_OTP_BLOCK_SIZE),
           NULL);
    ensure(flash_otp_lock(FLASH_OTP_BLOCK_RANDOMNESS), NULL);
  }
  // collect entropy from OTP randomness block
  ensure(flash_otp_read(FLASH_OTP_BLOCK_RANDOMNESS, 0, HW_ENTROPY_DATA + 12,
                        FLASH_OTP_BLOCK_SIZE),
         NULL);
}

bool check_all_ones(const void *data, int len) {
  if (!data) return false;
  uint8_t result = 0xff;
  const uint8_t *ptr = (const uint8_t *)data;

  for (; len; len--, ptr++) {
    result &= *ptr;
    if (result != 0xff) break;
  }

  return (result == 0xff);
}

bool check_all_zeros(const void *data, int len) {
  if (!data) return false;
  uint8_t result = 0x0;
  const uint8_t *ptr = (const uint8_t *)data;

  for (; len; len--, ptr++) {
    result |= *ptr;
    if (result) break;
  }

  return (result == 0x00);
}
