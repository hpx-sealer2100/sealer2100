#include "device.h"

#include <stdbool.h>
#include <string.h>
#include <stdint.h>

#include STM32_HAL_H
#include "stm32h7xx_hal_pwr.h"
#include "stm32h7xx_hal_rtc_ex.h"

#include "common.h"
#include "flash.h"
#include "power_manager.h"
#include "rand.h"
#include "sha2.h"

#define STAY_IN_BOARDLOADER_FLAG 0x64616F62
#define STAY_IN_BOOTLOADER_FLAG 0x746F6F62

static DeviceInfomation dev_info = {0};
static bool factory_mode = false;

#define DEV_INFO_FILED_IS_INITIALIZED(info, filed_block) \
  (info.initialized & (1 << filed_block))

#define DEV_INFO_FILED_SET_INITIALIZED(info, filed_block) \
  info.initialized |= (1 << filed_block)

// device serial initialized
#define DEV_INFO_SN_INITIALIZED(info) DEV_INFO_FILED_IS_INITIALIZED(info, FLASH_OTP_DEVICE_SERIAL)
#define DEV_INFO_SN_SET_INITIALIZED(info) DEV_INFO_FILED_SET_INITIALIZED(info, FLASH_OTP_DEVICE_SERIAL)

// device random key initialized
#define DEV_INFO_RANDOM_KEY_INITIALIZED(info) \
  DEV_INFO_FILED_IS_INITIALIZED(info, FLASH_OTP_RANDOM_KEY)
#define DEV_INFO_RANDOM_KEY_SET_INITIALIZED(info) \
  DEV_INFO_FILED_SET_INITIALIZED(info, FLASH_OTP_RANDOM_KEY)

// device pre shared key initialized
#define DEV_INFO_PRE_SHARED_KEY_INITIALIZED(info) \
  DEV_INFO_FILED_IS_INITIALIZED(info, FLASH_OTP_PRE_SHARED_KEY)
#define DEV_INFO_PRE_SHARED_KEY_SET_INITIALIZED(info) \
  DEV_INFO_FILED_SET_INITIALIZED(info, FLASH_OTP_PRE_SHARED_KEY)

// device iris pre shared key initialized
#define DEV_INFO_IRIS_PRE_SHARED_KEY_INITIALIZED(info) \
  DEV_INFO_FILED_IS_INITIALIZED(info, FLASH_OTP_IRIS_PRE_SHARED_KEY)
#define DEV_INFO_IRIS_PRE_SHARED_KEY_SET_INITIALIZED(info) \
  DEV_INFO_FILED_SET_INITIALIZED(info, FLASH_OTP_IRIS_PRE_SHARED_KEY)



static bool is_valid_ascii(const uint8_t *data, uint32_t size) {
  while (size--) {
    uint8_t c = *data++;
    if (c == 0) {
      break;
    }
    if (c < ' ' || c > '~') {
      return false;
    }
  }
  return true;
}

void device_set_factory_mode(bool mode) { factory_mode = mode; }

bool device_is_factory_mode(void) { return factory_mode; }

