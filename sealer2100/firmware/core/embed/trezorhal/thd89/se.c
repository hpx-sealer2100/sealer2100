#include "se.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stddef.h>
#include <stdio.h>

#include "log.h"
#include "thd89.h"
#include "se_spi.h"
#include "alignment.h"
#include "sha2.h"

#define MODULE "SE"

enum {
  CMD_PARAM_OLD_PIN = 0x00,
  CMD_PARAM_NEW_PIN = 0x01,
};

typedef enum {
  RESP_CODE_SUCCESS = 0,
  RESP_CODE_INTERNAL_FAILED,
  RESP_CODE_INVALID_STATE,
  RESP_CODE_INVALID_PARAM_DATA,
  RESP_CODE_UNSUPPORTED_COMMAND,
  RESP_CODE_OBJECT_ALREADY_EXIST,
  RESP_CODE_OBJECT_NOT_EXIST,
  RESP_CODE_PIN_LOCKED,
  RESP_CODE_ACCESS_DENIED,
  RESP_CODE_SEC_STATE_NOT_MATCH,

  RESP_CODE_VERIFY_PIN_FAIL_X = 0x80,
} response_code_t;

typedef enum {
    // can't be protected, the plain command. e.g. factory commands
    CMD_PROTECT_TYPE_PLAIN,
    // must be protected, the secure command.
    CMD_PROTECT_TYPE_SECURE,
    // maybe protected protected, the maybe secure command.
    CMD_PROTECT_TYPE_MAYBE,
} cmd_protect_type_t;

#define CHECK_FID(id) do {          \
    if (id < OID_USER_OBJ_BASE) {   \
        return 1;                   \
    }                               \
} while (0)

#define CHECK_CMD_RESULT(ret) do {  \
    if (ret != 0) {     \
        return ret;                 \
    }                               \
} while (0)

// struct for command and response
typedef struct {
  uint8_t cmd;
  uint8_t len[2];
  uint8_t payload[0];
} __attribute__((packed)) request_t;

typedef struct {
  uint8_t code;
  uint8_t len[2];
  uint8_t payload[0];
} __attribute__((packed)) response_t;

typedef struct {
  uint8_t tag;
  uint8_t len[2];
  uint8_t data[0];
}__attribute__((packed))cmd_param_t;

static inline size_t request_get_length(const request_t* req) {
  return GET_UINT16_BE(req->len, 0);
}
static inline void request_set_length(request_t* req, size_t len) {
  PUT_UINT16_BE(len, req->len, 0);
}

static inline size_t response_get_length(const response_t* resp) {
  return GET_UINT16_BE(resp->len, 0);
}
static inline void response_set_length(response_t* resp, size_t len) {
  PUT_UINT16_BE(len, resp->len, 0);
}

#define REQ_INIT_CMD(BUF, CMD) request_t *req = (void *)BUF; req->cmd = CMD
#define REQ_EMPTY_PAYLOAD(req) request_set_length(req, 0)
#define REQ_PAYLOAD(req, __payload__, __len__)      \
  do {                                              \
    if (__len__) {                              \
        memcpy(req->payload, __payload__, __len__); \
    }                                               \
    request_set_length(req, __len__);               \
  } while (0)

#define RESP_INIT(addr) response_t* resp = (void*)(addr)

static inline size_t command_size(const request_t* req) {
  return 3 + request_get_length(req);
}

static cmd_protect_type_t get_cmd_protect_type(uint8_t cmd_id) {
    if (
        cmd_id == CMD_ID_SET_PRE_SHARED_KEY ||
        cmd_id == CMD_ID_ERASE_STORAGE ||
        cmd_id == CMD_ID_GEN_DEV_KEY
    ) {
        return CMD_PROTECT_TYPE_PLAIN;
    }
    if (
        cmd_id == CMD_ID_GET_STATE ||
        cmd_id == CMD_ID_GET_VERSION ||
        cmd_id == CMD_ID_REBOOT ||
        cmd_id == CMD_ID_GET_LIFE_CYCLE
    ) {
        // commands used to get system status, can execute before set pre shared key
        // may or may not require sec channel established
        return CMD_PROTECT_TYPE_MAYBE;
    }

    return CMD_PROTECT_TYPE_SECURE;
}
const char* protect_type_str[] = {"PLAIN", "SECURE", "MAYBE"};

