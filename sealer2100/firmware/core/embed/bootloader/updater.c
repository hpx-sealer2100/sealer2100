#include "updater.h"

#include <assert.h>

#include "boot_context.h"
#include "fs.h"
#include "fsm.h"
#include "image.h"
#include "iris.h"
#include "log.h"
#include "messages-common.pb.h"
#include "power_manager.h"
#include "secure_heap.h"

#define MODULE "updater"


static updater_t unknow_image_type_updater = {
    .get_error_fn = updater_get_error,
};

static updating_context_t* updating_context_create(void) {
  updating_context_t *ctx = pvPortMalloc(sizeof(updating_context_t));
  memset(ctx, 0, sizeof(updating_context_t));
  return ctx;
}

void updater_update_image(const char *path, bool reboot_on_success) {
  updater_context_t *ctx = &boot_ctx.updater;
  ctx->updating = updating_context_create();
  strcpy(ctx->updating->path, path);
  ctx->updating->reboot_on_success = reboot_on_success;
  // use unknow image type updater for default
  // the special updater will overwrite it
  updater_register(&unknow_image_type_updater);

  if (!fs_is_file(&boot_ctx.fs, path)) {
    updater_failed(UPDATER_ERROR_INVALID_PATH, "Invalid path");
    return;
  }
  ctx->updating->fp = pvPortMalloc(sizeof(fs_file_t));
  int r = fs_file_open(&boot_ctx.fs, ctx->updating->fp, path, FS_O_RDONLY);
  if (r < 0) {
    updater_failed(UPDATER_ERROR_INVALID_PATH, "Invalid path");
    return;
  }
  // firmware vendor header
  uint8_t data[4*1024] = {0};
  r = fs_file_read(ctx->updating->fp, data, sizeof(data));
  if (r != sizeof(data)) {
    updater_failed(UPDATER_ERROR_READ_FAILED, "Read binary failed");
    return;
  }
  // reset cursor to start
  fs_file_seek(ctx->updating->fp, 0, FS_SEEK_SET);

  ctx->updating->image_type = image_get_type(data);

  switch (ctx->updating->image_type) {
    default:
    case IMAGE_TYPE_UNKNOWN:
      LOG_DEBUG(MODULE, "unknown image type of :%s", path);
      updater_failed(UPDATER_ERROR_UNKNOWN_IMAGE_TYPE, "Unknown image type");
      return;
    case IMAGE_TYPE_BOOTLOADER:
      // bootloader can't update self, it updated by firmware, so filed
      LOG_DEBUG(MODULE, "update bootloader self, not support");
      updater_failed(UPDATER_ERROR_UNKNOWN_IMAGE_TYPE, "Bootloader can't update self");
      return;
    case IMAGE_TYPE_FIRMWARE:
      LOG_DEBUG(MODULE, "updating FIRMWARE");
      update_firmware();
      break;
    case IMAGE_TYPE_BLE:
      LOG_DEBUG(MODULE, "updating BLE ...");
      update_ble();
      break;
    case IMAGE_TYPE_SE:
      LOG_DEBUG(MODULE, "updating SE ...");
      update_se();
      break;
    case IMAGE_TYPE_IRIS:
      LOG_DEBUG(MODULE, "updating IRIS ...");
      update_iris();
      break;
  }
  if (ctx->state == UPDATER_STATE_ERROR) {
    return;
  }
  LOG_DEBUG(MODULE, "cur_ver: %s, new_ver: %s", ctx->updating->cur_ver, ctx->updating->new_ver);
  bool version_match = false;
  // image type: iris or (iris ble se)
  if (ctx->updating->image_type == IMAGE_TYPE_IRIS) {
    iris_version_t cur_ver = {0}, new_ver = {0};
    if (iris_version_parse(ctx->updating->cur_ver, &cur_ver) != 0) {
      updater_failed(UPDATER_ERROR_INVALID_VERSION, "Invalid version");
      return;
    }
    if (iris_version_parse(ctx->updating->new_ver, &new_ver) != 0) {
      updater_failed(UPDATER_ERROR_INVALID_VERSION, "Invalid version");
      return;
    }
    version_match = iris_version_compare(&new_ver, &cur_ver) > 0;
  } else {
    image_version_t cur_ver = {0}, new_ver = {0};
    if (image_version_parse(ctx->updating->cur_ver, &cur_ver) != 0) {
      updater_failed(UPDATER_ERROR_INVALID_VERSION, "Invalid version");
      return;
    }
    if (image_version_parse(ctx->updating->new_ver, &new_ver) != 0) {
      updater_failed(UPDATER_ERROR_INVALID_VERSION, "Invalid version");
      return;
    }
    version_match = image_version_compare(&new_ver, &cur_ver) > 0;
  }

  if (!version_match) {
    LOG_ERROR(MODULE, "Version not match");
    // for develop mode, skip
#if PRODUCTION
    updater_failed(UPDATER_ERROR_INVALID_VERSION, "Version not match");
    return;
#endif
  }

  updater_begin(UPDATER_STATE_NOTIFY_APP);
}

