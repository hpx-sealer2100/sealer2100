#include "fsm_msg_fs.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/_intsup.h>

#include "boot_context.h"
#include "bootui.h"
#include "fs.h"
#include "fsm.h"
#include "log.h"
#include "messages-common.pb.h"
#include "messages-fs.pb.h"
#include "messages.h"
#include "messages.pb.h"
#include "secure_heap.h"
#include "uart_log.h"

#define MODULE "fs message"

#define FS_FILE_OPERATION_TIMEOUT_MS (10*1000)

static void fs_message_ctx_abort(void) {
  // reset fs message timeout
  boot_fs_msg_monitor_reset();

  if(!boot_ctx.file_operating) {
    return;
  }

  if (boot_ctx.file_operating->file) {
    fs_file_close(boot_ctx.file_operating->file);
    boot_ctx.file_operating->file = NULL;
  }
  vPortFree(boot_ctx.file_operating);
  boot_ctx.file_operating = NULL;
  ui_fs_file_op_reset();
  ui_home();
}

#define fs_message_ctx_reset() fs_message_ctx_abort()

static bool is_same_file(const char* path) {
    if (boot_ctx.file_operating && strcmp(boot_ctx.file_operating->path, path) == 0) {
        return true;
    }
    return false;
}

static boot_file_t* setup_file_read(const char* path) {
  boot_file_t *ctx = NULL;
  // check if file is already opened
  if (boot_ctx.file_operating) {
    if (is_same_file(path)) {
      LOG_DEBUG(MODULE, "pending read %s", path);
      ctx = boot_ctx.file_operating;
      return ctx;
    } else {
      // file is opend, but not the same file
      fs_message_ctx_abort();
    }
  }

  fs_info_t info = {0};
  int ret = 0;
  ret = fs_stat(&boot_ctx.fs, path, &info);
  // check if file is regular file
  if (ret < 0 || info.type != FS_TYPE_REG) {
    fsm_sendFailure(FailureType_Failure_ProcessError, "Invalid path");
    return NULL;
  }

  LOG_DEBUG(MODULE, "reading %s", path);
  ctx = pvPortMalloc(sizeof(boot_file_t));
  fs_file_t *file = pvPortMalloc(sizeof(fs_file_t));
  ret = fs_file_open(&boot_ctx.fs, file, path, FS_O_RDONLY);
  if (ret < 0) {
    fsm_sendFailure(FailureType_Failure_ProcessError, "Open file failed");
    vPortFree(file); file = NULL;
    vPortFree(ctx); ctx = NULL;
    return NULL;
  }
  // cache file path
  strcpy(ctx->path, path);
  ctx->file = file;
  boot_ctx.file_operating = ctx;
  ui_fs_file_op_reset();
  ui_fs_file_op("Reading ...");

  // setup fs file operation timeout
  boot_fs_msg_monitor_init(fs_message_ctx_abort, FS_FILE_OPERATION_TIMEOUT_MS);
  return ctx;
}

static boot_file_t* setup_file_write(const char* path) {
  boot_file_t *ctx = NULL;
  int ret = 0;

  if (boot_ctx.file_operating) {
    // check if file is already opened
    if (is_same_file(path)) {
      LOG_DEBUG(MODULE, "pending write %s", path);
      ctx = boot_ctx.file_operating;
      return ctx;
    } else {
      // file is opend, but not the same file
      fs_message_ctx_abort();
    }
  }

  LOG_DEBUG(MODULE, "writing %s", path);
  ctx = pvPortMalloc(sizeof(boot_file_t));
  fs_file_t *file = pvPortMalloc(sizeof(fs_file_t));
  int flags = FS_O_RDWR | FS_O_CREAT;
  fs_info_t info = {0};
  ret = fs_stat(&boot_ctx.fs, path, &info);
  if (ret == 0) {
    if (info.type == FS_TYPE_REG) {
      // file exist, truncate it
      flags |= FS_O_TRUNC;
    } else {
      // path exist, but is a directory
      fsm_sendFailure(FailureType_Failure_ProcessError, "Invalid path");
      return NULL;
    }
 }

  ret = fs_file_open(&boot_ctx.fs, file, path, flags);
  if (ret < 0) {
    fsm_sendFailure(FailureType_Failure_ProcessError, "Open file failed");
    vPortFree(file); file = NULL;
    vPortFree(ctx); ctx = NULL;
    return NULL;
  }
  // cache file path
  strcpy(ctx->path, path);
  ctx->file = file;
  boot_ctx.file_operating = ctx;
  ui_fs_file_op_reset();
  const char* basename = fs_base_name(path);
  char transfering[128] = {0};
  sprintf(transfering, "Transfing %s ...", basename);
  ui_fs_file_op(transfering);
  // setup fs file operation timeout
  boot_fs_msg_monitor_init(fs_message_ctx_abort, FS_FILE_OPERATION_TIMEOUT_MS);
  return ctx;
}