inline const char* get_protect_type_str(cmd_protect_type_t cpt) {
    return protect_type_str[cpt];
}

int se_get_version(char version[17]) {
    uint8_t response[16] = {0};
    size_t response_size = sizeof(response);

    int ret = se_execute_case3(CMD_ID_GET_VERSION, response, &response_size);
    CHECK_CMD_RESULT(ret);
    RESP_INIT(response);
    if (response_get_length(resp) != 4) {
        return 1;
    }
    struct {
        uint8_t major;
        uint8_t minor;
        uint8_t patch;
        uint8_t build;
    } *v = (void*)resp->payload;
    sprintf(version, "%d.%d.%d", v->major, v->minor, v->patch);
    return 0;
}

int se_get_sn(char serial[33]) {
    uint8_t response[64] = {0};
    size_t response_size = sizeof(response);

    int ret = se_execute_case3(CMD_ID_GET_SN, response, &response_size);
    CHECK_CMD_RESULT(ret);
    RESP_INIT(response);
    memcpy(serial, resp->payload, response_get_length(resp));
    return 0;
}

int se_get_running_state(se_state_t *state) {
    uint8_t response[16] = {0};
    size_t response_size = sizeof(response);

    int ret = se_execute_case3(CMD_ID_GET_STATE, response, &response_size);
    CHECK_CMD_RESULT(ret);
    RESP_INIT(response);
    *state = (se_state_t)resp->payload[0];
    return 0;
}

int se_get_life_cycle(life_cycle_t *life_cycle) {
    uint8_t response[16] = {0};
    size_t response_size = sizeof(response);

    int ret = se_execute_case3(CMD_ID_GET_LIFE_CYCLE, response, &response_size);
    CHECK_CMD_RESULT(ret);
    RESP_INIT(response);
    *life_cycle = (life_cycle_t)resp->payload[0];
    return 0;
}

int se_reboot(void) {
    return se_execute_case1(CMD_ID_REBOOT);
}

int se_launch(se_state_t state) {
    // 注意 boot状态下只能启动 app, app状态下只能启动到boot
    // boot -> app, app -> boot
    // 其他状态下会失败
    uint8_t _state_ = state;
    return se_execute_case2(CMD_ID_LAUNCH, &_state_, 1);
}
int se_back_to_rom_bl(void) {
    return se_execute_case1(CMD_ID_ROM_BL);
}

int se_wipe_user_storage(void) {
    return se_execute_case1(CMD_ID_WIPE_USER_STORAGE);
}

int se_user_storage_size(size_t *size) {
    uint8_t response[16] = {0};
    size_t response_size = sizeof(response);

    int ret = se_execute_case3(CMD_ID_USER_STORAGE_SIZE, response, &response_size);
    CHECK_CMD_RESULT(ret);
    RESP_INIT(response);
    if (response_get_length(resp) != 2) {
        return 1;
    }
    *size = GET_UINT16_BE(resp->payload, 0);
    return 0;
}

int se_protect_user_storage(void) {
    return se_execute_case1(CMD_ID_PROTECT_USER_STORAGE);
}

int se_write_file(uint16_t id, const uint8_t *data, size_t data_len) {
    if (data_len > 1024) {
        return 1;
    }

    uint8_t payload[1024+4]  = {0};
    // payload: <id:2 bytes><len:2 bytes><data>
    uint8_t *p = payload;
    // id
    PUT_UINT16_BE(id, p, 0);
    // move over `id`
    p += 2;

    // len
    PUT_UINT16_BE(data_len, p, 0);
    // move over `len`
    p += 2;

    memcpy(p, data, data_len);

    return se_execute_case2(CMD_ID_WRITE_FILE, payload, 4 + data_len);
}


int se_read_file(uint16_t id, uint8_t *data, size_t *data_len) {
    uint8_t response[1024+3] = {0};
    size_t response_size = sizeof(response);

    uint8_t payload[2] = {0};
    uint8_t *p = payload;
    // id
    PUT_UINT16_BE(id, p, 0);

    int ret = se_execute_case0(CMD_ID_READ_FILE, payload, sizeof(payload), response, &response_size);
    CHECK_CMD_RESULT(ret);
    RESP_INIT(response);
    *data_len = response_get_length(resp);
    if (*data_len) {
        memcpy(data, resp->payload, *data_len);
    }
    return 0;
}

