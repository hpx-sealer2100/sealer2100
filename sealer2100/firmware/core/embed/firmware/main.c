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

#include "sdram.h"
#include STM32_HAL_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "firmware_context.h"

#include "py/compile.h"
#include "py/gc.h"
#include "py/mperrno.h"
#include "py/nlr.h"
#include "py/repl.h"
#include "py/runtime.h"
#include "py/stackctrl.h"
#include "shared/runtime/pyexec.h"

#include "ports/stm32/gccollect.h"
#include "ports/stm32/pendsv.h"

#include "button.h"
#include "board_capabilities.h"
#include "common.h"
#include "compiler_traits.h"
#include "display.h"
#include "emmc.h"
#include "nand_flash.h"
#include "fs.h"
#include "fs.h"
#include "fsport/lfs_port.h"
#include "fsport/fatfs_diskio.h"
#include "flash.h"
#include "image.h"
#include "mpu.h"
#include "random_delays.h"
#include "sys.h"
#include "usart.h"
#ifdef SYSTEM_VIEW
#include "systemview.h"
#endif
#include "rng.h"
#include "device.h"
#include "mipi_lcd.h"
#include "qspi_flash.h"
#include "spi.h"
#include "battery.h"
#include "supervise.h"
#include "timer.h"
#include "touch.h"
#include "thd89/se.h"
#include "uart_log.h"
#include "power_manager.h"
#ifdef USE_SECP256K1_ZKP
#include "zkp_context.h"
#endif
#include "i2c.h"
#include "keys.h"
#include "log.h"
#include "boot_updater.h"

#include "lvgl.h"
#include "lv_port.h"
#include "sram.h"
#include "memzero.h"

#define MODULE "firmware"
#define BOOT_BINARY "/boot/bootloader.bin"

#define NAND_TEST 0

#include "upgrader.c"

// from util.s
extern void shutdown_privileged(void);
void system_problem_detected(void);

static void copyflash2sdram(void) {
  extern int _flash2_load_addr, _flash2_start, _flash2_end;
  volatile uint32_t *dst = (volatile uint32_t *)&_flash2_start;
  volatile uint32_t *end = (volatile uint32_t *)&_flash2_end;
  volatile uint32_t *src = (volatile uint32_t *)&_flash2_load_addr;

  while (dst < end) {
    *dst++ = *src++;
  }
}

static void bus_fault_disable() { SCB->SHCSR &= ~SCB_SHCSR_BUSFAULTENA_Msk; }

void upgrade_bootloader(void) {
  // check is bootloader binary exist
  if (!fs_is_file(&firmware_ctx.fs, BOOT_BINARY)) {
    LOG_DEBUG(MODULE, "Bootloader binary not found, no need to upgrade");
    return;
  }

  LOG_DEBUG(MODULE, "Upgrade bootloader...");
  // enter bootloader updater
  boot_updater(BOOT_BINARY);
}


void fs_setup(fs_t* fs) {
  fs_init(fs);

  // user can test pcb version and set `lfs` or `fatfs` port

  // use fatfs and emmc
  const fatfs_diskio_t* config = fatfs_port_emmc_diskio();
  fatfs_port_t* port = fatfs_port_init(config);
  fs_fatfs_register(fs, port);

  // use lfs and nand, for test
  // const struct lfs_config* config = lfs_port_nand_config();
  // lfs_port_t* port = lfs_port_init(config);
  // fs_lfs_register(fs, port);


  // use lfs and emmc, for test
  // const struct lfs_config* config = lfs_port_emmc_config();
  // lfs_port_t* port = lfs_port_init(config);
  // fs_lfs_register(fs, port);
}

// provide fs_t* __fs__ for lvgl font
fs_t *__fs__ = &firmware_ctx.fs;


