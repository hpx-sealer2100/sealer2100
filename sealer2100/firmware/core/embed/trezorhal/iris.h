#ifndef __IRIS_H__
#define __IRIS_H__
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "keys.h"
#include "secbool.h"
#include "sha2.h"
#include "image.h"


typedef enum {
    IRIS_ERROR_OK = 0,
    IRIS_ERROR_TIMEOUT = -1,
    IRIS_ERROR_MEMORY_TOO_SMALL = -2,
    IRIS_ERROR_INVALID_CRC = -3,
    IRIS_ERROR_INVALID_MSG_ID = -5,
    IRIS_ERROR_INVALID_MSG_TYPE = -6,
    IRIS_ERROR_INVALID_MSG_LEN = -7,
    IRIS_ERROR_INVALID_MSG_DATA = -8,
    IRIS_ERROR_INVALID_RESPONSE = -9,
    IRIS_ERROR_SEC_FAILED = -10,
} iris_error_t;

typedef enum {
    IRIS_HANDSHAKE_PENDING = 1,
} iris_handshake_result_t;

typedef enum {
    IRIS_MSG_TYPE_REQUEST = 0x00,
    IRIS_MSG_TYPE_REPLY = 0x01,
    IRIS_MSG_TYPE_NOTE = 0x02,
} iris_msg_type_t;

typedef enum {
    IRIS_MSG_ID_NONE = 0x00, // use as a invalid msg id
    IRIS_MSG_ID_REGIST = 0x01,
    IRIS_MSG_ID_REGIST_FACE = 0x02,
    IRIS_MSG_ID_CANCEL_OPERATE = 0x03,
    IRIS_MSG_ID_MATCH = 0x04,
    IRIS_MSG_ID_USER_GET_COUNT = 0x05,
    IRIS_MSG_ID_USER_GET_ID_ALL = 0x06,
    IRIS_MSG_ID_USER_GET_BY_ID = 0x07,
    IRIS_MSG_ID_DEVICE_GET_STATUS = 0x08,
    IRIS_MSG_ID_USER_DEL_BY_ID = 0x09,
    IRIS_MSG_ID_USER_DEL_ALL = 0x0B,
    IRIS_MSG_ID_DEVICE_GET_SERIAL_CODE = 0x0C,
    IRIS_MSG_ID_DEVICE_GET_VERSION = 0x0D,
    IRIS_MSG_ID_USER_ADD = 0x0E,
    IRIS_MSG_ID_DEVICE_RESET = 0x14,
    IRIS_MSG_ID_DEVICE_REBOOT = 0x15,
    IRIS_MSG_ID_DEVICE_POWERDOWN = 0x16,
    IRIS_MSG_ID_DEVICE_REBOOT_LOADER = 0x1A,
    IRIS_MSG_ID_NOTE_MODULE_STATUS = 0x32,
    IRIS_MSG_ID_NOTE_REGIST_PROGRESS = 0x33,
    IRIS_MSG_ID_NOTE_IRIS_STATUS = 0x34,
    IRIS_MSG_ID_NOTE_VIDEO_STREAM = 0x35,
    IRIS_MSG_ID_NOTE_OTA_STATUS = 0x36,
    IRIS_MSG_ID_PACKET_OTA_START = 225,
    IRIS_MSG_ID_PACKET_OTA_STOP = 226,
    IRIS_MSG_ID_PACKET_OTA_HEADER = 227,
    IRIS_MSG_ID_PACKET_OTA_FIRMWARE = 228,
    IRIS_MSG_ID_PACKET_OTA_STATUS = 229,

    IRIS_MSG_ID_SET_SHARED_SECRET = 231,
    IRIS_MSG_ID_HANDSHAKE_HELLO = 232,
    IRIS_MSG_ID_HANDSHAKE_FINISHED = 233,


    // a mock msg id, used to wait sometime
    IRIS_MSG_ID_WAIT = 0x8000,
} iris_msg_id_t;

typedef enum {
    // 模组启动成功 (ready)
    IRIS_MODULE_STATUS_READY = 1,
    // 模组启动识别（match）
    IRIS_MODULE_STATUS_MATCH = 2,
    // 模组启动注册（enroll）
    IRIS_MODULE_STATUS_ENROLL = 3,
    // 模组已经绑定密钥
    IRIS_MODULE_STATUS_BOUND = 4,
} iris_module_status_t;

typedef enum {
    IRIS_OTA_RESULT_SUCCESS = 0,
    IRIS_OTA_RESULT_ERROR = 1,
    IRIS_OTA_RESULT_MD5_MISMATCH = 2,
}iris_ota_result_t;

