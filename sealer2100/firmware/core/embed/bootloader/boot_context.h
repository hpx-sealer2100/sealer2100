#ifndef __BOOT_CONTEXT_H__
#define __BOOT_CONTEXT_H__

#include "bootui.h"
#include "updater.h"
#include <stdint.h>

#include "fs.h"

// 10 minutes, if no operation, power off
#define IDLE_TIMEOUT_MS (1000 * 60 * 10)

typedef enum {
  BUILD_MODE_NONE,
  BUILD_MODE_DEV,
  BUILD_MODE_PRODUCTION,
} build_mode_t;

typedef enum {
  HOST_CHANNEL_NONE,    // invalid channel
  HOST_CHANNEL_BLE,     // data come from ble
  HOST_CHANNEL_USB,     // data come from usb
}host_channel_t;

typedef enum {
  HOST_CHANNEL_STATE_IDLE,
  HOST_CHANNEL_STATE_READING,
  HOST_CHANNEL_STATE_PROCESSED,
}host_channel_state_t;

// a fs message timeout callback
typedef void (*fs_msg_timeout_fn_t)(void);

// boot operating file
typedef struct {
  char path[256]; // the full path of bootloader file operating
  fs_file_t* file;
}boot_file_t;

typedef struct {
  build_mode_t build;
  uint32_t last_alive_time_ms;


  struct {
    host_channel_t channel;
    host_channel_state_t state;
  }host;

  struct {
    bool valided;
  } firmware;

  struct {
    char name[17];
    char version[17];
  } ble;

  struct {
    char version[33];
  } iris;

  struct {
    bool busy; // may be writing/reading file or upgrading
    bool usb_connected;
    bool charging;
    uint8_t soc;
  }device;

  struct {
    uint32_t last_time_ms;
    uint32_t lvgl_last_active_time;
    // fs message timeout
    uint32_t timeout_ms;
    fs_msg_timeout_fn_t fs_msg_timeout_fn;
  }monitor;

  // file system
  fs_t fs;

  // ui context
  ui_context_t* ui;

  // boot operating file
  boot_file_t *file_operating;

  // updater context
  updater_context_t updater;
}boot_context_t;

extern boot_context_t boot_ctx;

void boot_context_init(void);

// update the last alive time to current time
void boot_alive_time_touch(void);

void boot_fs_msg_monitor_init(fs_msg_timeout_fn_t timeout_fn, uint32_t timeout_ms);
void boot_fs_msg_monitor_reset(void);
void boot_fs_msg_monitor_touch(void);
// poll for fs message timeout
void boot_fs_msg_monitor_poll(void);
#endif