int main(void) {
  extern uint32_t _vector_offset;
  SCB->VTOR = (uint32_t)&_vector_offset;

  // Enable MPU
  mpu_config_firmware();
  memset((void*)SRAM_BASE_ADDRESS, 0, SRAM_SIZE);
  // update system clock from registers
  SystemCoreClockUpdate();
  rng_init();

  // logging
  uart_log_init();
  LOG_DEBUG(MODULE, "Hello, world!");

  battery_init();
  pm_init();

  // emmc flash
  emmc_init();
  LOG_DEBUG(MODULE, "emmc init done");

#if NAND_TEST
  // nand flash
  nand_flash_init();
  LOG_DEBUG(MODULE, "nand flash init done");

#else
  // quad spi flash, mpy code stored in, need copy to ram for run
  qspi_flash_init();
  qspi_flash_config();
  qspi_flash_memory_mapped();
#endif

  fs_setup(&firmware_ctx.fs);

  device_para_init();

  bus_fault_disable();

  // try startup upgrader
  // try_startup_upgrader();

  // touch pad
  i2c4_init();
  touch_init();

  display_backlight(0);
  lcd_init(DISPLAY_RESX, DISPLAY_RESY, LCD_PIXEL_FORMAT_RGB565);
  lcd_pwm_init();
  display_clear();

  // lvgl timer
  timer_init();

  upgrade_bootloader();

  random_delays_init();
#ifdef RDI
  rdi_start();
#endif

  collect_hw_entropy();

#ifdef SYSTEM_VIEW
  enable_systemview();
#endif

  se_init();

  uint8_t secret[32] = {0};
  if (device_get_pre_shared_key(secret)) {
    se_handshake(secret, sizeof(secret));
  }
  memzero(secret, sizeof(secret));

#if !PRODUCTION
  // enable BUS fault and USAGE fault handlers
  SCB->SHCSR |= (SCB_SHCSR_USGFAULTENA_Msk | SCB_SHCSR_BUSFAULTENA_Msk);
#endif



#ifdef USE_SECP256K1_ZKP
  ensure(sectrue * (zkp_context_init() == 0), NULL);
#endif
  LOG_DEBUG(MODULE, "Preparing code");
  copyflash2sdram();
  LOG_DEBUG(MODULE, "Preparing code done");

  // Stack limit should be less than real stack size, so we have a chance
  // to recover from limit hit.
  mp_stack_set_top(&_estack);
  mp_stack_set_limit((char *)&_estack - (char *)&_sstack - 1024);

#if MICROPY_ENABLE_PYSTACK
  static mp_obj_t pystack[1024];
  mp_pystack_init(pystack, &pystack[MP_ARRAY_SIZE(pystack)]);
#endif

  // GC init
  LOG_DEBUG(MODULE, "Starting GC");
//   gc_init(&_heap_start, &_heap_end);
  gc_init((void*)SDRAM_MP_ADDRESS, (void*)SDRAM_MP_END);

  // Interpreter init
  LOG_DEBUG(MODULE, "Starting interpreter");
  mp_init();
  mp_obj_list_init(mp_sys_argv, 0);
  mp_obj_list_init(mp_sys_path, 0);
  mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR__dot_frozen));

  // lvgl init
  lv_init();
  // don't known why can't register driver in c, mpy just use, in this case, lv_timer_handler will crash
  // why?
  // but init lvgl itself work fine
//   lv_port_disp_init();
//   lv_port_indev_init();
  lv_port_fs_init(&firmware_ctx.fs);

  // Execute the main script
  LOG_DEBUG(MODULE, "Executing main script");
  pyexec_frozen_module("main.py");
  // Clean up
  LOG_DEBUG(MODULE, "Main script finished, cleaning up");
  mp_deinit();

  // system problem detect
  // when system problem detected, this way can help to recover from it
  system_problem_detected();

  return 0;
}

// MicroPython default exception handler

void __attribute__((noreturn)) nlr_jump_fail(void *val) {
  error_shutdown("Internal error", "(UE)", NULL, NULL);
}

// interrupt handlers

void NMI_Handler(void) {
  // Clock Security System triggered NMI
  // if ((RCC->CIR & RCC_CIR_CSSF) != 0)
  { error_shutdown("Internal error", "(CS)", NULL, NULL); }
}

// Hard fault handler
#if defined SYSTEM_VIEW
enum { r0, r1, r2, r3, r12, lr, pc, psr };
void STACK_DUMP(unsigned int *stack) {
  display_printf("[STACK DUMP]\n");
  display_printf("R0 = 0x%08x\n", stack[r0]);
  display_printf("R1 = 0x%08x\n", stack[r1]);
  display_printf("R2 = 0x%08x\n", stack[r2]);
  display_printf("R3 = 0x%08x\n", stack[r3]);
  display_printf("R12 = 0x%08x\n", stack[r12]);
  display_printf("LR = 0x%08x\n", stack[lr]);
  display_printf("PC = 0x%08x\n", stack[pc]);
  display_printf("PSR = 0x%08x\n", stack[psr]);
  display_printf("BFAR = 0x%08x\n", (*((volatile unsigned int *)(0xE000ED38))));
  display_printf("CFSR = 0x%08x\n", (*((volatile unsigned int *)(0xE000ED28))));
  display_printf("HFSR = 0x%08x\n", (*((volatile unsigned int *)(0xE000ED2C))));
  display_printf("DFSR = 0x%08x\n", (*((volatile unsigned int *)(0xE000ED30))));
  display_printf("AFSR = 0x%08x\n", (*((volatile unsigned int *)(0xE000ED3C))));
  exit(0);
  return;
}