int se_delete_file(uint16_t id) {
    uint8_t payload[2] = {0};
    uint8_t *p = payload;
    // id
    PUT_UINT16_BE(id, p, 0);

    return se_execute_case2(CMD_ID_DELETE_FILE, payload, sizeof(payload));
}

int se_get_file_size(uint16_t id, size_t *size) {
    uint8_t response[16] = {0};
    size_t response_size = sizeof(response);
    uint8_t payload[2] = {0};
    uint8_t *p = payload;
    // id
    PUT_UINT16_BE(id, p, 0);

    int ret = se_execute_case0(CMD_ID_GET_FILE_SIZE, payload, sizeof(payload), response, &response_size);
    CHECK_CMD_RESULT(ret);
    RESP_INIT(response);
    if (response_get_length(resp) != 2) {
        return 1;
    }
    *size = GET_UINT16_BE(resp->payload, 0);
    return 0;
}

int se_set_file_access(uint16_t id, uint8_t access) {
    uint8_t payload[3] = {0};
    uint8_t *p = payload;
    // id
    PUT_UINT16_BE(id, p, 0);
    // move over `id`
    p += 2;
    // access
    *p++ = access;
    return se_execute_case2(CMD_ID_SET_FILE_ACCESS, payload, sizeof(payload));
}

int se_random(size_t len, uint8_t *rnd) {
    if (len > 0xff) {
        return 1;
    }
    uint8_t response[256 + 3] = {0};
    size_t response_size = sizeof(response);
    uint8_t _len_ = len & 0xff;

    int ret = se_execute_case0(CMD_ID_RANDOM, &_len_, 1, response, &response_size);
    CHECK_CMD_RESULT(ret);
    RESP_INIT(response);
    if (response_get_length(resp) != len) {
        return 1;
    }
    memcpy(rnd, resp->payload, len);
    return 0;
}

int se_gen_secret(uint16_t fid, size_t len) {
    CHECK_FID(fid);

    // id[2]|len[1]
    uint8_t payload[3] = {0};
    uint8_t *p = payload;
    PUT_UINT16_BE(fid, p, 0);
    p += 2;
    *p++ = len & 0xFF;

    return se_execute_case2(CMD_ID_GEN_SECRET, payload, sizeof(payload));
}

int se_gen_sym_key(uint16_t fid, uint8_t key_type) {
    CHECK_FID(fid);

    // id[2]|type[1]
    uint8_t payload[3] = {0};
    uint8_t *p = payload;
    PUT_UINT16_BE(fid, p, 0);
    p += 2;
    *p++ = key_type;

    return se_execute_case2(CMD_ID_GEN_KEY, payload, sizeof(payload));
}

int se_gen_keypair(uint16_t fid, uint8_t key_type) {
    CHECK_FID(fid);

    // id[2]|type[1]
    uint8_t payload[3] = {0};
    uint8_t *p = payload;
    PUT_UINT16_BE(fid, p, 0);
    p += 2;
    *p++ = key_type;

    return se_execute_case2(CMD_ID_GEN_KEY_PAIR, payload, sizeof(payload));
}

int se_get_pubkey(uint16_t fid, uint8_t *pk, size_t *pk_len) {
    CHECK_FID(fid);
    uint8_t response[128] = {0};
    size_t response_size = sizeof(response);
    // id[2]
    uint8_t payload[2] = {0};
    uint8_t *p = payload;
    PUT_UINT16_BE(fid, p, 0);

    int ret = se_execute_case0(CMD_ID_GET_PUB_KEY, payload, sizeof(payload), response, &response_size);
    CHECK_CMD_RESULT(ret);
    RESP_INIT(response);
    if (response_get_length(resp) != 65) {
        return 1;
    }
    *pk_len = response_get_length(resp);
    memcpy(pk, resp->payload, *pk_len);
    return 0;
}

int se_cmac(uint16_t fid, const uint8_t *msg, size_t msg_len, uint8_t *cmac) {
    CHECK_FID(fid);
    if (msg_len > 1024) {
        return 1;
    }
    uint8_t response[64] = {0};
    size_t response_size = sizeof(response);

    // id[2]|mode[1]|len[2]|data
    uint8_t payload[1024+5] = {0};
    uint8_t *p = payload;
    PUT_UINT16_BE(fid, p, 0);
    p += 2;
    // mode
    *p++ = 0;
    // len
    PUT_UINT16_BE(msg_len, p, 0);
    // move over `len`
    p += 2;
    memcpy(p, msg, msg_len);
    int ret = se_execute_case0(CMD_ID_CMAC, payload, msg_len + 5, response, &response_size);
    CHECK_CMD_RESULT(ret);
    RESP_INIT(response);
    if (response_get_length(resp) != 16) {
        return 1;
    }
    memcpy(cmac, resp->payload, response_get_length(resp));
    return 0;
}

