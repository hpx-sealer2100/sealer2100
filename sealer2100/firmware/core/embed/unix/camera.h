#ifndef UNIX_CAMERA_H
#define UNIX_CAMERA_H
#include <stdint.h>
#include <stdbool.h>
enum {
  CAMERA_POWER_OFF,
  CMAERA_POWER_ON,
};

enum {
  CAMERA_QR_TYPE_NUMBER = 1,
  CAMERA_QR_TYPE_STRING = 2,
  CAMERA_QR_TYPE_BYTES  = 4,
};

static uint8_t __camera_buffer__[1 * 1024 * 1024] = {0};

bool camera_init(int width, int height){return 0;};
void camera_start(void){};
// only suspend camera capturing
void camera_suspend(void){};
// resume camera capturing
void camera_resume(void){};
void camera_stop(void){};
void camera_deinit(void){};
void camera_led_on(void){};
void camera_led_off(void){};
void camera_work_normal(const char* name){};

bool camera_is_power_on(void){return 0;};

bool camera_is_captured(void){return 0;};
bool camera_is_grayscale_done(void) {return 0;};
void camera_scan_reset(void) {};

int camera_scan_qrcode(uint8_t qrcode[1024 + 1], int* type) {return -1;};
uint8_t* camera_buffer(void){return __camera_buffer__;};
#endif
