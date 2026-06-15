#include "boot_context.h"

#include <stdint.h>
#include <stdio.h>

#include "common.h"
#include "fs.h"
#include "flash.h"
#include "fsm.h"
#include "image.h"
#include "log.h"
#include "mpu.h"
#include "secure_heap.h"
#include "updater.h"
#include "sha2.h"

#define MODULE "firmware updater"

#define CHUNK_SIZE (16*1024)

typedef struct {
    image_header_t* header;     // cached image header, read from file

    SHA256_CTX sha256;;

    uint32_t sector_index;      // sector index for install
    uint32_t sector_offset;     // the current sector offset
    uint32_t sector_free_space; // the current sector free space
} firmware_updater_context_t;


#define failed(code, msg) updater_failed(code, msg)

static void verify_async(void) {
  // vender header have verified
  // image header have verified

  updating_context_t *ctx = boot_ctx.updater.updating;
  firmware_updater_context_t *fctx = ctx->user_data;

  uint8_t chunk[CHUNK_SIZE] = {0};

  if (fs_file_tell(ctx->fp) == 0) {
    // setup hasher
    LOG_DEBUG(MODULE, "setup sha256 hasher for verify firmware");
    sha256_Init(&fctx->sha256);
    image_header_hash_update(&fctx->sha256, fctx->header);
    // seek to code offset
    fs_file_seek(ctx->fp, fctx->header->header_size, FS_SEEK_SET);
    return;
  }


  int r = fs_file_read(ctx->fp, chunk, CHUNK_SIZE);
  if (r < 0) {
    failed(UPDATER_ERROR_READ_FAILED, "Read firmware binary failed");
    return;
  }
  // hash update code
  sha256_Update(&fctx->sha256, chunk, r);

  if (fs_file_tell(ctx->fp) != fs_file_size(ctx->fp)) {
    return;
  }

  // have read all code
  uint8_t digest[SHA256_DIGEST_LENGTH];
  sha256_Final(&fctx->sha256, digest);
  LOG_HEXDUMP_DEBUG(MODULE, "", digest, sizeof(digest));

  image_print_header(fctx->header);

  // verify digest
  if (memcmp(digest, fctx->header->digest, sizeof(digest)) != 0) {
    LOG_DEBUG(MODULE, "firmware digest dismatch");
    LOG_HEXDUMP_DEBUG(MODULE, "digest in header", fctx->header->digest, sizeof(digest));
    failed(UPDATER_ERROR_IMAGE_VERIFY_FAILED, "Verify Firmware binary failed");
    return;
  }
  LOG_DEBUG(MODULE, "firmware digest verify success");

  // verify signature
  if (image_header_verify(fctx->header) != sectrue) {
    LOG_DEBUG(MODULE, "firmware signature verify failed");
    failed(UPDATER_ERROR_IMAGE_VERIFY_FAILED, "Verify Firmware binary failed");
    return;
  }
  LOG_DEBUG(MODULE, "firmware signature verify success");

  // seek to start
  fs_file_seek(ctx->fp, 0, FS_SEEK_SET);
  updater_success(UPDATER_STATE_INSTALL);
}

