#ifndef _DEVICE_H_
#define _DEVICE_H_

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

typedef enum {
  PCB_V1_0,
  PCB_V1_1,
}pcb_version_t;

typedef struct __attribute__((packed)) {
  char product[2];
  char hardware[2];
  char color;
  char factory[2];
  char utc[10];
  char serial[7];
} DeviceSerialNo;

typedef struct __attribute__((packed)) {
  char serial[32];
  char cpu_info[16];
  char pre_firmware[16];
  char se_config[8];
  uint32_t st_id[3];
  bool random_key_init;
  uint8_t random_key[32];
  bool pre_shared_key_init;
  uint8_t pre_shared_key[32];
} DeviceInfomation;

void device_set_factory_mode(bool mode);
bool device_is_factory_mode(void);
void device_para_init(void);
bool device_serial_set(void);
bool device_set_serial(char *dev_serial);
bool device_cpu_firmware_set(void);
bool device_set_cpu_firmware(char *cpu_info, char *firmware_ver);
bool device_get_cpu_firmware(char **cpu_info, char **firmware_ver);
bool device_get_serial(char **serial);
char *device_get_se_config_version(void);
void device_get_enc_key(uint8_t key[32]);
bool device_set_pre_shared_key(uint8_t key[32]);
bool device_get_pre_shared_key(uint8_t key[32]);

void device_test(bool force);
void device_burnin_test(bool force);

void device_power_on(void);
void device_power_off(void);

void device_set_pcb_version(pcb_version_t version);
pcb_version_t device_get_pcb_version(void);
#define PCB_IS_V1_0() (device_get_pcb_version() == PCB_V1_0)
#define PCB_IS_V1_1() (device_get_pcb_version() == PCB_V1_1)
#endif