void fsm_msgFsFileRead(const FsFileRead* msg){
  (void)msg;
  CHECK_PARAM(strlen(msg->file.path) > 0, "Path not provide");
  RESP_INIT(FsFile);

  CHECK_PARAM(msg->file.len <= sizeof(resp->data.bytes), "Invalid length");
  boot_file_t *ctx = setup_file_read(msg->file.path);
  if (!ctx) {
    goto err;
  }

  if (fs_file_size(ctx->file) < msg->file.offset) {
    fsm_sendFailure(FailureType_Failure_ProcessError, "Invalid param");
    goto err;
  }
  if (fs_file_tell(ctx->file) != msg->file.offset) {
    fs_file_seek(ctx->file, msg->file.offset, FS_SEEK_SET);
  }

  int ret = fs_file_read(ctx->file, resp->data.bytes, msg->file.len);
  if (ret < 0) {
    fsm_sendFailure(FailureType_Failure_ProcessError, "Read file failed");
    goto err;
  }
  // touch fs file operation timeout
  boot_fs_msg_monitor_touch();

  strcpy(resp->path, msg->file.path);
  resp->offset = msg->file.offset;
  resp->len = msg->file.len;
  resp->has_data = true;
  resp->data.size = ret;
  resp->has_processed_byte = true;
  resp->processed_byte = ret;

  msg_write(MessageType_MessageType_FsFile, resp);
  int file_size = fs_file_size(ctx->file);
  if (msg->file.offset + ret < file_size) {
    int pct = (msg->file.offset + ret) * 100 / file_size;
    ui_fs_file_op_update(pct);
  } else {
    fs_message_ctx_reset();
  }
  return;
err:
  fs_message_ctx_abort();
}
void fsm_msgFsFileWrite(const FsFileWrite* msg){
  (void)msg;
  CHECK_PARAM(strlen(msg->file.path) > 0, "Path not provide");
  CHECK_PARAM(msg->file.has_data && msg->file.data.size > 0, "Invalid param");

  boot_file_t *ctx = setup_file_write(msg->file.path);
  if (!ctx) {
    goto err;
  }

  int ret = 0;

  if (fs_file_size(ctx->file) < msg->file.offset) {
    fsm_sendFailure(FailureType_Failure_ProcessError, "Invalid param");
    goto err;
  }

  if (fs_file_tell(ctx->file) != msg->file.offset) {
    fs_file_seek(ctx->file, msg->file.offset, FS_SEEK_SET);
  }

  ret = fs_file_write(ctx->file, msg->file.data.bytes, msg->file.data.size);

  if (ret < 0) {
    fsm_sendFailure(FailureType_Failure_ProcessError, "Write file failed");
    goto err;
  }
  // touch fs file operation timeout
  boot_fs_msg_monitor_touch();

  if (msg->ui_percentage < 100) {
    ui_fs_file_op_update(msg->ui_percentage);
  } else {
    fs_message_ctx_reset();
  }
  RESP_INIT(FsFile);
  strcpy(resp->path, msg->file.path);
  resp->offset = msg->file.offset;
  resp->len = msg->file.len;
  resp->has_processed_byte = true;
  resp->processed_byte = ret;
  msg_write(MessageType_MessageType_FsFile, resp);
  return;
err:
  fs_message_ctx_abort();
}

void fsm_msgFsMkdir(const FsMkdir* msg){
  CHECK_PARAM(strlen(msg->path) > 0, "Path not provide");

  fs_info_t info = {0};
  int ret = fs_stat(&boot_ctx.fs, msg->path, &info);
  if (ret < 0 && ret == FS_ERR_NOENT) {
    ret = fs_mkdir(&boot_ctx.fs, msg->path);
    if (ret < 0) {
      fsm_sendFailure(FailureType_Failure_ProcessError, "Create directory failed");
      return;
    }
    fsm_sendSuccess("Create directory success");
    return;
  } else if (ret < 0) {
    fsm_sendFailure(FailureType_Failure_ProcessError, "Get path info failed");
    return;
  }
  if (info.type != FS_TYPE_DIR) {
    fsm_sendFailure(FailureType_Failure_ProcessError, "Path already exist, but is not directory");
    return;
  }

  // already exsit
  fsm_sendSuccess(NULL);
}

