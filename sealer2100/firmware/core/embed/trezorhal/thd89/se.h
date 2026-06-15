#ifndef _SE_THD89_H_
#define _SE_THD89_H_
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

/// commands
enum {
  // 设备相关指令
  CMD_ID_GET_VERSION = 0x00,
  CMD_ID_GET_STATE = 0x01,
  CMD_ID_GET_LIFE_CYCLE = 0x02,
  CMD_ID_GET_SN = 0x03,
  CMD_ID_GET_DEV_PUBKEY = 0x04,
  CMD_ID_GET_DEV_CERT_LENGTH = 0x05,
  CMD_ID_GET_DEV_CERT = 0x06,
  CMD_ID_DEV_SIGN = 0x07,

  // PIN 相关指令
  CMD_ID_HAS_PIN = 0x08,
  CMD_ID_SET_PIN = 0x09,
  CMD_ID_VERIFY_PIN = 0x0A,
  CMD_ID_CHANGE_PIN = 0x0B,
  CMD_ID_RESET_PIN = 0x0C,
  CMD_ID_GET_PIN_MAX_RETRY = 0x0D,
  CMD_ID_GET_PIN_RETRY = 0x0E,
  CMD_ID_FORGET_PIN = 0x0F,
  CMD_ID_SET_USER_PIN_MAX_RETRY = 0xD8,


  // 管理指令
  CMD_ID_REBOOT = 0x10,
  CMD_ID_LAUNCH = 0x11,
  CMD_ID_USER_STORAGE_SIZE = 0x12,
  CMD_ID_WIPE_USER_STORAGE = 0x13,
  CMD_ID_PROTECT_USER_STORAGE = 0x14,

  // 文件指令
  CMD_ID_WRITE_FILE = 0x20,
  CMD_ID_READ_FILE = 0x21,
  CMD_ID_DELETE_FILE = 0x22,
  CMD_ID_GET_FILE_SIZE = 0x23,
  CMD_ID_SET_FILE_ACCESS = 0x24,

  // 密码算法指令
  CMD_ID_RANDOM = 0x30,
  CMD_ID_GEN_SECRET = 0x31,
  CMD_ID_GEN_KEY = 0x32,
  CMD_ID_SET_KEY = 0x33,
  CMD_ID_GEN_KEY_PAIR = 0x34,
  CMD_ID_GET_PUB_KEY = 0x35,
  CMD_ID_DERIVE_KEY = 0x36,

  CMD_ID_ENCRYPT_SYM = 0x40,
  CMD_ID_DECRYPT_SYM = 0x41,
  CMD_ID_ENCRYPT_ASYM = 0x42,
  CMD_ID_DECRYPT_ASYM = 0x43,
  CMD_ID_CMAC = 0x44,
  CMD_ID_HASH = 0x45,
  CMD_ID_HMAC = 0x46,
  CMD_ID_SIGN = 0x47,
  CMD_ID_VERIFY = 0x48,
  CMD_ID_ECDH = 0x49,

  // boot 指令
  CMD_ID_VERIFY_APP = 0x90,
  CMD_ID_INSTALL_APP = 0x91,

  CMD_ID_ROM_BL = 0xEF,

  // 生产用指令
  CMD_ID_ERASE_STORAGE = 0x80,
  CMD_ID_SWITCH_LIFE_CYCLE = 0x81,
  CMD_ID_SET_SN = 0x82,
  CMD_ID_SET_PRE_SHARED_KEY = 0x83,
  CMD_ID_GEN_DEV_KEY = 0x84,
  CMD_ID_SET_DEV_CERT = 0x85,
};

/// life cycle object states
typedef enum {
  LCS_FACTORY = 0,
  LCS_USER = 1,

  LCS_UNKNOWN = 0xFF,
}life_cycle_t;

/// which state runing
typedef enum {
  STATE_BOOTLOADER,
  STATE_APP,
}se_state_t;

enum {
  USER_OBJ_OP_EXECUTE = 0x01,
  USER_OBJ_OP_READ = 0x02,
  USER_OBJ_OP_WRITE = 0x04,
  USER_OBJ_OP_DELETE = 0x04, // can delete when can write
};

// key types, which storead in SE, can't be readout
enum {
  KEY_TYPE_SECRET = 0x80,  // a secret key
  KEY_TYPE_AES_128 = 0x81,  // a AES 128 key
  KEY_TYPE_AES_192 = 0x82,  // a AES 192 key
  KEY_TYPE_AES_256 = 0x83,  // a AES 256 key

  KEY_TYPE_NISTP256 = 0x90,  // a Nistp256 keypair(256r1)
};

// 用户对象的起始ID
#define OID_USER_OBJ_BASE  0xF000

#define PIN_MAX_LENGTH 64

