#include "alignment.h"
#include "bootui.h"
#include "device.h"
#include "iris.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "boot_context.h"
#include "common.h"
#include "fs.h"
#include "fsm.h"
#include "log.h"
#include "md5.h"
#include "secure_heap.h"
#include "updater.h"

#define MODULE "iris updater"

// packet size for iris updater
#define PACKET_SIZE (2 * 1024)

typedef enum {
    // the state of iris install begin
    IRIS_INSTALLING_STATE_NONE,
    // the state of iris power up and waiting iris note power up
    IRIS_INSTALLING_STATE_OPENING,
    // the state of iris opened
    IRIS_INSTALLING_STATE_OPEN_DONE,
    // the state of iris ota starting
    IRIS_INSTALLING_STATE_OTA_STARTING,
    // the state of iris ota start
    IRIS_INSTALLING_STATE_OTA_START_DONE,
    // the state of iris ota header sending
    IRIS_INSTALLING_STATE_OTA_HEADER_SENDING,
    // the state of iris ota header send
    IRIS_INSTALLING_STATE_OTA_HEADER_SEND_DONE,
    // the state of iris ota send packt, packet need send in loop, this indicate send packet continue
    IRIS_INSTALLING_STATE_OTA_PACKET_SEND,
    // the state of iris ota packet sending
    IRIS_INSTALLING_STATE_OTA_PACKET_SENDING,
    // the state of iris ota packet send
    IRIS_INSTALLING_STATE_OTA_PACKET_SEND_DONE,
    // the state of iris ota status sending, for sync packet id, when packet sending time out
    IRIS_INSTALLING_STATE_OTA_STATUS_SENDING,
    // the state of iris ota status send
    IRIS_INSTALLING_STATE_OTA_STATUS_SEND_DONE,
    // the state of iris upgrading
    IRIS_INSTALLING_STATE_OTA_UPGRADING,

    // the state of iris ota done
    IRIS_INSTALLING_STATE_OTA_DONE,
} iris_installing_sate_t;

typedef struct {
    iris_header_t* header;     // cached image header, read from file

    // for tracking packet id
    size_t pkt_total; // total packet count
    size_t pkt_id;    // current packet id

    uint32_t time;

    // for verifing firmware md5 sum, IRIS module use
    md5_context md5_ctx;
    uint8_t fw_md5_sum[32]; // firmware md5 sum

    // for verifing firmware signature, our use
    SHA256_CTX sha256;

    // for iris async response
    uint8_t resp_buf[64];
    iris_poll_context_t resp_poll_ctx;

    // for iris install state
    iris_installing_sate_t installing_state;
} iris_updater_context_t;

static void bytes_to_hex_str(const uint8_t* bytes, char* hex, size_t len) {
#define HEX_CHAR_TABLE "0123456789ABCDEF"
    while (len--) {
        uint8_t b = *bytes++;
        *hex++ = HEX_CHAR_TABLE[b >> 4];
        *hex++ = HEX_CHAR_TABLE[b & 0x0F];
    }
}

#define failed(code, msg) updater_failed(code, msg)

