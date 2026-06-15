#include "fsm_msg_management.h"

#include <stdbool.h>
#include <stdio.h>

#include "battery.h"
#include "boot_context.h"
#include "common.h"
#include "device.h"
#include "flash.h"
#include "fsm.h"
#include "messages-common.pb.h"
#include "messages.pb.h"
#include "se.h"
#include "version.h"
#include "rand.h"
#include "log.h"

void get_features(Features* feature) {
  feature->has_vendor = true;
  strcpy(feature->vendor, "hypermate.com");

  feature->has_model = true;
  strcpy(feature->model, "T");

  feature->has_bootloader_mode = true;
  feature->bootloader_mode = true;

  feature->has_bootloader_version = true;
  sprintf(feature->bootloader_version, "%d.%d.%d", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);

  feature->major_version = VERSION_MAJOR;
  feature->minor_version = VERSION_MINOR;
  feature->patch_version = VERSION_PATCH;


  if (boot_ctx.firmware.valided) {
    const image_firmware_t* const fw = FIRMWARE;

    feature->firmware_present = true;
    feature->has_fw_major = true;
    feature->fw_major = fw->header.version.major;
    feature->has_fw_minor = true;
    feature->fw_minor = fw->header.version.minor;
    feature->has_fw_patch = true;
    feature->fw_patch = fw->header.version.patch;
    feature->has_hypermate_version = true;
    image_version_format(fw->header.version, feature->hypermate_version);
  } else {
    feature->firmware_present = false;
  }

  // `ble_enable`
  feature->has_ble_enable = true;
  feature->ble_enable = true;

  if (strlen(boot_ctx.ble.name)) {
    feature->has_ble_name = true;
    strcpy(feature->ble_name, boot_ctx.ble.name);
  }
  if (strlen(boot_ctx.ble.version)) {
    feature->has_ble_ver = true;
    strcpy(feature->ble_ver, boot_ctx.ble.version);
  }

  {
    char version[17] = {0};
    if (se_is_running_app() && se_get_version(version) == 0) {
      feature->has_se_ver = true;
      strcpy(feature->se_ver, version);
    }
  }

  {
    char *sn = NULL;
    if (device_get_serial(&sn)) {
      feature->has_serial_no = true;
      strcpy(feature->serial_no, sn);
    }
  }
  {
    uint32_t soc = battery_read_SOC();
    feature->has_battery = true;
    feature->battery = soc;
  }
  {
    if (strlen(boot_ctx.iris.version)) {
      feature->has_iris_version = true;
      strcpy(feature->iris_version, boot_ctx.iris.version);
    }
  }
}