void se_init(void);
void se_conn_reset(void);
// execute command with payload and response
int se_execute_case0(uint8_t cmd_id, const uint8_t *payload, size_t payload_size, uint8_t *response, size_t *response_size);
// execute command without payload and response
int se_execute_case1(uint8_t cmd_id);
// execute command with payload, but without response
int se_execute_case2(uint8_t cmd_id, const uint8_t *payload, size_t payload_size);
// execute command without payload, but with response
int se_execute_case3(uint8_t cmd_id, uint8_t *response, size_t *response_size);

int se_handshake(const uint8_t *secret, size_t secret_len);
int se_get_life_cycle(life_cycle_t *life_cycle);
int se_get_version(char version[17]);
int se_get_sn(char serial[33]);
int se_get_running_state(se_state_t *state);

int se_get_dev_pubkey(uint8_t pubkey[65]);
int se_get_certificate_len(size_t *cert_len);
int se_read_certificate(uint8_t *cert, size_t *cert_len);
int se_sign_message(uint8_t *msg, size_t msg_len, uint8_t *signature);

int se_has_pin(bool* exist);
int se_set_pin(const uint8_t *pin, size_t pin_len);
int se_verify_pin(const uint8_t* pin, size_t pin_len);
int se_change_pin(const uint8_t *old_pin, size_t old_pin_len, const uint8_t *new_pin, size_t new_pin_len);
// tell SE forget verified state, aka `logout`
int se_forget_pin(void);
int se_set_pin_user_max_retry(int max_retry);
int se_get_pin_max_retry(int* max_retry);
int se_get_pin_retry(int* retry);
int se_reset_pin(void);

// 系统指令
int se_reboot(void);
int se_launch(se_state_t state);
int se_back_to_rom_bl(void);
int se_wipe_user_storage(void); // need pin verified
int se_user_storage_size(size_t *size);

/**
 * make user storage protected
 *
 * need call this function when recover/import wallet done
 *
 * user may force reboot device when set PIN, but not finish recover/import wallet
 * in this case, if the user storage without this state:
 *   the user storage in SE have been protected by PIN
 *   the user try recover/imort wallet again, the SE wipe will faile, and can't set PIN again
 *
 * to slove this case, add a `user storage protected` state,
 * user storage with this state:
 *   the user storage protected by PIN only when this state have been turn on
 *
 *  need call this function when recover/imort wallet done
 */
int se_protect_user_storage(void);

// 文件指令
int se_write_file(uint16_t id, const uint8_t *data, size_t data_len);
int se_read_file(uint16_t id, uint8_t *data, size_t *data_len);
int se_delete_file(uint16_t id);
int se_get_file_size(uint16_t id, size_t *size);
int se_set_file_access(uint16_t id, uint8_t access);

// 密码算法指令
int se_random(size_t len, uint8_t *rnd);
int se_gen_secret(uint16_t fid, size_t len);
int se_gen_sym_key(uint16_t fid, uint8_t key_type);
int se_gen_keypair(uint16_t fid, uint8_t key_type);
int se_get_pubkey(uint16_t fid, uint8_t *pk, size_t *pk_len);
int se_cmac(uint16_t fid, const uint8_t *msg, size_t msg_len, uint8_t *cmac);
int se_hmac(uint16_t fid, const uint8_t *msg, size_t msg_len, uint8_t *hmac);
int se_ecdh(uint16_t fid, const uint8_t *pk, size_t pk_len, uint8_t *secret);

// boot 下的指令
int se_verify_app(void);
int se_install_app(size_t index, const uint8_t* block, size_t block_size);

// se factory function
int se_erase_storage(void);
int se_switch_life_cycle(void);
int se_set_sn(const uint8_t *sn, size_t sn_len);
int se_set_sheared_key(const uint8_t *key, size_t key_len);
int se_gen_dev_keypair(void);
int se_write_certificate(const uint8_t *cert, size_t cert_len);

// helper function
static inline bool se_is_running_bootloader(void) {
  se_state_t state;
  if (0 != se_get_running_state(&state)) {
    return false;
  }
  return state == STATE_BOOTLOADER;
}
static inline bool se_is_running_app(void) {
  se_state_t state;
  if (0 != se_get_running_state(&state)) {
    return false;
  }
  return state == STATE_APP;
}

int se_ping(void);
bool se_check_app_binary(const uint8_t *binary, size_t binary_len);
void se_binary_version(const uint8_t *binary, char version[17]);

// helper user function
int se_set_user_pin(uint8_t pin[32]);
int se_verify_user_pin(uint8_t pin[32]);
int se_set_storage_kekiv(uint8_t keyiv[32]);
int se_get_storage_kekiv(uint8_t keyiv[32]);
#define se_lock() se_forget_pin()

#endif