void device_para_init(void) {
  memset(&dev_info, 0, sizeof(dev_info));

  dev_info.st_id[0] = HAL_GetUIDw0();
  dev_info.st_id[1] = HAL_GetUIDw1();
  dev_info.st_id[2] = HAL_GetUIDw2();

  if (!flash_otp_is_locked(FLASH_OTP_RANDOM_KEY)) {
    uint8_t entropy[FLASH_OTP_BLOCK_SIZE] = {0};
    random_buffer(entropy, FLASH_OTP_BLOCK_SIZE);
    ensure(
        flash_otp_write(FLASH_OTP_RANDOM_KEY, 0, entropy, FLASH_OTP_BLOCK_SIZE),
        NULL);
    ensure(flash_otp_lock(FLASH_OTP_RANDOM_KEY), NULL);
  }
  ensure(flash_otp_read(FLASH_OTP_RANDOM_KEY, 0, dev_info.random_key,
                        FLASH_OTP_BLOCK_SIZE),
         NULL);
  DEV_INFO_RANDOM_KEY_SET_INITIALIZED(dev_info);

  if (flash_otp_is_locked(FLASH_OTP_DEVICE_SERIAL)) {
    strlcpy(
      dev_info.serial,
      (char *)flash_otp_data->flash_otp[FLASH_OTP_DEVICE_SERIAL],
      sizeof(dev_info.serial)
    );

    if (is_valid_ascii((uint8_t *)dev_info.serial, FLASH_OTP_BLOCK_SIZE)) {
      DEV_INFO_SN_SET_INITIALIZED(dev_info);
    }
  }

  if (flash_otp_is_locked(FLASH_OTP_PRE_SHARED_KEY)) {
    ensure(
      flash_otp_read(FLASH_OTP_PRE_SHARED_KEY, 0, dev_info.pre_shared_key, FLASH_OTP_BLOCK_SIZE),
      NULL
    );
    DEV_INFO_PRE_SHARED_KEY_SET_INITIALIZED(dev_info);
  }

  if (flash_otp_is_locked(FLASH_OTP_IRIS_PRE_SHARED_KEY)) {
    ensure(
      flash_otp_read(FLASH_OTP_IRIS_PRE_SHARED_KEY, 0, dev_info.iris_pre_shared_key, FLASH_OTP_BLOCK_SIZE),
      NULL
    );
    DEV_INFO_IRIS_PRE_SHARED_KEY_SET_INITIALIZED(dev_info);
  }

  return;
}

bool device_serial_set(void) { return DEV_INFO_SN_INITIALIZED(dev_info); }

bool device_set_serial(char *dev_serial) {
  uint8_t buffer[FLASH_OTP_BLOCK_SIZE] = {0};

  if (DEV_INFO_SN_INITIALIZED(dev_info)) {
    return false;
  }

  if (!is_valid_ascii((uint8_t *)dev_serial, FLASH_OTP_BLOCK_SIZE - 1)) {
    return false;
  }

  // check serial
  if (!flash_otp_is_locked(FLASH_OTP_DEVICE_SERIAL)) {
    if (check_all_ones(flash_otp_data->flash_otp[FLASH_OTP_DEVICE_SERIAL],
                       FLASH_OTP_BLOCK_SIZE)) {
      strlcpy((char *)buffer, dev_serial, sizeof(buffer));
      ensure(flash_otp_write(FLASH_OTP_DEVICE_SERIAL, 0, buffer,
                             FLASH_OTP_BLOCK_SIZE),
             NULL);
      ensure(flash_otp_lock(FLASH_OTP_DEVICE_SERIAL), NULL);
      device_para_init();
      return true;
    }
  }
  return false;
}

bool device_get_serial(char **serial) {
  if (!DEV_INFO_SN_INITIALIZED(dev_info)) {
    return false;
  }
  *serial = dev_info.serial;
  return true;
}

void device_get_enc_key(uint8_t key[32]) {
  SHA256_CTX ctx = {0};

  sha256_Init(&ctx);
  sha256_Update(&ctx, (uint8_t *)dev_info.st_id, sizeof(dev_info.st_id));
  sha256_Update(&ctx, dev_info.random_key, sizeof(dev_info.random_key));
  sha256_Final(&ctx, key);
}

void device_power_on(void) {
}
void device_power_off(void) {
  pm_power_off();
}

void device_set_pcb_version(pcb_version_t version) {
  // have set
  if (flash_otp_is_locked(FLASH_OTP_PCB_VERSION)) {
    return;
  }
  uint8_t buf[FLASH_OTP_BLOCK_SIZE] = {0};
  memset(buf, 0xFF, FLASH_OTP_BLOCK_SIZE);
  buf[0] = version;
  ensure(flash_otp_write(FLASH_OTP_PCB_VERSION, 0, buf, FLASH_OTP_BLOCK_SIZE),
         NULL);
  ensure(flash_otp_lock(FLASH_OTP_PCB_VERSION), NULL);
}

