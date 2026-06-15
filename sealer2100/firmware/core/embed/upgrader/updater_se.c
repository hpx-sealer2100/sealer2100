#include "updater.h"

#include <string.h>

#include "common.h"
#include "boot_context.h"
#include "fs.h"
#include "log.h"
#include "se.h"
#include "secure_heap.h"
#include "sha2.h"
#include "device.h"
#include "memzero.h"

#define MODULE "SE updater"

#define HEADER_SIZE 512
#define CHUNK_SIZE 512

typedef struct {
  uint8_t header[HEADER_SIZE];
  SHA256_CTX sha256;

  int index;
}se_updater_context_t;

typedef struct {
  uint8_t magic[4];
  uint32_t version;
  // the `header_t`
  uint32_t header_size;
  // code size
  uint32_t code_size;
  uint8_t digest[32];
  uint8_t signature[64];
}__attribute__((packed, aligned(HEADER_SIZE))) header_t ;

static inline void failed(updater_error_t ucode, const char* msg) {
  updater_failed(ucode);
  LOG_ERROR(MODULE, "%s", msg);
}

// verify image
static void verify_async(void) {
  // not use `se_check_app_binary` block function
  updating_context_t *ctx = boot_ctx.updater.updating;
  uint8_t chunk[CHUNK_SIZE] = {0};

  int r = fs_file_read(ctx->fp, chunk, sizeof(chunk));

  if (r < 0) {
    failed(UPDATER_ERROR_READ_FAILED, "Read SE binary failed");
    return;
  }

  se_updater_context_t *se_ctx = ctx->user_data;
  uint32_t processed = fs_file_tell(ctx->fp);
  // we have read the header
  if (processed == sizeof(chunk)) {
    // cache the header
    memcpy(se_ctx->header, chunk, sizeof(chunk));
    header_t *header = (header_t*)se_ctx->header;
    if (fs_file_size(ctx->fp) != header->header_size + header->code_size) {
      failed(UPDATER_ERROR_IMAGE_VERIFY_FAILED, "Invalied SE binary file");
      return;
    }
    sha256_Init(&se_ctx->sha256);
  } else {
    sha256_Update(&se_ctx->sha256, chunk, r);
  }

  if (fs_file_tell(ctx->fp) != fs_file_size(ctx->fp)) {
    return;
  }

  header_t *header = (header_t*)se_ctx->header;
  uint8_t digest[32] = {0};
  sha256_Final(&se_ctx->sha256, digest);
  if (memcmp(header->digest, digest, 32) != 0 ) {
    failed(UPDATER_ERROR_IMAGE_VERIFY_FAILED, "Verify SE binary file failed");
    return;
  }

  // seek to start
  fs_file_seek(ctx->fp, 0, FS_SEEK_SET);
  updater_success(UPDATER_STATE_INSTALL);
  LOG_DEBUG(MODULE, "SE binary file verify successed");
}

// install image
static void install_async(void) {
  updating_context_t *ctx = boot_ctx.updater.updating;
  int r = 0;

  // when at begin of file, is just begin upating, we need switch to bootloader
  if (fs_file_tell(ctx->fp) == 0) {
    if (se_is_running_app()) {
        r = se_launch(STATE_BOOTLOADER);
        if (r != 0) {
          failed(UPDATER_ERROR_IMAGE_INSTALL_FAILED, "SE reboot to bootloader failed");
          return;
        }

        // wait SE reboot to bootloader
        hal_delay(30);
        se_conn_reset();
        uint8_t secret[32] = {0};
        if (device_get_pre_shared_key(secret)) {
          se_handshake(secret, sizeof(secret));
        }
        memzero(secret, sizeof(secret));
    }
    if (!se_is_running_bootloader()) {
        failed(UPDATER_ERROR_IMAGE_INSTALL_FAILED, "SE reboot to bootloader failed");
        return;
    }
  }


  uint8_t chunk[CHUNK_SIZE] = {0};

  r = fs_file_read(ctx->fp, chunk, sizeof(chunk));
  if (r < 0) {
    failed(UPDATER_ERROR_IMAGE_INSTALL_FAILED, "Read SE binary file failed");
    return;
  }

  se_updater_context_t *se_ctx = ctx->user_data;
  r = se_install_app(se_ctx->index++, chunk, r);
  if (r != 0) {
    failed(UPDATER_ERROR_IMAGE_INSTALL_FAILED, "Install SE binary file failed");
    return;
  }

  LOG_DEBUG(MODULE, "SE install %d block success", se_ctx->index);
  // give SE time to install
  hal_delay(20);
  // not install complete
  if (fs_file_tell(ctx->fp) != fs_file_size(ctx->fp)) {
    return;
  }
  se_reboot();
  hal_delay(100);
  se_conn_reset();
  {
    uint8_t secret[32] = {0};
    if (device_get_pre_shared_key(secret)) {
      se_handshake(secret, sizeof(secret));
    }
    memzero(secret, sizeof(secret));
  }
  if (!se_is_running_app()) {
    failed(UPDATER_ERROR_IMAGE_INSTALL_FAILED, "SE invalid state");
    return;
  }
  LOG_DEBUG(MODULE, "SE install successed");
  boot_ctx.upgrader_op.state = UPGRADER_OP_STATE_SE_INSTALL_DONE;
  boot_ctx.upgrader_op.active_time = hal_ticks_ms();
  updater_success(UPDATER_STATE_FINISHED);
}

static void __abort(void) {
  updating_context_t *ctx = boot_ctx.updater.updating;
  se_updater_context_t *se_ctx = ctx->user_data;
  if (se_ctx) {
    vPortFree(ctx);
  }
  ctx->user_data = NULL;
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

void update_se(void) {
  // step 0. check se state
  // maybe SE is running in boot mode
  // if (!se_is_running_app()) {
  //   failed(UPDATER_ERROR_INVALID_STATE, "SE invalied state");
  //   return;
  // }

  updating_context_t *ctx = boot_ctx.updater.updating;

  // step 1. setup current version
  // current version
  if (se_is_running_app()) {
    int r = se_get_version(ctx->cur_ver);
    if (r != 0) {
      LOG_DEBUG(MODULE, "get current version failed");
      failed(UPDATER_ERROR_IMAGE_INSTALL_FAILED, "Get SE version failed");
      return;
    }
  } else {
    // SE running in `boot` mode
    strcpy(ctx->cur_ver, "None");
  }

  // step 2. setup new version
  if (fs_file_size(ctx->fp) < 512) {
    LOG_DEBUG(MODULE, "bianry file too small");
    failed(UPDATER_ERROR_IMAGE_VERIFY_FAILED, "SE binary file invalied");
    return;
  }
  uint8_t header[HEADER_SIZE] = {0};
  int r = fs_file_read(ctx->fp, header, sizeof(header));
  if (r < 0) {
    LOG_DEBUG(MODULE, "read binary failed");
    failed(UPDATER_ERROR_READ_FAILED, "Read SE binary file failed");
    return;
  }

  // seek to start
  fs_file_seek(ctx->fp, 0, FS_SEEK_SET);
  se_binary_version(header, ctx->new_ver);

  // step 3. alloc se updater context
  ctx->user_data = pvPortMalloc(sizeof(se_updater_context_t));
  memset(ctx->user_data, 0, sizeof(se_updater_context_t));

  // step 4. register updater
  updater_register(&updater);
}