__attribute__((naked)) void HardFault_Handler(void) {
  __asm volatile(
      " tst lr, #4    \n"  // Test Bit 3 to see which stack pointer we should
                           // use.
      " ite eq        \n"  // Tell the assembler that the nest 2 instructions
                           // are if-then-else
      " mrseq r0, msp \n"  // Make R0 point to main stack pointer
      " mrsne r0, psp \n"  // Make R0 point to process stack pointer
      " b STACK_DUMP \n"   // Off to C land
  );
}
#else
void HardFault_Handler(void) {
  error_shutdown("Internal error", "(HF)", NULL, NULL);
}
#endif

void MemManage_Handler_MM(void) {
  error_shutdown("Internal error", "(MM)", NULL, NULL);
}

void MemManage_Handler_SO(void) {
  error_shutdown("Internal error", "(SO)", NULL, NULL);
}

void BusFault_Handler(void) {
  error_shutdown("Internal error", "(BF_firmware)", NULL, NULL);
}

void UsageFault_Handler(void) {
  error_shutdown("Internal error", "(UF)", NULL, NULL);
}

__attribute__((noreturn)) void reboot_to_bootloader() {
  reboot_to_boot();
  for (;;)
    ;
}

void SVC_C_Handler(uint32_t *stack) {
  uint8_t svc_number = ((uint8_t *)stack[6])[-2];
  switch (svc_number) {
    case SVC_ENABLE_IRQ:
      HAL_NVIC_EnableIRQ(stack[0]);
      break;
    case SVC_DISABLE_IRQ:
      HAL_NVIC_DisableIRQ(stack[0]);
      break;
    case SVC_SET_PRIORITY:
      NVIC_SetPriority(stack[0], stack[1]);
      break;
#ifdef SYSTEM_VIEW
    case SVC_GET_DWT_CYCCNT:
      cyccnt_cycles = *DWT_CYCCNT_ADDR;
      break;
#endif
    case SVC_SHUTDOWN:
      shutdown_privileged();
      for (;;)
        ;
      break;
    case SVC_RESET_SYSTEM:
      HAL_NVIC_SystemReset();
      while (1)
        ;
      break;
    default:
      stack[0] = 0xffffffff;
      break;
  }
}

__attribute__((naked)) void SVC_Handler(void) {
  __asm volatile(
      " tst lr, #4    \n"    // Test Bit 3 to see which stack pointer we should
                             // use.
      " ite eq        \n"    // Tell the assembler that the nest 2 instructions
                             // are if-then-else
      " mrseq r0, msp \n"    // Make R0 point to main stack pointer
      " mrsne r0, psp \n"    // Make R0 point to process stack pointer
      " b SVC_C_Handler \n"  // Off to C land
  );
}

// MicroPython builtin stubs
#include "py/builtin.h"
mp_import_stat_t mp_import_stat(const char *path) {
  return MP_IMPORT_STAT_NO_EXIST;
}