static void install_async(void) {
  updating_context_t *ctx = boot_ctx.updater.updating;
  firmware_updater_context_t *fctx = ctx->user_data;

  if (fs_file_tell(ctx->fp) == 0) {
    // setup mpu for firmware update
    mpu_update_enable();

    // setup sector info
    uint8_t sector = FIRMWARE_SECTORS[fctx->sector_index];
    fctx->sector_free_space = flash_sector_size(sector);
    fctx->sector_offset = 0;
    if (sectrue != flash_erase(sector)) {
      failed(UPDATER_ERROR_IMAGE_INSTALL_FAILED, "Erase firmware flash area failed");
      return;
    }
    hal_delay(1);
  }

  uint8_t chunk[CHUNK_SIZE] = {0};
  memset(chunk, 0, sizeof(chunk));
  int r = fs_file_read(ctx->fp, chunk, sizeof(chunk));
  if (r < 0) {
    failed(UPDATER_ERROR_IMAGE_INSTALL_FAILED, "Read firmware binary file failed");
    return;
  }

  if (sectrue != flash_unlock_write()) {
    failed(UPDATER_ERROR_IMAGE_INSTALL_FAILED, "Unlock firmware flash area failed");
    return;
  }

  uint8_t* p = chunk;
  uint8_t* end = p + r;
  while (p < end) {
    uint8_t sector = FIRMWARE_SECTORS[fctx->sector_index];
    if (fctx->sector_free_space == 0) {
      fctx->sector_index++;
      if (fctx->sector_index >= FIRMWARE_SECTORS_COUNT) {
        failed(UPDATER_ERROR_IMAGE_INSTALL_FAILED, "Firmware binary not flash area");
        return;
      }
      // update sector
      sector = FIRMWARE_SECTORS[fctx->sector_index];
      if (sectrue != flash_erase(sector)) {
        failed(UPDATER_ERROR_IMAGE_INSTALL_FAILED, "Erase firmware flash area failed");
        return;
      }
      fctx->sector_free_space = flash_sector_size(sector);
      hal_delay(1);
      fctx->sector_offset = 0;
      // erase fnction do lock flsh, need unlock again
      if (sectrue != flash_unlock_write()) {
        failed(UPDATER_ERROR_IMAGE_INSTALL_FAILED, "Unlock firmware flash area failed");
        return;
      }
    }
    if (sectrue != flash_write_words(sector, fctx->sector_offset, (uint32_t*)p)) {
      failed(UPDATER_ERROR_IMAGE_INSTALL_FAILED, "Firmware binary write failed");
      return;
    }
    p += 32;
    fctx->sector_free_space -= 32;
    fctx->sector_offset += 32;
  }

  if (sectrue != flash_lock_write()) {
    failed(UPDATER_ERROR_IMAGE_INSTALL_FAILED, "Unlock firmware flash area failed");
    return;
  }

  if (fs_file_tell(ctx->fp) != fs_file_size(ctx->fp)) {
    return;
  }

  // erase others
  int unused_sector_index = fctx->sector_index + 1;
  if (unused_sector_index < FIRMWARE_SECTORS_COUNT) {
    if (sectrue != flash_erase_sectors(&FIRMWARE_SECTORS[unused_sector_index], FIRMWARE_SECTORS_COUNT - unused_sector_index, NULL)) {
        LOG_DEBUG(MODULE, "Erase unused secotrs failed");
    }
  }

  // refresh boot_ctx
  boot_ctx.firmware.valided = true;
  LOG_DEBUG(MODULE, "firmware update success");
  fsm_sendSuccess("Firmware update success");
  updater_success(UPDATER_STATE_FINISHED);
}

static void __abort(void) {
  updating_context_t *ctx = boot_ctx.updater.updating;
  firmware_updater_context_t *fctx = ctx->user_data;
  if (!fctx) {
    return;
  }

  if (fctx->header) {
    vPortFree(fctx->header);
    fctx->header = NULL;
  }

  vPortFree(fctx);
  ctx->user_data = NULL;

  // disable mpu for firmware update
  mpu_update_disable();
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

void update_firmware(void) {
  updating_context_t *ctx = boot_ctx.updater.updating;
  // step 0. setup current version
  if (boot_ctx.firmware.valided) {
    const image_firmware_t* const fw = FIRMWARE;
    image_version_format(fw->header.version, ctx->cur_ver);
  } else {
    strcpy(ctx->cur_ver, "None");
  }

  // step 1. load header
  fs_file_seek(ctx->fp, 0, FS_SEEK_SET);
  firmware_updater_context_t *fctx = pvPortMalloc(sizeof(firmware_updater_context_t));
  memset(fctx, 0, sizeof(firmware_updater_context_t));
  fctx->header = pvPortMalloc(sizeof(image_header_t));
  memset(fctx->header, 0, sizeof(image_header_t));
  ctx->user_data = fctx;

  int r = fs_file_read(ctx->fp, fctx->header, sizeof(image_header_t));
  if (r < sizeof(image_header_t)) {
    LOG_DEBUG(MODULE, "Read firmware header filed");
    failed(UPDATER_ERROR_READ_FAILED, "Read firmware binary failed");
    // have not register updater, free manually
    vPortFree(fctx->header);
    vPortFree(fctx);
    ctx->user_data = NULL;
    return;
  }

  if (fctx->header->magic != FIRMWARE_IMAGE_MAGIC) {
    LOG_DEBUG(MODULE, "Invalid firmware magic number");
    failed(UPDATER_ERROR_IMAGE_VERIFY_FAILED, "Invalid firmware binary file");
    // have not register updater, free manually
    vPortFree(fctx->header);
    vPortFree(fctx);
    ctx->user_data = NULL;
    return;
  }

  if (fctx->header->code_size > FIRMWARE_IMAGE_MAX_SIZE || fctx->header->code_size != fs_file_size(ctx->fp)) {
    LOG_DEBUG(MODULE, "Invalid firmware code size");
    failed(UPDATER_ERROR_IMAGE_VERIFY_FAILED, "Invalid firmware binary file");
    // have not register updater, free manually
    vPortFree(fctx->header);
    vPortFree(fctx);
    ctx->user_data = NULL;
    return;
  }

  // seek to start
  fs_file_seek(ctx->fp, 0, FS_SEEK_SET);
  // step 2. setup new version
  image_version_format(fctx->header->version, ctx->new_ver);

  // step 3. register updater
  updater_register(&updater);
}