int se_hmac(uint16_t fid, const uint8_t *msg, size_t msg_len, uint8_t *hmac) {
    CHECK_FID(fid);
    if (msg_len > 1024) {
        return 1;
    }
    uint8_t response[64] = {0};
    size_t response_size = sizeof(response);

    // id[2]|mode[1]|len[2]|data
    uint8_t payload[1024+5] = {0};
    uint8_t *p = payload;
    PUT_UINT16_BE(fid, p, 0);
    // move over `fid`
    p += 2;
    // mode
    *p++ = 0;
    // len
    PUT_UINT16_BE(msg_len, p, 0);
    // move over `len`
    p += 2;
    memcpy(p, msg, msg_len);
    int ret = se_execute_case0(CMD_ID_HMAC, payload, msg_len + 5, response, &response_size);
    CHECK_CMD_RESULT(ret);
    RESP_INIT(response);
    // the sha256_hamc
    if (response_get_length(resp) != 32) {
        return 1;
    }
    memcpy(hmac, resp->payload, response_get_length(resp));
    return 0;
}

int se_ecdh(uint16_t fid, const uint8_t *pk, size_t pk_len, uint8_t *secret) {
    CHECK_FID(fid);
    if (pk_len > 65) {
        return 1;
    }
    uint8_t response[128] = {0};
    size_t response_size = sizeof(response);

    // id[2]|<uncompressed public key>
    uint8_t payload[65+2] = {0};
    uint8_t *p = payload;
    PUT_UINT16_BE(fid, p, 0);
    // move over `fid`
    p += 2;
    memcpy(p, pk, pk_len);
    int ret = se_execute_case0(CMD_ID_ECDH, payload, pk_len + 2, response, &response_size);
    CHECK_CMD_RESULT(ret);
    RESP_INIT(response);
    if (response_get_length(resp) != 32) {
        return 1;
    }
    memcpy(secret, resp->payload, response_get_length(resp));
    return 0;
}

int se_get_dev_pubkey(uint8_t pubkey[65]) {
    uint8_t response[128] = { 0 };
    size_t response_size = sizeof(response);

    int ret = se_execute_case3(CMD_ID_GET_DEV_PUBKEY, response, &response_size);
    CHECK_CMD_RESULT(ret);
    RESP_INIT(response);
    if (response_get_length(resp) != 65) {
        return 1;
    }

    memcpy(pubkey, resp->payload, response_get_length(resp));
    return 0;
}


int se_get_certificate_len(size_t *cert_len) {
    *cert_len = 0;
    uint8_t response[16] = {0};
    size_t response_size = sizeof(response);

    int ret = se_execute_case3(CMD_ID_GET_DEV_CERT_LENGTH, response, &response_size);
    CHECK_CMD_RESULT(ret);
    RESP_INIT(response);
    if (response_get_length(resp) != 2) {
        return 1;
    }
    *cert_len = GET_UINT16_BE(resp->payload, 0);
    return 0;
}

int se_read_certificate(uint8_t *cert, size_t *cert_len) {
    uint8_t response[1024] = {0};
    size_t response_size = sizeof(response);

    int ret = se_execute_case3(CMD_ID_GET_DEV_CERT, response, &response_size);
    CHECK_CMD_RESULT(ret);
    RESP_INIT(response);
    if (*cert_len < response_get_length(resp)) {
        return 1;
    }
    *cert_len = response_get_length(resp);
    memcpy(cert, resp->payload, *cert_len);
    return 0;
}

int se_sign_message(uint8_t *msg, size_t msg_len, uint8_t *signature) {
    if (msg_len > 1024) {
        return 1;
    }
    uint8_t response[128] = {0};
    size_t response_size = sizeof(response);

    int ret = se_execute_case0(CMD_ID_DEV_SIGN, msg, msg_len, response, &response_size);
    CHECK_CMD_RESULT(ret);
    RESP_INIT(response);
    if (response_get_length(resp) != 64) {
        return 1;
    }
    *signature = response_get_length(resp);
    memcpy(signature, resp->payload, response_get_length(resp));
    return 0;
}

