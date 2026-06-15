#ifndef _DEVICE_H_
#define _DEVICE_H_

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

typedef enum {
  PCB_V1_0,
  PCB_V1_1,
}pcb_version_t;

typedef enum {
  RUN_MODE_BOARDLOADER,
  RUN_MODE_BOOTLOADER,
  RUN_MODE_FIRMWARE,
} run_mode_t;

typedef struct __attribute__((packed)) {
  uint32_t initialized; // use bits operation record filed initialized or not
  char serial[32];
  char cpu_info[16];
  uint32_t st_id[3];
  uint8_t random_key[32];
  uint8_t pre_shared_key[32];
  uint8_t iris_pre_shared_key[32];
} DeviceInfomation;

void device_set_factory_mode(bool mode);
bool device_is_factory_mode(void);
void device_para_init(void);
bool device_serial_set(void);
bool device_set_serial(char *dev_serial);
bool device_get_serial(char **serial);
void device_get_enc_key(uint8_t key[32]);
bool device_set_pre_shared_key(uint8_t key[32]);
bool device_get_pre_shared_key(uint8_t key[32]);
bool device_set_iris_pre_shared_key(uint8_t key[32]);
bool device_get_iris_pre_shared_key(uint8_t key[32]);

bool device_is_pre_shared_key_set(void);
bool device_is_iris_pre_shared_key_set(void);

void device_power_on(void);
void device_power_off(void);

void device_set_run_mode(run_mode_t mode);
run_mode_t device_get_run_mode(void);

void device_set_pcb_version(pcb_version_t version);
pcb_version_t device_get_pcb_version(void);
#define PCB_IS_V1_0() (device_get_pcb_version() == PCB_V1_0)
#define PCB_IS_V1_1() (device_get_pcb_version() == PCB_V1_1)
#endif