void fsm_msgFsRemove(const FsRemove* msg){
  CHECK_PARAM(strlen(msg->path) > 0, "Path not provide");

  fs_info_t info = {0};
  int ret = fs_stat(&boot_ctx.fs, msg->path, &info);
  if (ret < 0 && ret == FS_ERR_NOENT) {
    // not exist
    fsm_sendSuccess(NULL);
    return;
  } else if (ret < 0) {
    fsm_sendFailure(FailureType_Failure_ProcessError, "Get path info failed");
    return;
  }

  ret = fs_remove(&boot_ctx.fs, msg->path);
  if (ret < 0) {
    fsm_sendFailure(FailureType_Failure_ProcessError, "Delete path failed");
    return;
  }
  fsm_sendSuccess("Delete path success");
}


void fsm_msgFsStat(const FsStat* msg) {
  CHECK_PARAM(strlen(msg->path) > 0, "Path not provide");
  fs_info_t info = {0};
  int ret = fs_stat(&boot_ctx.fs, msg->path, &info);
  if (ret < 0) {
    fsm_sendFailure(FailureType_Failure_ProcessError, "Get path info failed");
    return;
  }
  RESP_INIT(FsInfo);

  resp->type = info.type == FS_TYPE_DIR ? FsType_DIRECTORY : FsType_FILE;
  resp->size = info.size;
  strcpy(resp->name, info.name);
  msg_write(MessageType_MessageType_FsInfo, resp);
}
void fsm_msgFsFsStat(const FsFsStat *msg) {
  fs_fsinfo_t info = {0};
  int ret = fs_fs_stat(&boot_ctx.fs, &info);
  if (ret < 0) {
    fsm_sendFailure(FailureType_Failure_ProcessError, "Get fs info failed");
    return;
  }
  RESP_INIT(FsFsInfo);
  resp->block_count = info.block_count;
  resp->block_size = info.block_size;
  resp->block_used = info.block_used;
  msg_write(MessageType_MessageType_FsFsInfo, resp);
}
void fsm_msgFsRead(const FsRead* msg) {
  CHECK_PARAM(strlen(msg->path) > 0, "Path not provide");
  CHECK_PARAM(msg->size > 0, "Invalid param");
  boot_file_t *ctx = setup_file_read(msg->path);
  if (!ctx) {
    goto err;
  }

  int ret = 0;

  if (fs_file_size(ctx->file) < msg->offset) {
    fsm_sendFailure(FailureType_Failure_ProcessError, "Invalid param");
    goto err;
  }
  if (fs_file_tell(ctx->file) != msg->offset) {
    fs_file_seek(ctx->file, msg->offset, FS_SEEK_SET);
  }
  RESP_INIT(FsChunk);
  ret = fs_file_read(ctx->file, resp->data.bytes, msg->size);
  if (ret < 0) {
    fsm_sendFailure(FailureType_Failure_ProcessError, "Read file failed");
    goto err;
  }
  // touch fs file operation timeout
  boot_fs_msg_monitor_touch();

  resp->data.size = ret;
  resp->has_data_hash = true;
  resp->data_hash.size = 32;
  sha256_Raw(resp->data.bytes, resp->data.size, resp->data_hash.bytes);

  msg_write(MessageType_MessageType_FsChunk, resp);
  int file_size = fs_file_size(ctx->file);
  if (msg->offset + ret < file_size) {
    int pct = (msg->offset + ret) * 100 / file_size;
    ui_fs_file_op_update(pct);
  } else {
    fs_message_ctx_reset();
  }
  return;
err:
  fs_message_ctx_abort();
}

void checksum_ui_setup(char* txt) {
  ui_fs_file_op_reset();
  ui_fs_file_op(txt);
  lv_timer_handler();
}

void checksum_ui_updater(int pct) {
  ui_fs_file_op_update(pct);
  lv_timer_handler();
}

void generate_file_checksum(const char* path, fs_file_t* fchecksum) {
  const char* name = fs_base_name(path);
  char op[128];

  snprintf(op, sizeof(op), "Checksuming %s ...", name);
  checksum_ui_setup(op);

  fs_file_t f;
  int ret = fs_file_open(&boot_ctx.fs, &f, path, FS_O_RDONLY);
  if (ret < 0) {
    LOG_ERROR(MODULE, "open failed: %d", ret);
    return;
  }

  fs_info_t info = {0};
  fs_stat(&boot_ctx.fs, path, &info);

  // set sha256 hash to checksum
  SHA256_CTX sha256;
  sha256_Init(&sha256);

  uint32_t processed = 0;
  uint8_t chunk[16*1024];
  while (processed < info.size) {
    ret = fs_file_read(&f, chunk, sizeof(chunk));
    if (ret < 0) {
      LOG_ERROR(MODULE, "read failed: %d", ret);
      break;
    }
    processed += ret;
    sha256_Update(&sha256, chunk, ret);
    checksum_ui_updater(processed * 100 / info.size);
  }

  char checksum[SHA256_DIGEST_LENGTH*2+1];
  sha256_End(&sha256, checksum);

  // save checksum
  // <checksum> <path>
  // reuse chunk buffer
  memset(chunk, 0, 512);
  sprintf((char*)chunk, "%s %s\n", checksum, path);
  // LOG_DEBUG(MODULE, (char*)chunk);
  // uart_log_flush();
  fs_file_write(fchecksum, chunk, strlen((char*)chunk));
}