pcb_version_t device_get_pcb_version(void) {
  uint8_t* addr = flash_otp_data->flash_otp[FLASH_OTP_PCB_VERSION];
  return (pcb_version_t)*addr;
}

bool device_set_pre_shared_key(uint8_t key[32]) {

  if (flash_otp_is_locked(FLASH_OTP_PRE_SHARED_KEY)) {
    return false;
  }
  if (sectrue != flash_otp_write(FLASH_OTP_PRE_SHARED_KEY, 0, key, FLASH_OTP_BLOCK_SIZE)) {
    return false;
  }
  if (sectrue != flash_otp_lock(FLASH_OTP_PRE_SHARED_KEY)) {
    return false;
  }
  device_para_init();
  return true;
}

bool device_get_pre_shared_key(uint8_t key[32]) {
  if (!DEV_INFO_PRE_SHARED_KEY_INITIALIZED(dev_info)) {
    return false;
  }
  memcpy(key, dev_info.pre_shared_key, FLASH_OTP_BLOCK_SIZE);
  return true;
}

bool device_set_iris_pre_shared_key(uint8_t key[32]) {
  if (flash_otp_is_locked(FLASH_OTP_IRIS_PRE_SHARED_KEY)) {
    return false;
  }
  if (sectrue != flash_otp_write(FLASH_OTP_IRIS_PRE_SHARED_KEY, 0, key, FLASH_OTP_BLOCK_SIZE)) {
    return false;
  }
  if (sectrue != flash_otp_lock(FLASH_OTP_IRIS_PRE_SHARED_KEY)) {
    return false;
  }
  device_para_init();
  return true;
}

bool device_get_iris_pre_shared_key(uint8_t key[32]) {
  if (!DEV_INFO_IRIS_PRE_SHARED_KEY_INITIALIZED(dev_info)) {
    return false;
  }
  memcpy(key, dev_info.iris_pre_shared_key, FLASH_OTP_BLOCK_SIZE);
  return true;
}

bool device_is_pre_shared_key_set(void) {
  return DEV_INFO_PRE_SHARED_KEY_INITIALIZED(dev_info);
}

bool device_is_iris_pre_shared_key_set(void) {
  return DEV_INFO_IRIS_PRE_SHARED_KEY_INITIALIZED(dev_info);
}

void device_set_run_mode(run_mode_t mode) {
  HAL_PWR_EnableBkUpAccess();
  RTC_HandleTypeDef rtc;
  rtc.Instance = RTC;
  uint32_t flag = 0;
  if (mode == RUN_MODE_BOARDLOADER) {
    flag = STAY_IN_BOARDLOADER_FLAG;
  } else if (mode == RUN_MODE_BOOTLOADER) {
    flag = STAY_IN_BOOTLOADER_FLAG;
  }
  HAL_RTCEx_BKUPWrite(&rtc, RTC_BKP_DR0, flag);
  HAL_PWR_DisableBkUpAccess();
}

run_mode_t device_get_run_mode(void) {
  HAL_PWR_EnableBkUpAccess();
  RTC_HandleTypeDef rtc;
  rtc.Instance = RTC;
  uint32_t flag = HAL_RTCEx_BKUPRead(&rtc, RTC_BKP_DR0);
  // clear flag
  HAL_RTCEx_BKUPWrite(&rtc, RTC_BKP_DR0, 0);
  HAL_PWR_DisableBkUpAccess();
  if (flag == STAY_IN_BOARDLOADER_FLAG) {
    return RUN_MODE_BOARDLOADER;
  } else if (flag == STAY_IN_BOOTLOADER_FLAG) {
    return RUN_MODE_BOOTLOADER;
  }
  return RUN_MODE_FIRMWARE;
}
