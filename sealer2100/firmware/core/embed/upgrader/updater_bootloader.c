#include "boot_context.h"

#include <stdint.h>
#include <stdio.h>

#include "common.h"
#include "fs.h"
#include "flash.h"
#include "image.h"
#include "log.h"
#include "mpu.h"
#include "secure_heap.h"
#include "updater.h"
#include "sha2.h"

#define MODULE "boot updater"

#define CHUNK_SIZE (16 * 1024)

typedef struct {
    image_bootloader_t* boot; // cached image header, read from file

    SHA256_CTX sha256;

    uint32_t sector_index;      // sector index for install
    uint32_t sector_offset;     // the current sector offset
    uint32_t sector_free_space; // the current sector free space
} boot_updater_context_t;

static inline void failed(updater_error_t ucode, const char* msg) {
    updater_failed(ucode);
}

static void verify_async(void) {
    // vender header have verified
    // image header have verified

    updating_context_t* ctx = boot_ctx.updater.updating;
    boot_updater_context_t* fctx = ctx->user_data;

    uint8_t chunk[CHUNK_SIZE] = {0};

    if (fs_file_tell(ctx->fp) == 0) {
        // setup hasher
        LOG_DEBUG(MODULE, "setup sha256 hasher for verify bootloader");
        sha256_Init(&fctx->sha256);
        sha256_Update(&fctx->sha256, fctx->boot->vector_table, sizeof(fctx->boot->vector_table));
        image_header_hash_update(&fctx->sha256, &fctx->boot->header);
        // seek to code offset
        fs_file_seek(ctx->fp, sizeof(image_bootloader_t), FS_SEEK_SET);
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

    image_print_header(&fctx->boot->header);

    // verify digest
    if (memcmp(digest, fctx->boot->header.digest, sizeof(digest)) != 0) {
        LOG_DEBUG(MODULE, "bootloader digest dismatch");
        LOG_HEXDUMP_DEBUG(MODULE, "digest in header", fctx->boot->header.digest, sizeof(digest));
        failed(UPDATER_ERROR_IMAGE_VERIFY_FAILED, "Verify Bootloader binary failed");
        return;
    }
    LOG_DEBUG(MODULE, "bootloader digest verify success");

    // verify signature
    if (image_header_verify(&fctx->boot->header) != sectrue) {
        LOG_DEBUG(MODULE, "bootloader signature verify failed");
        failed(UPDATER_ERROR_IMAGE_VERIFY_FAILED, "Verify Bootloader binary failed");
        return;
    }
    LOG_DEBUG(MODULE, "bootloader signature verify success");

    // seek to start
    fs_file_seek(ctx->fp, 0, FS_SEEK_SET);
    updater_success(UPDATER_STATE_INSTALL);
}

static void install_async(void) {
    updating_context_t* ctx = boot_ctx.updater.updating;
    boot_updater_context_t* fctx = ctx->user_data;

    if (fs_file_tell(ctx->fp) == 0) {
#if defined(UPGRADER_DEBUG) && UPGRADER_DEBUG
        // setup mpu for bootloader update
        mpu_update_enable();
#endif

        // setup sector info
        uint8_t sector = BOOTLOADER_SECTORS[fctx->sector_index];
        fctx->sector_free_space = flash_sector_size(sector);
        fctx->sector_offset = 0;
        if (sectrue != flash_erase(sector)) {
            failed(UPDATER_ERROR_IMAGE_INSTALL_FAILED, "Erase bootloader flash area failed");
            return;
        }
        hal_delay(1);
    }

    uint8_t chunk[CHUNK_SIZE] = {0};
    memset(chunk, 0, sizeof(chunk));
    int r = fs_file_read(ctx->fp, chunk, sizeof(chunk));
    if (r < 0) {
        failed(UPDATER_ERROR_IMAGE_INSTALL_FAILED, "Read bootloader binary file failed");
        return;
    }

    if (sectrue != flash_unlock_write()) {
        failed(UPDATER_ERROR_IMAGE_INSTALL_FAILED, "Unlock bootloader flash area failed");
        return;
    }

    uint8_t* p = chunk;
    uint8_t* end = p + r;
    while (p < end) {
        uint8_t sector = BOOTLOADER_SECTORS[fctx->sector_index];
        if (fctx->sector_free_space == 0) {
            fctx->sector_index++;
            if (fctx->sector_index >= BOOTLOADER_SECTORS_COUNT) {
                failed(UPDATER_ERROR_IMAGE_INSTALL_FAILED, "Bootloader binary not flash area");
                return;
            }
            // update sector
            sector = BOOTLOADER_SECTORS[fctx->sector_index];
            if (sectrue != flash_erase(sector)) {
                failed(UPDATER_ERROR_IMAGE_INSTALL_FAILED, "Erase bootloader flash area failed");
                return;
            }
            fctx->sector_free_space = flash_sector_size(sector);
            hal_delay(1);
            fctx->sector_offset = 0;
            // erase fnction do lock flsh, need unlock again
            if (sectrue != flash_unlock_write()) {
                failed(UPDATER_ERROR_IMAGE_INSTALL_FAILED, "Unlock bootloader flash area failed");
                return;
            }
        }
        if (sectrue != flash_write_words(sector, fctx->sector_offset, (uint32_t*)p)) {
            failed(UPDATER_ERROR_IMAGE_INSTALL_FAILED, "Bootloader binary write failed");
            return;
        }
        p += 32;
        fctx->sector_free_space -= 32;
        fctx->sector_offset += 32;
    }

    if (sectrue != flash_lock_write()) {
        failed(UPDATER_ERROR_IMAGE_INSTALL_FAILED, "Unlock bootloader flash area failed");
        return;
    }

    if (fs_file_tell(ctx->fp) != fs_file_size(ctx->fp)) {
        return;
    }

    // erase others
    int unused_sector_index = fctx->sector_index + 1;
    if (unused_sector_index < BOOTLOADER_SECTORS_COUNT) {
        if (sectrue !=
            flash_erase_sectors(
                &BOOTLOADER_SECTORS[unused_sector_index], BOOTLOADER_SECTORS_COUNT - unused_sector_index, NULL
            )) {
            LOG_DEBUG(MODULE, "Erase unused secotrs failed");
        }
    }

    LOG_DEBUG(MODULE, "bootloader update success");
    boot_ctx.upgrader_op.state = UPGRADER_OP_STATE_BOOT_INSTALL_DONE;
    boot_ctx.upgrader_op.active_time = hal_ticks_ms();
    updater_success(UPDATER_STATE_FINISHED);
}

static void __abort(void) {
    updating_context_t* ctx = boot_ctx.updater.updating;
    boot_updater_context_t* fctx = ctx->user_data;
    if (!fctx) {
        return;
    }

    if (fctx->boot) {
        vPortFree(fctx->boot);
        fctx->boot = NULL;
    }

    vPortFree(fctx);
    ctx->user_data = NULL;
#if defined(UPGRADER_DEBUG) && UPGRADER_DEBUG
    // disable mpu for bootloader update
    mpu_update_disable();
#endif
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

void update_bootloader(void) {
    updating_context_t* ctx = boot_ctx.updater.updating;
    // step 0. setup current version
    const image_bootloader_t* const bl = BOOTLOADER;
    image_version_format(bl->header.version, ctx->cur_ver);

    // step 1. load header
    fs_file_seek(ctx->fp, 0, FS_SEEK_SET);
    boot_updater_context_t* fctx = pvPortMalloc(sizeof(boot_updater_context_t));
    memset(fctx, 0, sizeof(boot_updater_context_t));
    fctx->boot = pvPortMalloc(sizeof(image_bootloader_t));
    memset(fctx->boot, 0, sizeof(image_bootloader_t));
    ctx->user_data = fctx;

    int r = fs_file_read(ctx->fp, fctx->boot, sizeof(image_bootloader_t));
    if (r != sizeof(image_bootloader_t)) {
        LOG_DEBUG(MODULE, "Read bootloader filed");
        failed(UPDATER_ERROR_READ_FAILED, "Read bootloader failed");
        // have not register updater, free manually
        vPortFree(fctx->boot);
        vPortFree(fctx);
        ctx->user_data = NULL;
        return;
    }

    if (fctx->boot->header.magic != BOOTLOADER_IMAGE_MAGIC) {
        LOG_DEBUG(MODULE, "Invalid bootloader magic number");
        failed(UPDATER_ERROR_IMAGE_VERIFY_FAILED, "Invalid bootloader binary file");
        // have not register updater, free manually
        vPortFree(fctx->boot);
        vPortFree(fctx);
        ctx->user_data = NULL;
        return;
    }

    if (fctx->boot->header.code_size > BOOTLOADER_IMAGE_MAX_SIZE ||
        fctx->boot->header.code_size != fs_file_size(ctx->fp)) {
        LOG_DEBUG(MODULE, "Invalid bootloader code size");
        failed(UPDATER_ERROR_IMAGE_VERIFY_FAILED, "Invalid bootloader binary file");
        // have not register updater, free manually
        vPortFree(fctx->boot);
        vPortFree(fctx);
        ctx->user_data = NULL;
        return;
    }

    // seek to start
    fs_file_seek(ctx->fp, 0, FS_SEEK_SET);
    // step 2. setup new version
    image_version_format(fctx->boot->header.version, ctx->new_ver);

    // step 3. register updater
    updater_register(&updater);
}