mp_obj_t mp_builtin_open(uint n_args, const mp_obj_t *args, mp_map_t *kwargs) {
  return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(mp_builtin_open_obj, 1, mp_builtin_open);

// Dummy implementation required by ports/stm32/gccollect.c.
// The normal version requires MICROPY_ENABLE_SCHEDULER which we don't use.
void soft_timer_gc_mark_all(void) {}

// ui
#define X_FONT_SMALL           (&lv_font_ping_fang_regular_20)
#define X_FONT_BOLD            (&lv_font_ping_fang_medium_32)
#define X_FONT_BOLD_SMALL      (&lv_font_ping_fang_medium_20)
#define X_FONT_BOLD_LARGE      (&lv_font_ping_fang_medium_44)

#define X_BAR_HEIGHT           56
#define X_RADIUS               12
#define X_CONTENT_PAD          24
#define X_CONTENT_SPACE        20
#define X_BOTTOM_BUTTON_HEIGHT 76
#define X_BOTTOM_BUTTON_RADIUS 12

#define X_CONTAINER_PAD 20
#define X_CONTAINER_GAP 16

#define X_COLOR_BLACK 0x1D1D1D
#define X_COLOR_WHITE 0xFFFFFF
#define X_COLOR_GREEN 0x00FE33
#define X_COLOR_GRARY 0x282828

#define X_COLOR_LIGHT_GRAY 0xBFBFBF
#define X_COLOR_DARK_GREEN 0x0B3D15

static lv_obj_t* ui_btn_create_ex(lv_obj_t* parent, char* text, uint32_t bg_color, uint32_t text_color) {
    lv_obj_t* btn = lv_btn_create(parent);
    lv_obj_set_style_radius(btn, X_RADIUS, LV_PART_MAIN);
    lv_obj_set_style_bg_color(btn, lv_color_hex(bg_color), LV_PART_MAIN);
    lv_obj_t* label = lv_label_create(btn);
    lv_obj_center(label);
    lv_label_set_text(label, text);
    lv_obj_set_style_text_color(label, lv_color_hex(text_color), LV_PART_MAIN);
    return btn;
}

#define ui_btn_create(parent, text) ui_btn_create_ex(parent, text, X_COLOR_DARK_GREEN, X_COLOR_GREEN)
#define ui_primary_btn_create(parent, text) ui_btn_create(parent, text)
#define ui_secondary_btn_create(parent, text) ui_btn_create_ex(parent, text, X_COLOR_GRARY, X_COLOR_WHITE)

static void enter_update_mode_cb(lv_event_t* e) {
  (void)e;
  reboot_to_boot();
}

static void shutdown_cb(lv_event_t* e) {
  (void)e;
  pm_power_off();
}

void system_problem_detected(void) {

  // lvgl init
  lv_init();
  lv_port_disp_init();
  lv_port_indev_init();
  lv_port_fs_init(&firmware_ctx.fs);
  lv_theme_default_init(
      NULL, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), 1, LV_FONT_DEFAULT
  );

  lv_obj_t* screen = lv_obj_create(lv_scr_act());
  lv_obj_set_style_bg_color(screen, lv_color_hex(COLOR_BLACK), LV_PART_MAIN);
  lv_obj_set_size(screen, lv_pct(100), lv_pct(100));
  lv_obj_set_style_border_width(screen, 0, LV_PART_MAIN);
  lv_obj_set_style_pad_all(screen, X_CONTENT_PAD, LV_PART_MAIN);
  lv_obj_set_style_pad_top(screen, X_BAR_HEIGHT, LV_PART_MAIN);

  // title
  lv_obj_t* obj = lv_label_create(screen);
  lv_label_set_text(obj, "System problem detected");
  lv_obj_set_width(obj, lv_pct(100));
  lv_obj_set_style_text_color(obj, lv_color_hex(X_COLOR_WHITE), LV_PART_MAIN);
  lv_obj_set_style_text_font(obj, X_FONT_BOLD_LARGE, LV_PART_MAIN);
  lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);

  lv_obj_t* container = lv_obj_create(screen);

  lv_obj_add_flag(container, LV_OBJ_FLAG_SCROLLABLE);

  lv_obj_set_style_radius(container, X_RADIUS, LV_PART_MAIN);
  lv_obj_set_style_bg_color(container, lv_color_hex(X_COLOR_BLACK), LV_PART_MAIN);
  lv_obj_set_flex_flow(container, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_style_border_width(container, 0, LV_PART_MAIN);
  lv_obj_set_size(container, lv_pct(100), LV_SIZE_CONTENT);
  lv_obj_set_style_pad_all(container, 0, LV_PART_MAIN);
  lv_obj_set_style_pad_gap(container, 8, LV_PART_MAIN);
  lv_obj_align(container, LV_ALIGN_BOTTOM_MID, 0, 0);

  // enter update mode
  lv_obj_t* btn = ui_primary_btn_create(container, "Enter update mode");

  lv_obj_set_size(btn, lv_pct(100), X_BOTTOM_BUTTON_HEIGHT);
  lv_obj_add_event_cb(btn, enter_update_mode_cb, LV_EVENT_CLICKED, NULL);

  // shutdown
  btn = ui_secondary_btn_create(container, "Shutdown");
  lv_obj_set_size(btn, lv_pct(100), X_BOTTOM_BUTTON_HEIGHT);
  lv_obj_add_event_cb(btn, shutdown_cb, LV_EVENT_CLICKED, NULL);
  while(1) {
    lv_timer_handler();
    hal_delay(1);
  }
}