// 0 空闲
// 1 升级过程中
// 2 升级完成
// 3 升级出错
typedef enum {
    IRIS_OTA_STATUS_IDLE = 0,
    IRIS_OTA_STATUS_PROCESSING = 1,
    IRIS_OTA_STATUS_DONE = 2,
    IRIS_OTA_STATUS_ERROR = 3,
}iris_ota_status_t;

/// 响应码
// 0 成功
// 1 失败，未知错误
// 2 模组拒绝该命令
// 3 超时失败
// 4 虹膜已注册
// 5 用户 id 已存在
// 6 没有录入用户，模组中没有用户
// 7 用户 id 不存在
// 8 模组中用户数量已达最大值
// 11 终止注册/识别过程
// 12 参数错误
// 13 相机打开失败
// 14 模组初始化失败
// 15 活体检测失败
typedef enum {
    IRIS_RESPONSE_CODE_OK = 0,
    IRIS_RESPONSE_CODE_FAIL = 1,
    IRIS_RESPONSE_CODE_REJECT = 2,
    IRIS_RESPONSE_CODE_TIMEOUT = 3,
    IRIS_RESPONSE_CODE_IRIS_REGISTED = 4,
    IRIS_RESPONSE_CODE_USER_ID_EXIST = 5,
    IRIS_RESPONSE_CODE_NO_USER = 6,
    IRIS_RESPONSE_CODE_USER_ID_NOT_EXIST = 7,
    IRIS_RESPONSE_CODE_USER_COUNT_MAX = 8,
    IRIS_RESPONSE_CODE_OPERATE_CANCEL = 11,
    IRIS_RESPONSE_CODE_INVALID_PARAM = 12,
    IRIS_RESPONSE_CODE_CAMERA_OPEN_FAIL = 13,
    IRIS_RESPONSE_CODE_INIT_FAIL = 14,
    IRIS_RESPONSE_CODE_LIVE_DETECT_FAIL = 15,
} iris_response_code_t;

typedef enum {
    IRIS_POLL_STATE_PENDDING = 0,
    IRIS_POLL_STATE_DONE = 1,
    IRIS_POLL_STATE_ERROR = 2,
    IRIS_POLL_STATE_TIMEOUT = 3,
} iris_poll_state_t;

typedef struct {
    uint32_t build;
    uint32_t patch;
    uint32_t minor;
    uint32_t major;
}iris_version_t;

// like `image_header_t` but `version` is `iris_version_t`
typedef struct {
    uint32_t magic;                // magic of the image, need be protected
    iris_version_t version;       // version of the image, need be protected
    uint32_t header_size;          // size of the header, need be protected
    uint32_t code_size;            // size of the code, need be protected
    uint32_t required_sig_count;   // required signature count, need be protected
    uint8_t _reserved[0x80 - 32];  // reserved, for future use

    // filed under this filed, no need to be protected, fill 0x00 when compute the digest
    uint8_t digest[32];
    uint32_t sig_mask;
    uint8_t sigs[KEY_N][IMAGE_SIG_SIZE];
    uint8_t pad[0x380 - 32 - 4 - 64 * KEY_N];
} iris_header_t;

_Static_assert(
    IMAGE_HEADER_SIZE == sizeof(iris_header_t), "sizeof(iris_header_t) must be 1024 bytes"
);

typedef struct {
    iris_header_t header;
    uint8_t data[0];
} signed_image_t;

typedef struct {
    iris_msg_id_t id;
    size_t payload_size;
    const uint8_t* payload;
} iris_request_t;

typedef struct {
    size_t fw_size;         // 固件大小, 4 bytes
    size_t fw_pkt_count;    // 固件包数量, 4 bytes
    size_t fw_pkt_size;     // 固件包大小, 2 bytes
    uint8_t fw_md5_sum[32]; // 固件 md5 校验和, 32 bytes hex string
} iris_packet_ota_header_t;

typedef struct {
    size_t pkt_id;       // 包 id, 4 bytes
    size_t pkt_size;     // 包大小, 2 bytes
    const uint8_t* data; // 包数据, `pkt_size` 字节
} iris_packet_ota_firmware_t;

typedef struct {
    union {
        uint8_t value;
        iris_module_status_t status;
        iris_ota_result_t ota_result;
    };
} iris_note_t;

typedef struct {
    iris_response_code_t code;
    size_t size;
    const uint8_t* data;
} iris_reply_t;

typedef struct {
    iris_ota_status_t status;
    uint32_t next_pkt_id; // 下一个包 id, 4 bytes
}iris_reply_ota_status_t;

// 响应消息,`iris_note_t`或者`iris_reply_t`
typedef struct {
    /// 响应消息ID
    iris_msg_id_t id;
    /// 响应消息类型
    iris_msg_type_t type;
    union {
        iris_note_t note;
        iris_reply_t reply;
    };
} iris_response_t;