static void verify_async(void) {
    updating_context_t* ctx = boot_ctx.updater.updating;
    iris_updater_context_t* ictx = ctx->user_data;
    uint8_t chunk[4096];

    // sum md5 sum
    if (fs_file_tell(ctx->fp) == 0) {
        // setup md5 context
        LOG_DEBUG(MODULE, "setup md5 hasher for firmware verify");
        md5_init(&ictx->md5_ctx);
        md5_starts(&ictx->md5_ctx);

        LOG_DEBUG(MODULE, "setup sha256 hasher for firmware signature verify");
        sha256_Init(&ictx->sha256);
        iris_header_hash_update(&ictx->sha256, ictx->header);
        // seek to code offset
        fs_file_seek(ctx->fp, ictx->header->header_size, FS_SEEK_SET);
        return;
    }

    int r = fs_file_read(ctx->fp, chunk, sizeof(chunk));
    if (r < 0) {
        failed(UPDATER_ERROR_READ_FAILED, "Read firmware binary failed");
        return;
    }
    md5_update(&ictx->md5_ctx, chunk, r);
    sha256_Update(&ictx->sha256, chunk, r);

    if (fs_file_tell(ctx->fp) != fs_file_size(ctx->fp)) {
        return;
    }

    {
        // finalize md5 sum
        uint8_t digest[16];
        md5_finish(&ictx->md5_ctx, digest);
        // convert to hex string
        bytes_to_hex_str(digest, (char*)ictx->fw_md5_sum, sizeof(digest));
        LOG_HEXDUMP_DEBUG(MODULE, "iris firmware md5 sum", digest, sizeof(digest));
    }

    {
        uint8_t digest[32];
        sha256_Final(&ictx->sha256, digest);
        LOG_HEXDUMP_DEBUG(MODULE, "", digest, sizeof(digest));

        iris_print_header(ictx->header);

        // verify digest
        if (memcmp(digest, ictx->header->digest, sizeof(digest)) != 0) {
            LOG_DEBUG(MODULE, "iris firmware digest mismatch");
            LOG_HEXDUMP_DEBUG(MODULE, "digest in header", ictx->header->digest, sizeof(ictx->header->digest));
            failed(UPDATER_ERROR_IMAGE_VERIFY_FAILED, "Verify firmware binary failed");
            return;
        }
        LOG_DEBUG(MODULE, "iris firmware digest verify success");

        // verify signature
        if (iris_header_verify(ictx->header) != sectrue) {
            LOG_DEBUG(MODULE, "iris firmware signature verify failed");
            failed(UPDATER_ERROR_IMAGE_VERIFY_FAILED, "Verify firmware binary failed");
            return;
        }
        LOG_DEBUG(MODULE, "iris firmware signature verify success");
    }

    // seek to start
    fs_file_seek(ctx->fp, 0, FS_SEEK_SET);
    // prepare install state
    ictx->installing_state = IRIS_INSTALLING_STATE_NONE;
    updater_success(UPDATER_STATE_INSTALL);
}