void fsm_msgInitialize(const Initialize *msg){
  (void)msg;
  RESP_INIT(Features);
  get_features(resp);
  msg_write(MessageType_MessageType_Features, resp);
}
void fsm_msgPing(const Ping *msg){
  (void)msg;
  fsm_sendSuccess(NULL);
}
void fsm_msgGetFeatures(const GetFeatures *msg){
  (void)msg;
  RESP_INIT(Features);
  get_features(resp);
  msg_write(MessageType_MessageType_Features, resp);
}
void fsm_msgEndSession(const EndSession *msg){
  (void)msg;
  fsm_sendSuccess(NULL);
}
void fsm_msgDeviceInfoSettings(const DeviceInfoSettings *msg){
  UNSUPPORT_IN_PRODUCTION_BUILD();
  if (msg->has_serial_no) {
    CHECK_PARAM(strlen(msg->serial_no) > 0, "SN not provide");
    if (!device_set_serial((char*)msg->serial_no) || se_set_sn((const uint8_t*)msg->serial_no, strlen(msg->serial_no))) {
        fsm_sendFailure(FailureType_Failure_ProcessError, "Set serial failed");
        return;
    }

    fsm_sendSuccess("Set serial success");
    return;
  }
  fsm_sendFailure(FailureType_Failure_DataError, NULL);
}
void fsm_msgReadSEPublicKey(const ReadSEPublicKey *msg){
  (void)msg;
  RESP_INIT(SEPublicKey);
  if (se_get_dev_pubkey(resp->public_key.bytes) != 0) {
    fsm_sendFailure(FailureType_Failure_ProcessError, "SE read public key failed");
    return;
  }
  resp->public_key.size = 65;
  msg_write(MessageType_MessageType_SEPublicKey, resp);
}
void fsm_msgWriteSEPublicCert(const WriteSEPublicCert *msg) {
  UNSUPPORT_IN_PRODUCTION_BUILD();
  CHECK_PARAM(msg->public_cert.size > 0, "Cert not provide");
  life_cycle_t lcs;
  if (!se_is_running_app() && se_get_life_cycle(&lcs) != 0 && lcs != LCS_FACTORY) {
    fsm_sendFailure(FailureType_Failure_ProcessError, "SE invalid state");
    return;
  }
  if (se_write_certificate(msg->public_cert.bytes, msg->public_cert.size) != 0) {
    fsm_sendFailure(FailureType_Failure_ProcessError, "SE write cert failed");
    return;
  }
  fsm_sendSuccess("write SE cert success");
}
void fsm_msgReadSEPublicCert(const ReadSEPublicCert *msg){
  RESP_INIT(SEPublicCert);
  size_t len = sizeof(resp->public_cert.bytes);
  if (se_read_certificate(resp->public_cert.bytes, &len) != 0) {
    fsm_sendFailure(FailureType_Failure_ProcessError, "SE read cert failed");
    return;
  }
  resp->public_cert.size = len;
  msg_write(MessageType_MessageType_SEPublicCert, resp);
}
void fsm_msgSESignMessage(const SESignMessage *msg){
  (void)msg;
  RESP_INIT(SEMessageSignature);
  CHECK_PARAM(msg->message.size > 0, "Message not provide");
  if (se_sign_message((uint8_t*)msg->message.bytes, msg->message.size, resp->signature.bytes) != 0) {
    fsm_sendFailure(FailureType_Failure_ProcessError, "SE sign message failed");
    return;
  }
  resp->signature.size = 64;
  msg_write(MessageType_MessageType_SEMessageSignature, resp);
}
void fsm_msgSEInitialize(const SEInitialize *msg){
  (void)msg;
  UNSUPPORT_IN_PRODUCTION_BUILD();

  // `erase storage` will erase personalization data, SN, pre shared key, device key and so on.
  // so we need to reset connection to ensure secure channel is not used
  se_conn_reset();
  if (se_erase_storage() != 0) {
    fsm_sendFailure(FailureType_Failure_ProcessError, "SE erase storage failed");
    return;
  }
  hal_delay(10);
  se_reboot();
  hal_delay(50);

  // se have reboot, ourside we need reset connection
  se_conn_reset();

  // for test in develop mode
  // erase `storage`
  flash_erase(FLASH_SECTOR_STORAGE_1);
  flash_erase(FLASH_SECTOR_STORAGE_2);
  // erase `otp`
  flash_otp_erase();
  flash_otp_init();

  if (!se_is_running_app()) {
    fsm_sendFailure(FailureType_Failure_ProcessError, "SE invalid state");
    return;
  }
  life_cycle_t lcs;
  if (se_get_life_cycle(&lcs) != 0) {
    fsm_sendFailure(FailureType_Failure_ProcessError, "SE get lifecycle failed");
    return;
  }
  LOG_DEBUG("boot", "lcs: %d", lcs);
  if (lcs != LCS_FACTORY) {
    fsm_sendFailure(FailureType_Failure_ProcessError, "SE invalid state");
    return;
  }
  if (se_gen_dev_keypair() != 0) {
    fsm_sendFailure(FailureType_Failure_ProcessError, "SE gen keypair failed");
    return;
  }


  uint8_t key[32] = {0};
  random_buffer(key, sizeof(key));
  if (se_set_sheared_key(key, sizeof(key)) != 0 ||!device_set_pre_shared_key(key)) {
    memzero(key, sizeof(key));
    fsm_sendFailure(FailureType_Failure_ProcessError, "SE bind key failed");
    return;
  }
  // ok, bind key success
  se_handshake(key, sizeof(key));
  memzero(key, sizeof(key));
  fsm_sendSuccess("SE initialize success");
}
void fsm_msgSEInitializeDone(const SEInitializeDone *msg){
  (void)msg;
  UNSUPPORT_IN_PRODUCTION_BUILD();
  life_cycle_t lcs;
  if (!se_is_running_app() && se_get_life_cycle(&lcs) != 0 && lcs != LCS_FACTORY) {
    fsm_sendFailure(FailureType_Failure_ProcessError, "SE invalid state");
    return;
  }

  if (se_switch_life_cycle() != 0) {
    fsm_sendFailure(FailureType_Failure_ProcessError, "SE switch lifecycle failed");
    return;
  }

  // restart se to enter the `USER` lifecyle
  if (se_reboot() != 0) {
    fsm_sendFailure(FailureType_Failure_ProcessError, "SE reboot failed");
    return;
  }
  hal_delay(100);
  se_conn_reset();
  {
    uint8_t secret[32] = {0};
    if (device_get_pre_shared_key(secret)) {
      se_handshake(secret, sizeof(secret));
    }
    memzero(secret, sizeof(secret));
  }
  fsm_sendSuccess("SE initialize done success");
}
void fsm_msgSEBackToRomBoot(const SEBackToRomBoot *msg){
  (void)msg;
  UNSUPPORT_IN_PRODUCTION_BUILD();
  if (0 != se_back_to_rom_bl()) {
    fsm_sendFailure(FailureType_Failure_ProcessError, "SE back to rom boot failed");
    return;
  }
  if (se_reboot() != 0) {
    fsm_sendFailure(FailureType_Failure_ProcessError, "SE reboot failed");
    return;
  }
  hal_delay(100);
  se_conn_reset();
  fsm_sendSuccess("SE back to rom boot success");
}
void fsm_msgSEWipeUserStorage(const SEWipeUserStorage *msg){
  (void)msg;
  UNSUPPORT_IN_PRODUCTION_BUILD();
  int retry = 0;
  int rv = 0;
  uint8_t dummy[32] = {0};
  do {
    rv = se_verify_pin(dummy, sizeof(dummy));
    rv = se_get_pin_retry(&retry);
  } while (rv == 0 && retry);

  if (0 != se_wipe_user_storage()) {
      fsm_sendFailure(FailureType_Failure_ProcessError, "SE wipe user storage failed");
      return;
  }
  flash_erase(FLASH_SECTOR_STORAGE_1);
  flash_erase(FLASH_SECTOR_STORAGE_2);
  fsm_sendSuccess("SE wipe user storage success");
}