void updater_begin(updater_state_t state) {
  updater_success(state);
  // enter tiny mode
  msg_tiny_set(1);
}

void updater_register(updater_t *updater) {
  boot_ctx.updater.updater = updater;
}

void updater_failed(updater_error_t error, const char* msg) {
  updater_context_t *ctx = &boot_ctx.updater;
  ctx->state = UPDATER_STATE_ERROR;
  ctx->updating->error = error;
  fsm_sendFailure(FailureType_Failure_ProcessError, msg);
}

void updater_success(updater_state_t state) {
  updater_context_t *ctx = &boot_ctx.updater;
  ctx->state = state;
}

void updater_finished() {
  // exit tiny mode
  msg_tiny_set(0);
  // set device busy false
  boot_ctx.device.busy = false;
  // clean `updater` resource
  updater_context_t *ctx = &boot_ctx.updater;
  if (ctx->updater->abort_fn) ctx->updater->abort_fn();

  // clean updater resource
  if (ctx->updating->fp) {
    fs_file_close(ctx->updating->fp);
    vPortFree(ctx->updating->fp);
    ctx->updating->fp = NULL;
    ctx->updating = NULL;
  }
  vPortFree(ctx->updating);
  ctx->updating = NULL;
  ctx->state = UPDATER_STATE_NONE;
  // when finished enable long press power key restart device
  pm_battery_long_press_reset(true);
}

const char* updater_error_format(updater_error_t error) {
  switch (error) {
    case UPDATER_ERROR_SUCCESS:
      return "Success";
    case UPDATER_ERROR_INVALID_PATH:
      return "Invalid path";
    case UPDATER_ERROR_READ_FAILED:
      return "Image binary file read failed";
    case UPDATER_ERROR_UNKNOWN_IMAGE_TYPE:
      return "Unknow image type of file";
    case UPDATER_ERROR_INVALID_STATE:
      return "Invalid state";
    case UPDATER_ERROR_INVALID_VERSION:
      return "Invalid version";
    case UPDATER_ERROR_IMAGE_VERIFY_FAILED:
      return "Image binary file verify failed";
    case UPDATER_ERROR_IMAGE_INSTALL_FAILED:
      return "Image binary install failed";
    default:
      // not reachable
      assert(false);
  }
}

const char* updater_get_name(void) {
    return image_get_desc_name(boot_ctx.updater.updating->image_type);
}

const char* updater_get_cur_ver(void) {
    return boot_ctx.updater.updating->cur_ver;
}

const char* updater_get_new_ver(void) {
    return boot_ctx.updater.updating->new_ver;
}

const char* updater_get_error(void) {
  updater_context_t *ctx = &boot_ctx.updater;
  return updater_error_format(ctx->updating->error);
}

int updater_get_processed(void) {
    fs_file_t* fp = boot_ctx.updater.updating->fp;

    uint32_t processed = fs_file_tell(fp);
    uint32_t totoal = fs_file_size(fp);
    return (processed * 100)/totoal;
}

void updater_canceled(void) {
    fsm_sendFailure(FailureType_Failure_ActionCancelled, "User cancelled");
}