typedef struct {
    uint32_t until;                   // 超时时间, 单位: 毫秒
    uint32_t operation;                // 操作类型, 使用 `iris_msg_id_t` 定义，表示正在进行什么操作, 方便用户记录操作状态, 由用户设置
    iris_poll_state_t state; // 响应轮询状态
    size_t resp_buf_len;              // 响应缓冲区长度
    uint8_t* resp_buf;                // 响应缓冲区
    size_t resp_raw_size;             // 原始响应消息大小, 单位: 字节
    iris_response_t resp;             // 响应消息, parsed from `resp_buf`
} iris_poll_context_t;

static const iris_version_t IRIS_SUPPORT_SEC_CHANNEL_VERSION = {0, 1004, 260421, 3};

int iris_version_parse(const char* s, iris_version_t* ver);
void iris_version_format(iris_version_t version, char str[33]);
// 0: a = b, +n: a > b, -n: a < b
int iris_version_compare(const iris_version_t* a, const iris_version_t* b);
#define IRIS_IS_SUPPORT_SEC_CHANNEL(ver) (iris_version_compare(&ver, &IRIS_SUPPORT_SEC_CHANNEL_VERSION) >= 0)

void iris_usart_init(void);
void iris_usart_deinit(void);
int iris_usart_send(const uint8_t* msg, size_t len);
int iris_usart_read(uint8_t* msg, size_t msg_buf_len);
iris_error_t iris_send_request(const iris_request_t* req);

// power up iris and initialize uart
void iris_open(void);
// power down iris and deinitialize uart
void iris_close(void);
// wait for iris power down done
void iris_wait_power_down(bool opened);
// open secure channel, return 0 if success, negative if failed, positive if pending
int iris_sec_channel_open(const uint8_t* secret, size_t secret_size);
// set shared secret
int iris_set_shared_secret(const uint8_t* secret, size_t secret_size);

iris_error_t iris_resp_parser(const uint8_t* msg, size_t msg_len, iris_response_t* resp);
iris_error_t iris_resp_ota_status_parser(const iris_response_t* resp, iris_reply_ota_status_t* ota_status);

/**
 * `iris_response_t` helper functions
 */

static inline bool iris_response_is_note(const iris_response_t* resp) {
    return resp->type == IRIS_MSG_TYPE_NOTE;
}
static inline bool iris_response_is_reply(const iris_response_t* resp) {
    return resp->type == IRIS_MSG_TYPE_REPLY;
}

static inline bool iris_response_replay_is_ok(const iris_response_t* resp) {
    return resp->type == IRIS_MSG_TYPE_REPLY && resp->reply.code == IRIS_RESPONSE_CODE_OK;
}

static inline bool iris_response_replay_is(const iris_response_t* resp, iris_msg_id_t id) {
    return resp->type == IRIS_MSG_TYPE_REPLY && resp->id == id;
}

/**
 * async function only send request, please poll response manually
 */
void iris_async_send_powerdown(void);
void iris_async_send_reboot(void);
iris_error_t iris_async_get_version(void);
iris_error_t iris_async_start_ota(void);
iris_error_t iris_async_stop_ota(void);
iris_error_t iris_async_send_ota_header(const iris_packet_ota_header_t* header);
iris_error_t iris_async_send_ota_packet(const iris_packet_ota_firmware_t* pkt);
iris_error_t iris_async_send_ota_status(void);

void iris_poll_context_init(
    iris_poll_context_t* ctx, uint32_t timeout, uint8_t* resp_buf, size_t resp_buf_len
);

void iris_poll_context_set_timeout(iris_poll_context_t* ctx, uint32_t timeout);
void iris_async_response_poll(iris_poll_context_t* ctx);

iris_poll_context_t iris_poll_wait(uint32_t timeout, uint8_t* resp_buf, size_t resp_buf_len);

static inline void iris_poll_context_reset_state(iris_poll_context_t* ctx) {
    ctx->state = IRIS_POLL_STATE_PENDDING;
}
static inline bool iris_poll_context_is_error(iris_poll_context_t* ctx) {
    return ctx->state == IRIS_POLL_STATE_ERROR || ctx->state == IRIS_POLL_STATE_TIMEOUT;
}

static inline const char* iris_poll_context_get_error_msg(iris_poll_context_t* ctx) {
    switch (ctx->state) {
    case IRIS_POLL_STATE_ERROR:
        return "iris response poll error";
    case IRIS_POLL_STATE_TIMEOUT:
        return "iris response poll timeout";
    default:
        return "unknown iris response poll error";
    }
}

void iris_print_header(const iris_header_t* const header);
void iris_header_hash_update(SHA256_CTX* ctx, const iris_header_t* const header);
secbool __wur iris_header_verify(const iris_header_t* const header);

#endif