int se_verify_app(void) {
    uint8_t response[16] = {0};
    size_t response_size = sizeof(response);

    int ret = se_execute_case3(CMD_ID_VERIFY_APP, response, &response_size);
    CHECK_CMD_RESULT(ret);
    RESP_INIT(response);
    // we use 0 as success
    return *resp->payload == 1? 0: 1;
}

int se_install_app(size_t index, const uint8_t* block, size_t block_size) {
    if ( block_size > 512) {
        return 1;
    }

    uint8_t payload[512+4] = {0};
    struct {
        uint8_t block_index_bytes[2];
        // 512 or <512 when is last block
        uint8_t block_size_bytes[2];
        uint8_t data[0];
    } *app_block = (void*)payload;
    PUT_UINT16_BE(index, app_block->block_index_bytes, 0);
    PUT_UINT16_BE(block_size, app_block->block_size_bytes, 0);
    memcpy(app_block->data, block, block_size);

    return se_execute_case2(CMD_ID_INSTALL_APP, payload, block_size + 4);
}

int se_ping(void) {
    return thd89_ping();
}

int se_has_pin(bool* exist) {
    uint8_t response[16] = {0};
    size_t response_size = sizeof(response);

    int ret = se_execute_case3(CMD_ID_HAS_PIN, response, &response_size);
    CHECK_CMD_RESULT(ret);
    RESP_INIT(response);
    if (response_get_length(resp) != 1) {
        return 1;
    }
    *exist = !!resp->payload[0];
    return 0;
}

int se_set_pin(const uint8_t *pin, size_t pin_len) {
    if (pin_len > PIN_MAX_LENGTH) {
        return 1;
    }

    return se_execute_case2(CMD_ID_SET_PIN, pin, pin_len);
}

int se_verify_pin(const uint8_t* pin, size_t pin_len) {
    if (pin_len > PIN_MAX_LENGTH) {
        return 1;
    }
    return se_execute_case2(CMD_ID_VERIFY_PIN, pin, pin_len);
}

int se_change_pin(const uint8_t *old_pin, size_t old_pin_len, const uint8_t *new_pin, size_t new_pin_len) {
    if (old_pin_len > PIN_MAX_LENGTH || old_pin_len == 0 || new_pin_len > PIN_MAX_LENGTH || new_pin_len == 0) {
        return 1;
    }

    uint8_t payload[256] = {0};
    uint8_t *p = payload;
    // old pin
    // move over `tag`
    *p++ = CMD_PARAM_OLD_PIN;
    PUT_UINT16_BE(old_pin_len, p, 0);
    // move over `len`
    p += 2;
    memcpy(p, old_pin, old_pin_len);
    // move over `data`
    p += old_pin_len;


    // new pin
    // move over `tag`
    *p++ = CMD_PARAM_NEW_PIN;
    PUT_UINT16_BE(new_pin_len, p, 0);
    // move over `len`
    p += 2;
    memcpy(p, new_pin, new_pin_len);
    p += new_pin_len;

    return se_execute_case2(CMD_ID_CHANGE_PIN, payload, p - payload);
}

int se_forget_pin(void) {
    return se_execute_case1(CMD_ID_FORGET_PIN);
}

int se_set_pin_user_max_retry(int max_retry) {
    if (max_retry > 0xff) {
        return 1;
    }

    uint8_t retry = max_retry & 0xff;
    return se_execute_case2(CMD_ID_SET_USER_PIN_MAX_RETRY, &retry, 1);
}
int se_get_pin_max_retry(int *max_retry) {
    uint8_t response[16] = {0};
    size_t response_size = sizeof(response);

    int ret = se_execute_case3(CMD_ID_GET_PIN_MAX_RETRY, response, &response_size);
    CHECK_CMD_RESULT(ret);
    RESP_INIT(response);
    if (response_get_length(resp) != 1) {
        return 1;
    }
    *max_retry = resp->payload[0];
    return 0;
}

int se_get_pin_retry(int *retry) {
    uint8_t response[16] = {0};
    size_t response_size = sizeof(response);

    int ret = se_execute_case3(CMD_ID_GET_PIN_RETRY, response, &response_size);
    CHECK_CMD_RESULT(ret);
    RESP_INIT(response);
    if (response_get_length(resp) != 1) {
        return 1;
    }
    *retry = resp->payload[0];
    return 0;
}

