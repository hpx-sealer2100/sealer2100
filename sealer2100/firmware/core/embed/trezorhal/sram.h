#ifndef __SRAM_H__
#define __SRAM_H__

#include <stdint.h>

// SRAM: SRAM1:128KB, SRAM2:128KB, SRAM3:32KB
#define SRAM_SIZE ((uint32_t)(128+128+32)*1024)
#define SRAM_BASE_ADDRESS ((uint32_t)(0x30000000))
#define SRAM_END ((SRAM_BASE_ADDRESS) + (SRAM_SIZE))

#define DEF_SRAM(name, begin, size)             \
  enum {                                           \
    SRAM_##name##_ADDRESS = (begin),            \
    SRAM_##name##_SIZE    = (size),               \
    SRAM_##name##_END     = ((begin) + (size)), \
  }

// buffers used for dma

// spi ble buffers, 256 bytes each
DEF_SRAM(BLE_RECV, SRAM_BASE_ADDRESS, 256);
DEF_SRAM(BLE_SEND, SRAM_BLE_RECV_END, 256);

// camera buffer, 2KB, 1 line each, the camera max line is 640*480, enough
DEF_SRAM(CAMERA, SRAM_BLE_SEND_END, 2 * 1024);

DEF_SRAM(LOG, SRAM_CAMERA_END, 1024);


#endif