static void install_async(void) {
    updating_context_t* ctx = boot_ctx.updater.updating;
    iris_updater_context_t* ictx = ctx->user_data;

    // 由于状态比较多，不适用 `iris_response_context_t`.operation 记录当前状态
    // 而是使用 `ictx->installing_state` 记录

    if (ictx->installing_state == IRIS_INSTALLING_STATE_NONE) {
        // reset boot cached iris version
        memset(boot_ctx.iris.version, 0, sizeof(boot_ctx.iris.version));

        // step 0.0. do open iris
        LOG_DEBUG(MODULE, "opening iris");
        // open iris
        iris_open();
        ictx->installing_state = IRIS_INSTALLING_STATE_OPENING;
        // wait for iris note power up
        // prepare response poll context
        iris_poll_context_init(&ictx->resp_poll_ctx, 2000, ictx->resp_buf, sizeof(ictx->resp_buf));
        // iris open need a moment to power up, about 700ms
        // yield cpu
        return;
    }

    // wait 1s after open done
    if (ictx->installing_state == IRIS_INSTALLING_STATE_OPEN_DONE) {
        if (hal_ticks_ms() - ictx->time < 1000) {
            // yield cpu
            return;
        }
    }

    // poll iris response
    iris_async_response_poll(&ictx->resp_poll_ctx);

    // check iris response error
    if (ictx->resp_poll_ctx.state == IRIS_POLL_STATE_ERROR) {
        LOG_ERROR(MODULE, "iris response error");
        failed(UPDATER_ERROR_IMAGE_INSTALL_FAILED, "Iris response error");
        return;
    }

    // check timeout
    if (ictx->resp_poll_ctx.state == IRIS_POLL_STATE_TIMEOUT) {
        // when packet sending timeout, send status packet to sync packet id
        if (ictx->installing_state == IRIS_INSTALLING_STATE_OTA_PACKET_SENDING) {
            // send status packet
            iris_async_send_ota_status();
            ictx->installing_state = IRIS_INSTALLING_STATE_OTA_STATUS_SENDING;
            // reset poll context timeout
            iris_poll_context_set_timeout(&ictx->resp_poll_ctx, 1000);
            // reset poll context state
            iris_poll_context_reset_state(&ictx->resp_poll_ctx);
            // yield cpu
            return;
        }
        LOG_DEBUG(MODULE, "iris response timeout");
        failed(UPDATER_ERROR_IMAGE_INSTALL_FAILED, "Iris response timeout");
        return;
    }

    // check pending
    if (ictx->resp_poll_ctx.state == IRIS_POLL_STATE_PENDDING) {
        // iris not response, pending, wait for next poll
        return;
    }

    // now response is done

    if (ictx->installing_state == IRIS_INSTALLING_STATE_OPENING) {
        // step 0.1 check iris open done
        if (!iris_response_is_note(&ictx->resp_poll_ctx.resp)) {
            LOG_ERROR(MODULE, "iris open failed, response is not note");
            failed(UPDATER_ERROR_IMAGE_INSTALL_FAILED, "Iris open failed");
            return;
        }
        /*
        if (ictx->resp_poll_ctx.resp.note.status == IRIS_MODULE_STATUS_BOUND) {
            uint8_t key[32] = {0};
            if (!device_get_iris_pre_shared_key(key)) {
                // iris have set secret, but device not have key??
                LOG_ERROR(MODULE, "iris open failed, get pre shared key failed");
                failed(UPDATER_ERROR_IMAGE_INSTALL_FAILED, "Iris open failed");
                return;
            }
            int ret = IRIS_ERROR_OK;
            do {
                hal_delay(10);
                ret = iris_sec_channel_open(key, 32);
            } while (ret == IRIS_HANDSHAKE_PENDING);
            if (ret != IRIS_ERROR_OK) {
                LOG_ERROR(MODULE, "iris open failed, sec channel open failed");
                failed(UPDATER_ERROR_IMAGE_INSTALL_FAILED, "Iris open failed");
                return;
            }
            // sec channel open done
            LOG_DEBUG(MODULE, "iris sec channel open done");
        }
        else if (ictx->resp_poll_ctx.resp.note.status != IRIS_MODULE_STATUS_READY) {
            LOG_ERROR(MODULE, "iris open failed, status is not ready");
            failed(UPDATER_ERROR_IMAGE_INSTALL_FAILED, "Iris open failed");
            return;
        }*/

        if (ictx->resp_poll_ctx.resp.note.status != IRIS_MODULE_STATUS_BOUND &&
            ictx->resp_poll_ctx.resp.note.status != IRIS_MODULE_STATUS_READY) {
            failed(UPDATER_ERROR_IMAGE_INSTALL_FAILED, "Iris open failed");
            return;
        }

        LOG_DEBUG(MODULE, "iris power up done");
        ictx->time = hal_ticks_ms();
        ictx->installing_state = IRIS_INSTALLING_STATE_OPEN_DONE;
        // not reset poll context, not reset poll timeout
        // yield cpu, wait for next poll
        return;
    }

    if (ictx->installing_state == IRIS_INSTALLING_STATE_OPEN_DONE) {
        // step 1.0. open done, do start iris ota
        LOG_DEBUG(MODULE, "starting iris ota");
        ictx->installing_state = IRIS_INSTALLING_STATE_OTA_STARTING;
        iris_async_start_ota();

        // wait for iris note ota start response
        iris_poll_context_set_timeout(&ictx->resp_poll_ctx, 1000);
        iris_poll_context_reset_state(&ictx->resp_poll_ctx);
        // yield cpu
        return;
    }

    if (ictx->installing_state == IRIS_INSTALLING_STATE_OTA_STARTING) {
        // step 1.1 check response is ota start done
        if (!iris_response_replay_is(&ictx->resp_poll_ctx.resp, IRIS_MSG_ID_PACKET_OTA_START) ||
            !iris_response_replay_is_ok(&ictx->resp_poll_ctx.resp)) {
            LOG_ERROR(MODULE, "iris ota start failed");
            failed(UPDATER_ERROR_IMAGE_INSTALL_FAILED, "Iris ota start failed");
            return;
        }

        LOG_DEBUG(MODULE, "iris ota start done");
        ictx->installing_state = IRIS_INSTALLING_STATE_OTA_START_DONE;
        // no need yield cpu
    }

    // iris ota start done
    if (ictx->installing_state == IRIS_INSTALLING_STATE_OTA_START_DONE) {
        // step 2.0 send iris ota header
        LOG_DEBUG(MODULE, "sending iris ota header");
        // prepare packet id
        // the `code_size` in header is the total size of firmware, include header
        ictx->pkt_total = (ictx->header->code_size - ictx->header->header_size + PACKET_SIZE - 1) / PACKET_SIZE;
        ictx->pkt_id = 0;

        iris_packet_ota_header_t header = {
            .fw_size = ictx->header->code_size - ictx->header->header_size,
            .fw_pkt_size = PACKET_SIZE,
            .fw_pkt_count = ictx->pkt_total,
        };
        memcpy(header.fw_md5_sum, ictx->fw_md5_sum, sizeof(header.fw_md5_sum));
        iris_async_send_ota_header(&header);
        ictx->installing_state = IRIS_INSTALLING_STATE_OTA_HEADER_SENDING;
        // wait for iris send ota header response
        iris_poll_context_set_timeout(&ictx->resp_poll_ctx, 1000);
        iris_poll_context_reset_state(&ictx->resp_poll_ctx);
        // yield cpu
        return;
    }

    if (ictx->installing_state == IRIS_INSTALLING_STATE_OTA_HEADER_SENDING) {
        // step 2.1 check response is ota header send done

        if (!iris_response_replay_is(&ictx->resp_poll_ctx.resp, IRIS_MSG_ID_PACKET_OTA_HEADER) ||
            !iris_response_replay_is_ok(&ictx->resp_poll_ctx.resp)) {
            LOG_ERROR(MODULE, "iris ota header send failed");
            failed(UPDATER_ERROR_IMAGE_INSTALL_FAILED, "Iris ota header send failed");
            return;
        }

        LOG_DEBUG(MODULE, "iris ota header send done");
        ictx->installing_state = IRIS_INSTALLING_STATE_OTA_HEADER_SEND_DONE;
        // no need yield cpu
    }

    if (ictx->installing_state == IRIS_INSTALLING_STATE_OTA_HEADER_SEND_DONE) {
        // just change to send
        ictx->installing_state = IRIS_INSTALLING_STATE_OTA_PACKET_SEND;

        // seek to code start
        fs_file_seek(ctx->fp, ictx->header->header_size, FS_SEEK_SET);
    }

    if (ictx->installing_state == IRIS_INSTALLING_STATE_OTA_PACKET_SEND) {
        // step 3.0. send ota packet
        LOG_DEBUG(MODULE, "sending iris ota packet: %d", ictx->pkt_id);
        uint8_t data[PACKET_SIZE] = {0};
        int r = fs_file_read(ctx->fp, data, PACKET_SIZE);
        if (r < 0) {
            LOG_ERROR(MODULE, "iris ota packet read failed");
        }
        // send ota packet
        iris_packet_ota_firmware_t packet = {
            .pkt_id = ictx->pkt_id,
            .pkt_size = r,
            .data = data,
        };
        iris_async_send_ota_packet(&packet);
        ictx->installing_state = IRIS_INSTALLING_STATE_OTA_PACKET_SENDING;
        // wait for iris send ota packet response
        // 2K bytes, 115200 baud, 11K/s, 1 second time is enough
        iris_poll_context_set_timeout(&ictx->resp_poll_ctx, 1000);
        iris_poll_context_reset_state(&ictx->resp_poll_ctx);
        // yield cpu
        return;
    }

    if (ictx->installing_state == IRIS_INSTALLING_STATE_OTA_PACKET_SENDING) {
        // step 3.1 check response is ota packet send done
        if (!iris_response_replay_is(&ictx->resp_poll_ctx.resp, IRIS_MSG_ID_PACKET_OTA_FIRMWARE) ||
            !iris_response_replay_is_ok(&ictx->resp_poll_ctx.resp)) {
            LOG_ERROR(MODULE, "iris ota packet send failed");
            failed(UPDATER_ERROR_IMAGE_INSTALL_FAILED, "Iris ota packet send failed");
            return;
        }
        uint32_t pkt_id = GET_UINT32_LE(ictx->resp_poll_ctx.resp.reply.data, 0);
        // check pkt id
        if (pkt_id != ictx->pkt_id) {
            LOG_ERROR(MODULE, "iris ota packet send failed, pkt_id not match");
            failed(UPDATER_ERROR_IMAGE_INSTALL_FAILED, "Iris ota packet send failed, pkt_id not match");
            return;
        }
        LOG_DEBUG(MODULE, "iris ota packet send done: %d", pkt_id);

        ictx->pkt_id++;

        if (ictx->pkt_id < ictx->pkt_total) {
            // send, next packet
            ictx->installing_state = IRIS_INSTALLING_STATE_OTA_PACKET_SEND;
            // yield cpu
            return;
        } else {
            // send done
            ictx->installing_state = IRIS_INSTALLING_STATE_OTA_PACKET_SEND_DONE;
        }
    }

    if (ictx->installing_state == IRIS_INSTALLING_STATE_OTA_STATUS_SENDING) {
        // step 3.2 check response is ota status send done
        if (!iris_response_replay_is(&ictx->resp_poll_ctx.resp, IRIS_MSG_ID_PACKET_OTA_STATUS) ||
            !iris_response_replay_is_ok(&ictx->resp_poll_ctx.resp)) {
            LOG_ERROR(MODULE, "iris ota status send failed");
            failed(UPDATER_ERROR_IMAGE_INSTALL_FAILED, "Iris ota status send failed");
            return;
        }
        LOG_DEBUG(MODULE, "iris ota status send done");
        iris_reply_ota_status_t ota_status = {0};
        if (iris_resp_ota_status_parser(&ictx->resp_poll_ctx.resp, &ota_status) != IRIS_ERROR_OK) {
            LOG_ERROR(MODULE, "iris ota status parse failed");
            failed(UPDATER_ERROR_IMAGE_INSTALL_FAILED, "Iris ota status parse failed");
            return;
        }
        if (ota_status.status == IRIS_OTA_STATUS_DONE) {
            // ota have done
            ictx->installing_state = IRIS_INSTALLING_STATE_OTA_DONE;
            return;
        } else if (ota_status.status != IRIS_OTA_STATUS_PROCESSING) {
            // IDLE or ERROR, not we expect
            LOG_ERROR(MODULE, "iris ota status not processing");
            failed(UPDATER_ERROR_IMAGE_INSTALL_FAILED, "Iris ota status not processing");
            return;
        }
        // ota_status.processing, continue send status
        LOG_DEBUG(MODULE, "ota status next pkt id: %d", ota_status.next_pkt_id);
        LOG_DEBUG(MODULE, "ctx current pkt id: %d", ictx->pkt_id);
        // the next pkt id is ictx current pkt id + 1, for respone timeout
        // or is ictx current pkt id, for try send again
        if (ota_status.next_pkt_id == ictx->pkt_id) {
            // the ictx->pkt_id have read out from ctx->fp
            // seek of `fp` is (ictx->pkt_id +1) * PACKET_SIZE
            // request the ictx->pkt_id packet again,
            // we need seek back to (ictx->pkt_id) * PACKET_SIZE
            // `code_size` is the total size of file, include header
            // the iris module only need ota binary, ota binary only start from header size
            fs_file_seek(ctx->fp, ictx->pkt_id * PACKET_SIZE + ictx->header->header_size, FS_SEEK_SET);
        } else if (ota_status.next_pkt_id == ictx->pkt_id + 1) {
            // the ictx->pkt_id have read out from ctx->fp
            // iris have recevie complete, but not response
            // request the ictx->pkt_id + 1 packet
            // we not need seek
            ictx->pkt_id++;
        } else {
            LOG_ERROR(MODULE, "iris ota status next pkt id not match");
            failed(UPDATER_ERROR_IMAGE_INSTALL_FAILED, "Iris ota status next pkt id not match");
            return;
        }
        // send again
        ictx->installing_state = IRIS_INSTALLING_STATE_OTA_PACKET_SEND;
        // yield cpu
        return;
    }

    if (ictx->installing_state == IRIS_INSTALLING_STATE_OTA_PACKET_SEND_DONE) {
        // wait note ota result
        ictx->installing_state = IRIS_INSTALLING_STATE_OTA_UPGRADING;
        iris_poll_context_reset_state(&ictx->resp_poll_ctx);
        iris_poll_context_set_timeout(&ictx->resp_poll_ctx, 1000 * 60 * 5);
        return;
    }

    if (ictx->installing_state == IRIS_INSTALLING_STATE_OTA_UPGRADING) {
        // 3.2 check ota result
        if (!iris_response_is_note(&ictx->resp_poll_ctx.resp) ||
            ictx->resp_poll_ctx.resp.id != IRIS_MSG_ID_NOTE_OTA_STATUS) {
            LOG_ERROR(MODULE, "iris upgrade failed");
            failed(UPDATER_ERROR_IMAGE_INSTALL_FAILED, "Iris upgrade failed");
            return;
        }
        if (ictx->resp_poll_ctx.resp.note.ota_result != IRIS_OTA_RESULT_SUCCESS) {
            LOG_ERROR(MODULE, "iris upgrade failed: %d", ictx->resp_poll_ctx.resp.note.ota_result);
            failed(UPDATER_ERROR_IMAGE_INSTALL_FAILED, "Iris upgrade failed");
            return;
        }
        LOG_DEBUG(MODULE, "iris upgrade done");
        ictx->installing_state = IRIS_INSTALLING_STATE_OTA_DONE;
    }

    // not done, yield cpu, the dog
    if (ictx->installing_state != IRIS_INSTALLING_STATE_OTA_DONE) {
        return;
    }
    LOG_DEBUG(MODULE, "Iris update success");
    fsm_sendSuccess("Iris update success");
    updater_success(UPDATER_STATE_FINISHED);
}

