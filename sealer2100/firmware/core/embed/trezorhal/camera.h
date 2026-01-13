#ifndef TREZORHAL_CAMERA_H
#define TREZORHAL_CAMERA_H
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

bool camera_init(int width, int height);
void camera_work_normal(const char* name);
void camera_start(void);
// only suspend camera capturing
void camera_suspend(void);
// resume camera capturing
void camera_resume(void);
void camera_stop(void);

void camera_deinit(void);
void camera_led_on(void);
void camera_led_off(void);

bool camera_is_power_on(void);

// 摄像头的capture 和 二维码的 grayscale 状态并不完全等价
// 代码实现中 captured 一定是 grayscale_done
// grayscale 需要主动消耗，如果不消耗则无法触发界面刷新
bool camera_is_captured(void);
bool camera_is_grayscale_done(void);

int camera_scan_qrcode(uint8_t qrcode[1024 + 1], int* type);
void camera_scan_reset(void);

uint8_t* camera_buffer(void);
#endif
