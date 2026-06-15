#ifndef __UPDATER_H__
#define __UPDATER_H__

#include <stdbool.h>

#include "fs.h"
#include "image.h"

typedef const char* (*updater_get_name_fn_t)(void);
typedef const char* (*updater_get_cur_ver_fn_t)(void);
typedef const char* (*updater_get_new_ver_fn_t)(void);
typedef const char* (*updater_get_error_fn_t)(void);
typedef int (*updater_get_processed_fn_t)(void);
typedef void (*updater_canceled_fn_t)(void);
typedef void (*updater_verify_image_fn_t)(void);
typedef void (*updater_install_image_fn_t)(void);
typedef void (*updater_install_abort_fn_t)(void);

typedef enum {
  UPDATER_STATE_NONE,
  UPDATER_STATE_ERROR,
  UPDATER_STATE_NOTIFY_APP,
  UPDATER_STATE_WAIT_BUTTON_ACK,
  UPDATER_STATE_ASK_CONFIRM,
  UPDATER_STATE_WAITING_CONFIRM,
  UPDATER_STATE_CANCELLED,
  UPDATER_STATE_CONFIRMED,
  UPDATER_STATE_VERIFY,
  UPDATER_STATE_INSTALL,
  UPDATER_STATE_FINISHED,
}updater_state_t;

typedef struct {
  updater_get_name_fn_t         get_name_fn;
  updater_get_cur_ver_fn_t      get_cur_ver_fn;
  updater_get_new_ver_fn_t      get_new_ver_fn;
  updater_get_error_fn_t        get_error_fn;
  updater_get_processed_fn_t    get_processed_fn;
  updater_verify_image_fn_t     verify_image_fn;
  updater_install_image_fn_t    install_image_fn;
  updater_install_abort_fn_t    abort_fn;
  updater_canceled_fn_t         cancelled_fn;
}updater_t;

typedef enum {
  UPDATER_ERROR_SUCCESS,
  UPDATER_ERROR_INVALID_PATH,
  UPDATER_ERROR_READ_FAILED,
  UPDATER_ERROR_UNKNOWN_IMAGE_TYPE,
  UPDATER_ERROR_INVALID_VERSION,
  UPDATER_ERROR_INVALID_STATE,
  UPDATER_ERROR_IMAGE_VERIFY_FAILED,
  UPDATER_ERROR_IMAGE_INSTALL_FAILED,
} updater_error_t;

typedef struct {
  char path[64];
  image_type_t image_type;
  char cur_ver[33];
  char new_ver[33];
  void *user_data; // the special updater ctx
  updater_error_t error;
  fs_file_t *fp;
  bool reboot_on_success;
}updating_context_t;

typedef struct {
  updater_state_t state;
  uint32_t wait_confirm_uintil;
  updater_t* updater;
  updating_context_t* updating;
}updater_context_t;

void updater_update_image(const char* path, bool reboot_on_success);
void updater_begin(updater_state_t state);
void updater_register(updater_t* updater);
void updater_failed(updater_error_t error, const char* msg);
// updater success and switch to next `state`
void updater_success(updater_state_t state);
void updater_finished(void);
const char* updater_error_format(updater_error_t error);

// updater fn
const char* updater_get_name(void);
const char* updater_get_cur_ver(void);
const char* updater_get_new_ver(void);
const char* updater_get_error(void);
int updater_get_processed(void);
void updater_canceled(void);

// updaters
// bootloader can't update self
void update_firmware(void);
void update_se(void);
void update_ble(void);
void update_iris(void);
#endif