#define CHECKSUMS_FILE "/res/checksums.txt"

void generate_checksums_in_dir(const char* path, fs_file_t* fchecksum) {
    LOG_DEBUG(MODULE, "generating files checksum in dir: %s", path);
    fs_t* fs = &boot_ctx.fs;
    fs_dir_t dir;
    int err = fs_dir_open(fs, &dir, path);
    if (err) {
        LOG_ERROR(MODULE, "open failed: %d", err);
        return;
    }

    while (1) {
        fs_info_t info;
        err = fs_dir_read(&dir, &info);
        if (err == 0) {
            // end of directory
            LOG_DEBUG(MODULE, "list complete of %s", path);
            break;
        }
        if (err < 0) {
            LOG_ERROR(MODULE, "dir read failed: %d of %s", err, path);
            break;
        }
        char full_path[128];
        fs_path_join(full_path, path, info.name, NULL);
        if (info.type == FS_TYPE_DIR) {
            if (strcmp(info.name, ".") == 0 || strcmp(info.name, "..") == 0) {
                // skip self and parent directory
                continue;
            }
            // recurse into subdirectory
            generate_checksums_in_dir(full_path, fchecksum);
        }
        if (info.type == FS_TYPE_REG) {
            // skip /res/checksums.txt
            if (strcmp(full_path, CHECKSUMS_FILE) == 0) {
                continue;
            }
            generate_file_checksum(full_path, fchecksum);
        }
    }
    fs_dir_close(&dir);
}

void fsm_msgFsChecksums(const FsChecksums* msg) {
  fs_file_t fchecksum;
  int ret = fs_file_open(&boot_ctx.fs, &fchecksum, CHECKSUMS_FILE, FS_O_RDWR|FS_O_CREAT);
  fs_file_truncate(&fchecksum, 0);
  if (ret < 0) {
    LOG_ERROR(MODULE, "open failed: %d", ret);
    fsm_sendFailure(FailureType_Failure_ProcessError, "Open file failed");
    return;
  }
  generate_checksums_in_dir("/res", &fchecksum);
  fs_file_close(&fchecksum);
  ui_fs_file_op_reset();
  ui_home();
  fsm_sendSuccess(NULL);
}

void fsm_msgFsWrite(const FsWrite* msg) {
  CHECK_PARAM(strlen(msg->path) > 0, "Path not provide");
  CHECK_PARAM(msg->chunk.data.size > 0, "Invalid param");
  boot_file_t *ctx = setup_file_write(msg->path);
  if (!ctx) {
    goto err;
  }

  const FsChunk* chunk = &msg->chunk;
  if (chunk->has_data_hash) {
    // check hash
    uint8_t hash[SHA256_DIGEST_LENGTH];
    sha256_Raw(chunk->data.bytes, chunk->data.size, hash);
    if (memcmp(hash, chunk->data_hash.bytes, SHA256_DIGEST_LENGTH) != 0) {
      fsm_sendFailure(FailureType_Failure_ProcessError, "Hash check failed");
      goto err;
    }
  }

  if (msg->has_offset) {
    if (fs_file_size(ctx->file) < msg->offset) {
      fsm_sendFailure(FailureType_Failure_ProcessError, "Invalid param");
      goto err;
    }
    if (fs_file_tell(ctx->file) != msg->offset) {
      fs_file_seek(ctx->file, msg->offset, FS_SEEK_SET);
    }
   }

  int ret = fs_file_write(ctx->file, chunk->data.bytes, chunk->data.size);

  if (ret != chunk->data.size) {
    fsm_sendFailure(FailureType_Failure_ProcessError, "Write file failed");
    goto err;
  }
  // touch fs file operation timeout
  boot_fs_msg_monitor_touch();

  int file_size = fs_file_size(ctx->file);
  if (msg->total != file_size) {
    int pct = file_size * 100 / msg->total;
    ui_fs_file_op_update(pct);
  } else {
    fs_message_ctx_reset();
  }
  fsm_sendSuccess(NULL);
  return;

err:
  fs_message_ctx_abort();
}
