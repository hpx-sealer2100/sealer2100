#ifndef TREZORHAL_FMC_SDRAMH_
#define TREZORHAL_FMC_SDRAMH_
#include "stdint.h"

// 32 MB
#define SDRAM_SIZE ((uint32_t)32 * 1024 * 1024)
#define SDRAM_BASE_ADDRESS ((uint32_t)0xD0000000)
#define SDRAM_END ((SDRAM_BASE_ADDRESS) + (SDRAM_SIZE))

#define DEF_SDRAM(name, begin, size)             \
  enum {                                           \
    SDRAM_##name##_ADDRESS = (begin),            \
    SDRAM_##name##_SIZE    = (size),               \
    SDRAM_##name##_END     = ((begin) + (size)), \
  }

#define DEF_REVERSE_SDRAM(name, end, size) \
  enum {                                   \
    SDRAM_##name##_ADDRESS = (end - size), \
    SDRAM_##name##_SIZE    = (size),       \
    SDRAM_##name##_END     = (end),        \
  }

#define DEF_SDRAM_RANGE(name, begin, end) \
  enum { \
    SDRAM_##name##_ADDRESS = (begin), \
    SDRAM_##name##_SIZE    = ((end) - (begin)), \
    SDRAM_##name##_END     = (end), \
  }

// LTDC buffer, 800*480, rgb565 pixel format, 1.5MB > 2*(2*800*480)
// 2 buffers, each 750KB, we used as rendered and rendering buffer
DEF_SDRAM(LTDC, SDRAM_BASE_ADDRESS, 2*(768*1024));

// lvgl buffer, 800*480, double buffer, rgb565 pixel format, 1.5MB > 2*(2*800*480)
// bootloader and firmware all use the same buffer for lvgl draw
DEF_SDRAM(LVGL, SDRAM_LTDC_END, 2*(768*1024));

// camera buffer, for image processing, 512KB > 450KB
// we capture image in square, the max 480*480, 2*(480*480) = 450KB
DEF_SDRAM(CAMERA, SDRAM_LVGL_END, 512 * 1024);

// grayscale image buffer, for image processing, 512KB
// same size as camera buffer
DEF_SDRAM(GRAY, SDRAM_CAMERA_END, 512 * 1024);

// code buffer, for firmware code, 3MB, from D1D00000 ~ D2000000
// used in ld script
DEF_REVERSE_SDRAM(CODE, SDRAM_END, 3*1024*1024);

// micropython heap buffer, 2MB
DEF_REVERSE_SDRAM(MP, SDRAM_CODE_ADDRESS, 2*1024*1024);

// user heap buffer
// all left memory for user heap buffer
DEF_SDRAM_RANGE(USER_HEAP, SDRAM_GRAY_END, SDRAM_MP_ADDRESS);

int sdram_init(void);

#endif