static void __abort(void) {
    updating_context_t* ctx = boot_ctx.updater.updating;
    iris_updater_context_t* ictx = ctx->user_data;
    if (!ictx) {
        return;
    }

    if (ictx->installing_state > IRIS_INSTALLING_STATE_OPENING &&
        ictx->installing_state < IRIS_INSTALLING_STATE_OTA_DONE) {
        // have start ota, but not done
        LOG_DEBUG(MODULE, "aborting iris ota");
        iris_async_stop_ota();
        hal_delay(100);
    }

    // close iris
    if (ictx->installing_state != IRIS_INSTALLING_STATE_NONE) {
        iris_wait_power_down(true);
        // wait a moment for iris power down
        hal_delay(5);
        iris_close();
    }

    if (ictx->header) {
        vPortFree(ictx->header);
        ictx->header = NULL;
    }

    vPortFree(ictx);
    ctx->user_data = NULL;

    ui_event_broadcast(UI_EVENT_IRIS_INFO_REFRESH, NULL);
}

static updater_t updater = {
    .get_name_fn = updater_get_name,
    .get_cur_ver_fn = updater_get_cur_ver,
    .get_new_ver_fn = updater_get_new_ver,
    .get_error_fn = updater_get_error,
    .get_processed_fn = updater_get_processed,
    .verify_image_fn = verify_async,
    .install_image_fn = install_async,
    .abort_fn = __abort,
    .cancelled_fn = updater_canceled,
};
void update_iris(void) {
    updating_context_t* ctx = boot_ctx.updater.updating;

    // step 0. setup current version
    strcpy(ctx->cur_ver, boot_ctx.iris.version);

    // step 1. setup user data
    iris_updater_context_t* ictx = pvPortMalloc(sizeof(iris_updater_context_t));
    memset(ictx, 0, sizeof(iris_updater_context_t));
    ictx->header = pvPortMalloc(sizeof(iris_header_t));
    memset(ictx->header, 0, sizeof(iris_header_t));
    ctx->user_data = ictx;

    // step 2. load header
    fs_file_seek(ctx->fp, 0, FS_SEEK_SET);
    int r = fs_file_read(ctx->fp, ictx->header, sizeof(iris_header_t));
    if (r < 0) {
        LOG_DEBUG(MODULE, "Read iris header file failed");
        failed(UPDATER_ERROR_IMAGE_VERIFY_FAILED, "Read iris header file failed");
        // have not register updater, free manually
        vPortFree(ictx->header);
        vPortFree(ictx);
        ctx->user_data = NULL;
        return;
    }

    if (ictx->header->magic != IRIS_IMAGE_MAGIC) {
        LOG_DEBUG(MODULE, "Invalid iris magic number");
        failed(UPDATER_ERROR_IMAGE_VERIFY_FAILED, "Invalid iris binary file");
        // have not register updater, free manually
        vPortFree(ictx->header);
        vPortFree(ictx);
        ctx->user_data = NULL;
        return;
    }
    if (ictx->header->code_size != fs_file_size(ctx->fp)) {
        LOG_DEBUG(MODULE, "Invalid iris code size");
        failed(UPDATER_ERROR_IMAGE_VERIFY_FAILED, "Invalid iris binary file");
        // have not register updater, free manually
        vPortFree(ictx->header);
        vPortFree(ictx);
        ctx->user_data = NULL;
        return;
    }

    // seek to code offset
    fs_file_seek(ctx->fp, 0, FS_SEEK_SET);
    // set up new version
    iris_version_format(ictx->header->version, ctx->new_ver);


    // step 3. register updater
    updater_register(&updater);
}