int se_reset_pin(void) {
    return se_execute_case1(CMD_ID_RESET_PIN);
}

bool se_check_app_binary(const uint8_t *binary, size_t binary_len) {
    if (binary_len < 512) return false;
    typedef struct {
        uint8_t magic[4];
        uint32_t version;
        // the `header_t`
        uint32_t header_size;
        // code size
        uint32_t code_size;
        uint8_t digest[32];
        uint8_t signature[64];
    }__attribute__((packed, aligned(512))) header_t ;
    header_t *header = (header_t*)binary;
    if (binary_len != header->header_size + header->code_size) {
        return false;
    }
    uint8_t digest[32] = {0};
    sha256_Raw(binary + header->header_size, header->code_size, digest);
    return memcmp(header->digest, digest, 32) == 0;
}
void se_binary_version(const uint8_t *binary, char version[17]) {
    struct {
        uint8_t build;
        uint8_t patch;
        uint8_t minor;
        uint8_t major;
    } *v = (void*)(binary+4);
    sprintf(version, "%d.%d.%d", v->major, v->minor, v->patch);
    return ;
}

void se_init(void) {
    se_spi_init();
    thd89_init();
    // reset thd89 connection
    thd89_reset();
}

void se_conn_reset(void) {
    thd89_init();
    // reset thd89 connection
    thd89_reset();
}

int se_handshake(const uint8_t *secret, size_t secret_len) {
    if (thd89_handshake(secret, secret_len) != THD89_SUCCESS) {
        return 1;
    }
    return 0;
}

int se_execute_case0(uint8_t cmd_id, const uint8_t *payload, size_t payload_size, uint8_t *response, size_t *response_size) {
    cmd_protect_type_t cpt = get_cmd_protect_type(cmd_id);
    // LOG_DEBUG(MODULE, "cmd: %X, type: %s", cmd_id, get_protect_type_str(cpt));
    uint8_t command[1536] = {0};
    if (payload_size > sizeof(command) - 3) {
        // payload size is too large
        return 1;
    }
    REQ_INIT_CMD(command, cmd_id);
    REQ_PAYLOAD(req, payload, payload_size);
    size_t req_size = command_size(req);
    thd89_result_t ret = THD89_SUCCESS;
    if (cpt == CMD_PROTECT_TYPE_PLAIN) {
        ret = thd89_execute(command, req_size, response, *response_size, response_size);
    } else if (cpt == CMD_PROTECT_TYPE_SECURE) {
        ret = thd89_secure_execute(command, req_size, response, *response_size, response_size);
    } else if (cpt == CMD_PROTECT_TYPE_MAYBE) {
        // for MAYBE type command
        // if secure channel is established, use secure execute, otherwise use plain execute
        if (thd89_is_secure_channel_established()) {
            ret = thd89_secure_execute(command, req_size, response, *response_size, response_size);
        } else {
            ret = thd89_execute(command, req_size, response, *response_size, response_size);
        }
    } else {
        LOG_ERROR(MODULE, "unknown protect type %d", cpt);
        return 1;
    }
    // transmit result
    if (ret != THD89_SUCCESS) {
        LOG_ERROR(MODULE, "execute command [%X] failed, ret = %d", cmd_id, ret);
        return 1;
    }

    RESP_INIT(response);
    if (resp->code != RESP_CODE_SUCCESS) {
        LOG_ERROR(MODULE, "execute command [%X] failed, code = %d", cmd_id, resp->code);
        return resp->code;
    }
    return 0;
}

int se_execute_case1(uint8_t cmd_id) {
    uint8_t response[128] = {0};
    size_t response_size = sizeof(response);
    return se_execute_case0(cmd_id, NULL, 0, response, &response_size);
}

int se_execute_case2(uint8_t cmd_id, const uint8_t *payload, size_t payload_size) {
    uint8_t response[128] = {0};
    size_t response_size = sizeof(response);
    return se_execute_case0(cmd_id, payload, payload_size, response, &response_size);
}

int se_execute_case3(uint8_t cmd_id, uint8_t *response, size_t *response_size) {
    return se_execute_case0(cmd_id, NULL, 0, response, response_size);
}
