#include <stdint.h>
#include <stdbool.h>
#include <memory.h>
#include <string.h>

#include "alignment.h"
#include "common.h"
#include "aes/aesccm.h"
#include "hmac.h"
#include "iris.h"
#include "memzero.h"
#include "sha2.h"
#include "uart_log.h"

#ifndef LOG_DEBUG
#include "log.h"
#define MODULE "iris sec channel"
#endif

#define IRIS_TESTING 1

#define SEC_SCTR_SIZE 1
#define SEC_RND_SIZE 32
#define SEC_SEQ_SIZE 4
#define SEC_MAC_SIZE 8
#define SEC_PVER_SIZE 1
#define SEC_KEYS_SIZE 40
#define SEC_NONCE_SIZE 4
#define SEC_OVERHEAD_SIZE (SEC_SCTR_SIZE + SEC_SEQ_SIZE + SEC_MAC_SIZE)

// encryption/decryption nonce: NONCE|SEQ
#define SEC_ENC_NONCE_SIZE (SEC_NONCE_SIZE + SEC_SEQ_SIZE)

#define HELLO_RESPONSE_SIZE \
  (SEC_SCTR_SIZE + SEC_PVER_SIZE + SEC_RND_SIZE + SEC_SEQ_SIZE)

#define CIPHERTEXT_SIZE ((SEC_RND_SIZE + SEC_SEQ_SIZE) + SEC_MAC_SIZE)
#define HANDSHAKE_FINISHED_SIZE (SEC_SCTR_SIZE + SEC_SEQ_SIZE + CIPHERTEXT_SIZE)

// Security control bytes
enum {
  SCTR_HELLO = 0x00,
  SCTR_FINISHED = 0x08,
  SCTR_PROTECTED = 0x23,
};

// Security protocol versions
enum {
  PVER_FLAG_PRE_SHARED_SECRET = 1 << 0,
  PVER_FLAG_PRE_ECDHE = 1 << 1,
};

typedef union {
  uint8_t encryption_nonce[SEC_ENC_NONCE_SIZE];
  struct {
    uint8_t nonce[SEC_NONCE_SIZE];
    uint8_t seq[SEC_SEQ_SIZE];
  };
} encryption_nonce_t;

typedef struct {
  bool sec_chan_established;
  aes_encrypt_ctx enc_ctx;
  aes_encrypt_ctx dec_ctx;
  encryption_nonce_t enc_nonce;
  encryption_nonce_t dec_nonce;
} iris_sec_channel_t;

static iris_sec_channel_t iris_sec_ctx;

static void tls_prf_sha256(const uint8_t* secret, size_t secret_size,
                           const uint8_t* label, size_t label_size,
                           uint8_t* seed, size_t seed_size, uint8_t* output,
                           size_t out_len) {
  uint32_t idig[SHA256_DIGEST_LENGTH / sizeof(uint32_t)] = {0};
  uint32_t odig[SHA256_DIGEST_LENGTH / sizeof(uint32_t)] = {0};
  uint8_t a[SHA256_DIGEST_LENGTH] = {0};
  uint8_t result[SHA256_DIGEST_LENGTH] = {0};
  SHA256_CTX ctx = {0};

  // Prepare inner and outer digest.
  hmac_sha256_prepare(secret, secret_size, odig, idig);

  // a = HMAC(secret, label + seed)
  sha256_Init_ex(&ctx, idig, 8 * SHA256_BLOCK_LENGTH);
  sha256_Update(&ctx, label, label_size);
  sha256_Update(&ctx, seed, seed_size);
  sha256_Final(&ctx, a);
  sha256_Init_ex(&ctx, odig, 8 * SHA256_BLOCK_LENGTH);
  sha256_Update(&ctx, a, SHA256_DIGEST_LENGTH);
  sha256_Final(&ctx, a);

  while (1) {
    // result = HMAC(secret, a + label + seed)
    sha256_Init_ex(&ctx, idig, 8 * SHA256_BLOCK_LENGTH);
    sha256_Update(&ctx, a, SHA256_DIGEST_LENGTH);
    sha256_Update(&ctx, label, label_size);
    sha256_Update(&ctx, seed, seed_size);
    sha256_Final(&ctx, result);
    sha256_Init_ex(&ctx, odig, 8 * SHA256_BLOCK_LENGTH);
    sha256_Update(&ctx, result, SHA256_DIGEST_LENGTH);
    sha256_Final(&ctx, result);

    if (out_len <= SHA256_DIGEST_LENGTH) {
      break;
    }

    memcpy(output, result, SHA256_DIGEST_LENGTH);
    output += SHA256_DIGEST_LENGTH;
    out_len -= SHA256_DIGEST_LENGTH;

    // a = HMAC(secret, a)
    sha256_Init_ex(&ctx, idig, 8 * SHA256_BLOCK_LENGTH);
    sha256_Update(&ctx, a, SHA256_DIGEST_LENGTH);
    sha256_Final(&ctx, a);
    sha256_Init_ex(&ctx, odig, 8 * SHA256_BLOCK_LENGTH);
    sha256_Update(&ctx, a, SHA256_DIGEST_LENGTH);
    sha256_Final(&ctx, a);
  }

  memcpy(output, result, out_len);
  memzero(idig, sizeof(idig));
  memzero(odig, sizeof(odig));
  memzero(a, sizeof(a));
  memzero(result, sizeof(result));
}

static void increment_seq(uint8_t seq[SEC_SEQ_SIZE]) {
  for (int i = 3; i >= 0; --i) {
    seq[i]++;
    if (seq[i]) {
      return;
    }
  }

  // overflow
  memzero(&iris_sec_ctx, sizeof(iris_sec_channel_t));
}


static void iris_sec_channel_init(void) {
  memzero(&iris_sec_ctx, sizeof(iris_sec_channel_t));
  return;
}

// reset secure channel context
#define iris_sec_channel_reset() iris_sec_channel_init()

typedef enum {
  IRIS_HANDSHAKE_STATE_NONE,
  IRIS_HANDSHAKE_STATE_HELLO_SENDING,
  IRIS_HANDSHAKE_STATE_HELLO_SENDED,
  IRIS_HANDSHAKE_STATE_HELLO_RECEIVING,
  IRIS_HANDSHAKE_STATE_HELLO_RECEIVED,
  IRIS_HANDSHAKE_STATE_FINISHED_SENDING,
  IRIS_HANDSHAKE_STATE_FINISHED_SENDED,
  IRIS_HANDSHAKE_STATE_FINISHED_RECEIVING,
  IRIS_HANDSHAKE_STATE_FINISHED_RECEIVED,
  IRIS_HANDSHAKE_STATE_ESTABLISHED,
} iris_handshake_state_t;

typedef struct {
  iris_handshake_state_t state;
  iris_poll_context_t poll_ctx;
  uint8_t hello_resp[HELLO_RESPONSE_SIZE];
} iris_handshake_ctx_t;

static iris_handshake_ctx_t iris_handshake_ctx;

static void iris_handshake_ctx_init(void) {
  memzero(&iris_handshake_ctx, sizeof(iris_handshake_ctx_t));
}
#define iris_handshake_ctx_reset() iris_handshake_ctx_init()

/**
 * @brief Poll handshake reply
 *
 * @param ctx Poll context
 * @param reply Reply buffer
 * @return IRIS_ERROR_OK on success, negative on error, positive on pending
 * */
static int iris_handshake_poll_reply(iris_poll_context_t* ctx, iris_reply_t* reply) {
  // poll response
  iris_async_response_poll(ctx);
  if (ctx->state == IRIS_POLL_STATE_PENDDING) {
    return IRIS_HANDSHAKE_PENDING;
  } else if (ctx->state == IRIS_POLL_STATE_TIMEOUT) {
    return IRIS_ERROR_TIMEOUT;
  } else if (ctx->state != IRIS_POLL_STATE_DONE) {
    return IRIS_ERROR_INVALID_RESPONSE;
  }

  // poll done
  if (!iris_response_replay_is_ok(&ctx->resp)) {
    return IRIS_ERROR_INVALID_RESPONSE;
  }
  *reply = ctx->resp.reply;

  return IRIS_ERROR_OK;
}


int iris_sec_channel_handshake(const uint8_t* secret, size_t secret_size) {
  // enough work buf
  uint8_t work_buf[256] = {0};
  size_t work_buf_size = sizeof(work_buf);

  if (iris_handshake_ctx.state == IRIS_HANDSHAKE_STATE_NONE) {
    // change state to hello sending
    iris_handshake_ctx.state = IRIS_HANDSHAKE_STATE_HELLO_SENDING;

    // step1. Send `Handshake.Hello` message
    // use pre shared secret key agreement
    // Message: <Handshake.Hello[1]>|PVER[1]
    // Response: <Handshake.Hello[1]>|PVER[1]|RND[32]|SSEQ[4]
    const uint8_t HANDSHAKE_HELLO[] = {
        SCTR_HELLO,                   // Handshake hello message
        PVER_FLAG_PRE_SHARED_SECRET,  // security protocol
    };
    iris_request_t req = {
      .id = IRIS_MSG_ID_HANDSHAKE_HELLO,
      .payload = HANDSHAKE_HELLO,
      .payload_size = sizeof(HANDSHAKE_HELLO),
    };
    int ret = iris_send_request(&req);
    if (ret < 0) {
      return ret;
    }

    // change state to hello sended
    iris_handshake_ctx.state = IRIS_HANDSHAKE_STATE_HELLO_SENDED;

    // setup poll context for hello response
    iris_poll_context_init(&iris_handshake_ctx.poll_ctx, 2000, work_buf, work_buf_size);
    iris_handshake_ctx.poll_ctx.operation = req.id;
    // change state to hello receiving
    iris_handshake_ctx.state = IRIS_HANDSHAKE_STATE_HELLO_RECEIVING;
    // yield cpu
    return IRIS_HANDSHAKE_PENDING;
  } else if (iris_handshake_ctx.state == IRIS_HANDSHAKE_STATE_HELLO_RECEIVING) {
    // poll hello response
    iris_reply_t reply = {0};
    int ret = iris_handshake_poll_reply(&iris_handshake_ctx.poll_ctx, &reply);

    // pending or error
    if (ret != IRIS_ERROR_OK) {
      return ret;
    }

    // change state to hello received
    iris_handshake_ctx.state = IRIS_HANDSHAKE_STATE_HELLO_RECEIVED;

    // step2. Process Handshake.Hello response
    size_t hello_resp_size = reply.size;

    if (hello_resp_size != HELLO_RESPONSE_SIZE) {
      return IRIS_ERROR_SEC_FAILED;
    }

    uint8_t hello_resp[HELLO_RESPONSE_SIZE] = {0};
    // cache response, free work_buf
    memcpy(hello_resp, reply.data, hello_resp_size);

    if (hello_resp[0] != SCTR_HELLO ||
        hello_resp[1] != PVER_FLAG_PRE_SHARED_SECRET) {
      return IRIS_ERROR_SEC_FAILED;
    }

    // cache response
    memcpy(iris_handshake_ctx.hello_resp, hello_resp, HELLO_RESPONSE_SIZE);

    // Payload: RND[32]|SSEQ[4]
    uint8_t* payload = &hello_resp[2];
    size_t payload_size = SEC_RND_SIZE + SEC_SEQ_SIZE;
    uint8_t* rnd = payload;
    uint8_t* sseq = payload + SEC_RND_SIZE;

    // Master key derivation: <Encryption Key[16]>|<Decryption
    // Key[16]>|<Encryption Nonce[4]>|<Decryption Nonce[4]>
    uint8_t keys[SEC_KEYS_SIZE] = {0};
    tls_prf_sha256(secret, secret_size, (const uint8_t*)"Platform Binding", 16,
                  rnd, 32, keys, sizeof(keys));
    aes_encrypt_key128(&keys[0], &iris_sec_ctx.enc_ctx);
    aes_encrypt_key128(&keys[16], &iris_sec_ctx.dec_ctx);
    memcpy(iris_sec_ctx.enc_nonce.nonce, &keys[32], SEC_NONCE_SIZE);
    memcpy(iris_sec_ctx.dec_nonce.nonce, &keys[36], SEC_NONCE_SIZE);
    memzero(keys, sizeof(keys));

    // step3. Prepare Handshake Finished message
    // Message: <Handshake.Finished[1]>|SSEQ[4]|<ciphertext[44]>
    uint8_t handshake_finished[HANDSHAKE_FINISHED_SIZE] = {0};
    // <Handshake.Finished> 1 byte
    handshake_finished[0] = SCTR_FINISHED;
    // SSEQ 4 bytes
    memcpy(&handshake_finished[1], sseq, SEC_SEQ_SIZE);

    // Associated: <Handshake.Finished[1]>|SSEQ[4]|PVER[1]|<Payload length[2]>|
    uint8_t associated[8];
    // <Handshake.Finished> 1 byte
    associated[0] = SCTR_FINISHED;
    // SSEQ 4 bytes
    memcpy(&associated[1], sseq, SEC_SEQ_SIZE);
    // PVER
    associated[5] = PVER_FLAG_PRE_SHARED_SECRET;
    // Payload length 2 bytes, big endian
    associated[6] = 0;
    associated[7] = SEC_RND_SIZE + SEC_SEQ_SIZE;

    uint8_t* ciphertext = &handshake_finished[SEC_SCTR_SIZE + SEC_SEQ_SIZE];

    // nonce N: <Encrypt Nonce>|SSEQ
    encryption_nonce_t enc_nonce;
    memcpy(enc_nonce.nonce, iris_sec_ctx.enc_nonce.nonce, SEC_NONCE_SIZE);
    memcpy(enc_nonce.seq, sseq, SEC_SEQ_SIZE);

    // encrypted payload with mac
    int aes_ret =
        aes_ccm_encrypt(&iris_sec_ctx.enc_ctx, enc_nonce.encryption_nonce,
                        SEC_ENC_NONCE_SIZE, associated, sizeof(associated),
                        payload, payload_size, SEC_MAC_SIZE, ciphertext);
    if (aes_ret != EXIT_SUCCESS) {
      return IRIS_ERROR_SEC_FAILED;
    }

    // change state to finished sending
    iris_handshake_ctx.state = IRIS_HANDSHAKE_STATE_FINISHED_SENDING;

    // step4. Send `Handshake.Finished` message
    // Response: <Handshake.Finished[1]>|MSEQ[4]|<ciphertext[44]>
    iris_request_t req = {
      .id = IRIS_MSG_ID_HANDSHAKE_FINISHED,
      .payload = handshake_finished,
      .payload_size = HANDSHAKE_FINISHED_SIZE,
    };
    ret = iris_send_request(&req);
    if (ret < 0) {
      return ret;
    }

    // change state to finished receiving
    iris_handshake_ctx.state = IRIS_HANDSHAKE_STATE_FINISHED_SENDED;
    // setup poll context for finished response
    iris_poll_context_init(&iris_handshake_ctx.poll_ctx, 2000, work_buf, work_buf_size);
    iris_handshake_ctx.poll_ctx.operation = req.id;
    // change state to finished receiving
    iris_handshake_ctx.state = IRIS_HANDSHAKE_STATE_FINISHED_RECEIVING;
    // yield cpu
    return IRIS_HANDSHAKE_PENDING;
  } else if (iris_handshake_ctx.state == IRIS_HANDSHAKE_STATE_FINISHED_RECEIVING) {
    // poll finished response
    iris_reply_t reply = {0};
    int ret = iris_handshake_poll_reply(&iris_handshake_ctx.poll_ctx, &reply);

    // pending or error
    if (ret != IRIS_ERROR_OK) {
      return ret;
    }

    // change state to finished received
    iris_handshake_ctx.state = IRIS_HANDSHAKE_STATE_FINISHED_RECEIVED;
    // step5. Process Handshake.Finished response

    size_t finished_resp_len = 0;
    finished_resp_len = reply.size;
    if (finished_resp_len != HANDSHAKE_FINISHED_SIZE) {
      return IRIS_ERROR_SEC_FAILED;
    }
    uint8_t finished_resp[HANDSHAKE_FINISHED_SIZE] = {0};
    // cache response, free work_buf
    memcpy(finished_resp, reply.data, finished_resp_len);
    if (finished_resp[0] != SCTR_FINISHED) {
      return IRIS_ERROR_SEC_FAILED;
    }

    // step5. verify payload
    uint8_t* mseq = &finished_resp[1];
    uint8_t* ciphertext = &finished_resp[SEC_SCTR_SIZE + SEC_SEQ_SIZE];
    uint8_t associated[8];
    // Associated: <Handshake.Finished[1]>|MSEQ[4]|PVER[1]|<Payload length[2]>|
    // <Handshake.Finished> 1 byte
    associated[0] = SCTR_FINISHED;
    // MSEQ 4 bytes
    memcpy(&associated[1], mseq, SEC_SEQ_SIZE);
    // PVER
    associated[5] = PVER_FLAG_PRE_SHARED_SECRET;
    // Payload length 2 bytes, big endian
    associated[6] = 0;
    associated[7] = SEC_RND_SIZE + SEC_SEQ_SIZE;

    // nonce N: <Decrypt Nonce>|MSEQ
    encryption_nonce_t dec_nonce;
    memcpy(dec_nonce.nonce, iris_sec_ctx.dec_nonce.nonce, SEC_NONCE_SIZE);
    memcpy(dec_nonce.seq, mseq, SEC_SEQ_SIZE);
    uint8_t finished_payload[SEC_RND_SIZE + SEC_SEQ_SIZE];

    int aes_ret = aes_ccm_decrypt(&iris_sec_ctx.dec_ctx, dec_nonce.encryption_nonce,
                              SEC_ENC_NONCE_SIZE, associated, sizeof(associated),
                              ciphertext, CIPHERTEXT_SIZE, SEC_MAC_SIZE,
                              finished_payload);
    if (aes_ret != EXIT_SUCCESS) {
      return IRIS_ERROR_SEC_FAILED;
    }
    uint8_t* payload = &iris_handshake_ctx.hello_resp[2];
    uint8_t* rnd = payload;
    uint8_t* sseq = &payload[SEC_RND_SIZE];

    if (memcmp(rnd, finished_payload, SEC_RND_SIZE) != 0) {
      return IRIS_ERROR_SEC_FAILED;
    }
    if (memcmp(mseq, &finished_payload[SEC_RND_SIZE], SEC_SEQ_SIZE) != 0) {
      return IRIS_ERROR_SEC_FAILED;
    }

    // cache MSEQ SSEQ
    memcpy(iris_sec_ctx.enc_nonce.seq, mseq, SEC_SEQ_SIZE);
    memcpy(iris_sec_ctx.dec_nonce.seq, sseq, SEC_SEQ_SIZE);
    // YEAH! connected!
    iris_sec_ctx.sec_chan_established = true;

    // change state to established
    iris_handshake_ctx.state = IRIS_HANDSHAKE_STATE_ESTABLISHED;
    return IRIS_ERROR_OK;
  } else {
    memset(&iris_handshake_ctx, 0, sizeof(iris_handshake_ctx));
    return IRIS_ERROR_SEC_FAILED;
  }
}

int iris_sec_channel_decrypt(const uint8_t* record, size_t record_size, uint8_t* msg, size_t msg_buf_len) {
  if (!iris_sec_ctx.sec_chan_established) {
    LOG_DEBUG(MODULE, "sec channel not established");
    return IRIS_ERROR_SEC_FAILED;
  }

  // increment nonce seq
  increment_seq(iris_sec_ctx.dec_nonce.seq);

  if (record_size < SEC_OVERHEAD_SIZE) {
    LOG_DEBUG(MODULE, "ciphertext size too small");
    return IRIS_ERROR_SEC_FAILED;
  }

  // Response: <Record Exchange[1]>|SSEQ|ciphertext
  // verify format
  if (record[0] != SCTR_PROTECTED) {
    LOG_DEBUG(MODULE, "unprotected ciphertext received");
    return IRIS_ERROR_SEC_FAILED;
  }

  if (memcmp(&record[1], &iris_sec_ctx.dec_nonce.seq, SEC_NONCE_SIZE) != 0) {
    LOG_DEBUG(MODULE, "nonce not match");
    return IRIS_ERROR_SEC_FAILED;
  }
  const uint8_t *ciphertext = &record[SEC_SCTR_SIZE + SEC_SEQ_SIZE];
  size_t ciphertext_size = record_size - SEC_SCTR_SIZE - SEC_SEQ_SIZE;

  size_t payload_size = record_size - SEC_OVERHEAD_SIZE;
  if (msg_buf_len < payload_size) {
    LOG_DEBUG(MODULE, "msg buf size too small");
    return IRIS_ERROR_MEMORY_TOO_SMALL;
  }

  // decrypt payload
  // Associated: <Record Exchange[1]>|SSEQ[4]|PVER[1]|<Payload length[2]>|
  uint8_t associated[8];
  // <Record Exchange> 1 byte
  associated[0] = SCTR_PROTECTED;
  // SSEQ 4 bytes
  memcpy(&associated[1], &iris_sec_ctx.dec_nonce.seq, SEC_SEQ_SIZE);
  // PVER
  associated[5] = PVER_FLAG_PRE_SHARED_SECRET;
  // Payload length 2 bytes, big endian
  associated[6] = payload_size >> 8;
  associated[7] = payload_size & 0xFF;
  // LOG_HEXDUMP_DEBUG(MODULE, "associated", associated, sizeof(associated));
  int ret = aes_ccm_decrypt(
    &iris_sec_ctx.dec_ctx,
    iris_sec_ctx.dec_nonce.encryption_nonce, SEC_ENC_NONCE_SIZE,
    associated, sizeof(associated),
    ciphertext, ciphertext_size, SEC_MAC_SIZE, msg);
  if (ret != EXIT_SUCCESS) {
    return IRIS_ERROR_SEC_FAILED;
  }
  // LOG_HEXDUMP_DEBUG(MODULE, "msg", msg, payload_size);
  return payload_size;
}

int iris_sec_channel_encrypt(const uint8_t* msg, size_t msg_size, uint8_t* record, size_t record_buf_len) {
  if (!iris_sec_ctx.sec_chan_established) {
    LOG_DEBUG(MODULE, "sec channel not established");
    return IRIS_ERROR_SEC_FAILED;
  }

  increment_seq(iris_sec_ctx.enc_nonce.seq);

  // <Record Exchange[1]>|MSEQ[4]|ciphertext
  uint8_t *p = record;
  *p++ = SCTR_PROTECTED;
  memcpy(p, &iris_sec_ctx.enc_nonce.seq, SEC_SEQ_SIZE);
  p += SEC_SEQ_SIZE;

  // Associated: <Record Exchange[1]>|MSEQ[4]|PVER[1]|<Payload length[2]>|
  uint8_t associated[8];
  // <Record Exchange> 1 byte
  associated[0] = SCTR_PROTECTED;
  // MSEQ 4 bytes
  memcpy(&associated[1], &iris_sec_ctx.enc_nonce.seq, SEC_SEQ_SIZE);
  // PVER
  associated[5] = PVER_FLAG_PRE_SHARED_SECRET;
  // Payload length 2 bytes, big endian
  associated[6] = msg_size >> 8;
  associated[7] = msg_size & 0xFF;

  aes_ccm_encrypt(
    &iris_sec_ctx.enc_ctx,
    iris_sec_ctx.enc_nonce.encryption_nonce, SEC_ENC_NONCE_SIZE,
    associated, sizeof(associated),
    msg, msg_size,
    SEC_MAC_SIZE, p);

  return SEC_OVERHEAD_SIZE + msg_size;
}

#if IRIS_TESTING
#define MODULE_MASTER "iris (mcu)"
#define MODULE_SLAVE  "iris (module)"

#define SECRET_BYTE 0x11
#define RND_BYTE 0xAA
#define SSEQ_BYTE 0xBB
#define MSEQ_BYTE 0xCC
/// for test
static int iris_usart_read_mock(iris_msg_id_t id, uint8_t* buf, size_t len) {
  /* packet: 0xFE, 0xFD, msg_id, msg_type, N:2 bytes, <N bytes data>, crc*/
  // <0xFE, 0xFD> have been consumed, crc have been checked

  uint8_t *p = buf;
  // `msg_id`
  *p++ = id;
  // `msg_type`
  *p++ = IRIS_MSG_TYPE_REPLY;
  // `N`
  uint8_t* n = p;
  // move over N, 2 bytes, calculate later
  p += 2;
  // data: <code: 1 byte><payload: N-1 bytes>
  // `code`
  *p++ = IRIS_RESPONSE_CODE_OK;
  // `payload`
  switch (id) {
    case IRIS_MSG_ID_HANDSHAKE_HELLO: {
      LOG_DEBUG(MODULE_SLAVE, "Handshake.Hello");
      // resp: <Handshake.Hello[1]>|PVER[1]|RND[32]|SSEQ[4]
      // `SCTR`
      *p++ = SCTR_HELLO;
      LOG_DEBUG(MODULE_SLAVE, "SCTR: %02x", SCTR_HELLO);
      // `PVER`
      *p++ = PVER_FLAG_PRE_SHARED_SECRET;
      LOG_DEBUG(MODULE_SLAVE, "PVER: %02x", PVER_FLAG_PRE_SHARED_SECRET);
      // `RND`, use 0xAA*32 for test
      memset(p, RND_BYTE, SEC_RND_SIZE);
      LOG_HEXDUMP_DEBUG(MODULE_SLAVE, "RND", p, SEC_RND_SIZE);
      p += SEC_RND_SIZE;
      // `SSEQ`, use 0xBB*4 for test
      memset(p, SSEQ_BYTE, SEC_SEQ_SIZE);
      LOG_HEXDUMP_DEBUG(MODULE_SLAVE, "SSEQ", p, SEC_SEQ_SIZE);
      p += SEC_SEQ_SIZE;
      break;
    }
    case IRIS_MSG_ID_HANDSHAKE_FINISHED: {
      LOG_DEBUG(MODULE_SLAVE, "Handshake.Finished");
      // Response: <Handshake.Finished[1]>|MSEQ[4]|<ciphertext[44]>
      // `SCTR`
      *p++ = SCTR_FINISHED;
      LOG_DEBUG(MODULE_SLAVE, "SCTR: %02x", SCTR_FINISHED);
      // `MSEQ`, use 0xCC*4 for test
      memset(p, MSEQ_BYTE, SEC_SEQ_SIZE);
      LOG_HEXDUMP_DEBUG(MODULE_SLAVE, "MSEQ", p, SEC_SEQ_SIZE);
      p += SEC_SEQ_SIZE;

      // Master key derivation: <Encryption Key[16]>|<Decryption
      // Key[16]>|<Encryption Nonce[4]>|<Decryption Nonce[4]>
      uint8_t keys[40] = {0};
      uint8_t rnd[32] = {0};
      uint8_t secret[32] = {0};
      memset(secret, SECRET_BYTE, 32);
      memset(rnd, RND_BYTE, SEC_RND_SIZE);
      LOG_DEBUG(MODULE_SLAVE, "keys = tls_prf_sha256(secret, label, seed)");
      LOG_HEXDUMP_DEBUG(MODULE_SLAVE, "secret", secret, sizeof(secret));
      LOG_HEXDUMP_DEBUG(MODULE_SLAVE, "label", (const uint8_t*)"Platform Binding", 16);
      LOG_HEXDUMP_DEBUG(MODULE_SLAVE, "seed", rnd, sizeof(rnd));
      uart_log_flush();
      tls_prf_sha256(
        secret, 32,
        (const uint8_t*)"Platform Binding", 16,
        rnd, 32,
        keys, sizeof(keys)
      );
      // master enc <==> slave dec
      // master dec <==> slave enc
      LOG_HEXDUMP_DEBUG(MODULE_SLAVE, "keys", keys, sizeof(keys));
      LOG_HEXDUMP_DEBUG(MODULE_SLAVE, "master enc key/slave dec key", keys, 16);
      LOG_HEXDUMP_DEBUG(MODULE_SLAVE, "master dec key/slave enc key", keys + 16, 16);
      LOG_HEXDUMP_DEBUG(MODULE_SLAVE, "master enc nonce/slave dec nonce", keys + 32, 4);
      LOG_HEXDUMP_DEBUG(MODULE_SLAVE, "master dec nonce/slave enc nonce", keys + 36, 4);
      uart_log_flush();

      // authentication
      uint8_t associated[8];
      // Associated: <Handshake.Finished[1]>|MSEQ[4]|PVER[1]|<Payload length[2]>|
      associated[0] = SCTR_FINISHED;
      // MSEQ 4 bytes
      memset(&associated[1], MSEQ_BYTE, SEC_SEQ_SIZE);
      // PVER
      associated[5] = PVER_FLAG_PRE_SHARED_SECRET;
      // Payload length 2 bytes, big endian
      associated[6] = 0x00;
      associated[7] = SEC_RND_SIZE + SEC_SEQ_SIZE;

      // nonce: <slave enc nonce[4]><MSEQ[4]>
      uint8_t nonce[8] = {0};
      memcpy(nonce, keys + 36, 4);
      memset(nonce + 4, MSEQ_BYTE, 4);
      // plain: <rnd[32]><mseq[4]>
      uint8_t plain[36] = {0};
      memcpy(plain, rnd, 32);
      memcpy(plain + 32, buf + 6, 4);
      // ciphertext = aes_ccm_encrypt(key, nonce, associated, plain)
      aes_encrypt_ctx aes;
      // slave enc key
      aes_encrypt_key128(keys+16, &aes);
      aes_ccm_encrypt(
        &aes,
        nonce, sizeof(nonce),
        associated, sizeof(associated),
        plain, sizeof(plain),
        SEC_MAC_SIZE,
        p
      );
      LOG_DEBUG(MODULE_SLAVE, "handshake finished");
      LOG_DEBUG(MODULE_SLAVE, "ciphertext = aes_ccm_encrypt(key, nonce, associated, plain)");
      LOG_HEXDUMP_DEBUG(MODULE_SLAVE, "key", keys + 16, 16);
      LOG_HEXDUMP_DEBUG(MODULE_SLAVE, "nonce", nonce, sizeof(nonce));
      LOG_HEXDUMP_DEBUG(MODULE_SLAVE, "associated", associated, sizeof(associated));
      LOG_HEXDUMP_DEBUG(MODULE_SLAVE, "plain", plain, sizeof(plain));
      LOG_HEXDUMP_DEBUG(MODULE_SLAVE, "ciphertext", p, 44);
      uart_log_flush();
      p += 44;
      break;
    }
    default:
      return 0;
  }
  size_t resp_size = p - buf;
  // update N, remove <msg_id, msg_type, N:2 bytes>
  PUT_UINT16_LE(resp_size - 4, n, 0);
  LOG_HEXDUMP_DEBUG(MODULE_SLAVE, "resp, not include <crc>", buf, resp_size);
  return resp_size;
}

static int iris_transmit_mock(const iris_request_t* req, iris_reply_t* reply, uint8_t* work_buf, size_t work_buf_size) {
  LOG_DEBUG("sending", "msg id: %02x", req->id);
  LOG_HEXDUMP_DEBUG("sending", "payload", req->payload, req->payload_size);
  uint8_t msg[256] = {0};
  // `msg_id` 1 byte
  msg[0] = req->id;
  // `msg_type` 1 byte
  msg[1] = IRIS_MSG_TYPE_REQUEST;
  // `N:2 bytes` 2 bytes
  PUT_UINT16_LE(req->payload_size, msg, 2);
  // `payload`
  memcpy(msg + 4, req->payload, req->payload_size);
  LOG_HEXDUMP_DEBUG("sending", "request, not include <crc>", msg, 4 + req->payload_size);
  uart_log_flush();

  // 1. setup poll context
  iris_poll_context_t ctx;
  // 500ms does enough??
  iris_poll_context_init(&ctx, 500, work_buf, work_buf_size);
  ctx.operation = req->id;

  ctx.resp_raw_size = iris_usart_read_mock(req->id,ctx.resp_buf, ctx.resp_buf_len);
  if (ctx.resp_raw_size <= 0) {
      return IRIS_ERROR_INVALID_RESPONSE;
  }
  // parse response
  if (iris_resp_parser(ctx.resp_buf, ctx.resp_raw_size, &ctx.resp) == IRIS_ERROR_OK) {
      ctx.state = IRIS_POLL_STATE_DONE;
  } else {
      ctx.state = IRIS_POLL_STATE_ERROR;
  }
  if (ctx.state == IRIS_POLL_STATE_TIMEOUT) {
    return IRIS_ERROR_TIMEOUT;
  }
  if (ctx.state != IRIS_POLL_STATE_DONE) {
    return IRIS_ERROR_INVALID_RESPONSE;
  }

  if (!iris_response_replay_is_ok(&ctx.resp)) {
    return IRIS_ERROR_INVALID_RESPONSE;
  }
  *reply = ctx.resp.reply;

  return IRIS_ERROR_OK;
}

int iris_sec_channel_handshake_test(void) {
  // enough work buf
  uint8_t work_buf[256] = {0};
  size_t work_buf_size = sizeof(work_buf);

  // step1. Send `Handshake.Hello` message
  // use pre shared secret key agreement
  // Message: <Handshake.Hello[1]>|PVER[1]
  // Response: <Handshake.Hello[1]>|PVER[1]|RND[32]|SSEQ[4]
  const uint8_t HANDSHAKE_HELLO[] = {
      SCTR_HELLO,                   // Handshake hello message
      PVER_FLAG_PRE_SHARED_SECRET,  // security protocol
  };
  LOG_DEBUG(MODULE_MASTER, "Handshake.Hello");
  LOG_DEBUG(MODULE_MASTER, "SCTR: %02x", SCTR_HELLO);
  LOG_DEBUG(MODULE_MASTER, "PVER: %02x", PVER_FLAG_PRE_SHARED_SECRET);
  uart_log_flush();
#define HELLO_RESPONSE_SIZE \
  (SEC_SCTR_SIZE + SEC_PVER_SIZE + SEC_RND_SIZE + SEC_SEQ_SIZE)
  iris_request_t req = {
    .id = IRIS_MSG_ID_HANDSHAKE_HELLO,
    .payload = HANDSHAKE_HELLO,
    .payload_size = sizeof(HANDSHAKE_HELLO),
  };
  iris_reply_t reply;
  int ret = iris_transmit_mock(&req, &reply, work_buf, work_buf_size);
  if (ret < 0) {
    return ret;
  }

  size_t hello_resp_size = reply.size;

  // step2. Process Handshake.Hello response
  if (hello_resp_size != HELLO_RESPONSE_SIZE) {
    return IRIS_ERROR_SEC_FAILED;
  }

  uint8_t hello_resp[HELLO_RESPONSE_SIZE] = {0};
  // cache response, free work_buf
  memcpy(hello_resp, reply.data, hello_resp_size);

  if (hello_resp[0] != SCTR_HELLO ||
      hello_resp[1] != PVER_FLAG_PRE_SHARED_SECRET) {
    return IRIS_ERROR_SEC_FAILED;
  }

  // Payload: RND[32]|SSEQ[4]
  uint8_t* payload = &hello_resp[2];
  size_t payload_size = SEC_RND_SIZE + SEC_SEQ_SIZE;
  uint8_t* rnd = payload;
  uint8_t* sseq = payload + SEC_RND_SIZE;

  LOG_DEBUG(MODULE_MASTER, "Handshake.Hello response");
  LOG_DEBUG(MODULE_MASTER, "SCTR: %02x", SCTR_HELLO);
  LOG_DEBUG(MODULE_MASTER, "PVER: %02x", PVER_FLAG_PRE_SHARED_SECRET);
  LOG_HEXDUMP_DEBUG(MODULE_MASTER, "RND", rnd, SEC_RND_SIZE);
  LOG_HEXDUMP_DEBUG(MODULE_MASTER, "SSEQ", sseq, SEC_SEQ_SIZE);
  uart_log_flush();

  // Master key derivation: <Encryption Key[16]>|<Decryption
  // Key[16]>|<Encryption Nonce[4]>|<Decryption Nonce[4]>
  uint8_t keys[SEC_KEYS_SIZE] = {0};
  uint8_t secret[32] = {0};
  memset(secret, SECRET_BYTE, 32);
  LOG_DEBUG(MODULE_MASTER, "keys = tls_prf_sha256(secret, label, seed)");
  LOG_HEXDUMP_DEBUG(MODULE_MASTER, "secret", secret, sizeof(secret));
  LOG_HEXDUMP_DEBUG(MODULE_MASTER, "label", (const uint8_t*)"Platform Binding", 16);
  LOG_HEXDUMP_DEBUG(MODULE_MASTER, "seed", rnd, SEC_RND_SIZE);
  uart_log_flush();
  tls_prf_sha256(secret, 32, (const uint8_t*)"Platform Binding", 16,
                 rnd, 32, keys, sizeof(keys));
  aes_encrypt_key128(&keys[0], &iris_sec_ctx.enc_ctx);
  aes_encrypt_key128(&keys[16], &iris_sec_ctx.dec_ctx);
  memcpy(iris_sec_ctx.enc_nonce.nonce, &keys[32], SEC_NONCE_SIZE);
  memcpy(iris_sec_ctx.dec_nonce.nonce, &keys[36], SEC_NONCE_SIZE);
  LOG_HEXDUMP_DEBUG(MODULE_MASTER, "keys", keys, sizeof(keys));
  LOG_HEXDUMP_DEBUG(MODULE_MASTER, "master enc key/slave dec key", keys, 16);
  LOG_HEXDUMP_DEBUG(MODULE_MASTER, "master dec key/slave enc key", keys + 16, 16);
  LOG_HEXDUMP_DEBUG(MODULE_MASTER, "master enc nonce/slave dec nonce", keys + 32, 4);
  LOG_HEXDUMP_DEBUG(MODULE_MASTER, "master dec nonce/slave enc nonce", keys + 36, 4);
  uart_log_flush();
  // memzero(keys, sizeof(keys));

  // step3. Prepare Handshake Finished message
#define CIPHERTEXT_SIZE ((SEC_RND_SIZE + SEC_SEQ_SIZE) + SEC_MAC_SIZE)
#define HANDSHAKE_FINISHED_SIZE (SEC_SCTR_SIZE + SEC_SEQ_SIZE + CIPHERTEXT_SIZE)
  // Message: <Handshake.Finished[1]>|SSEQ[4]|<ciphertext[44]>
  uint8_t handshake_finished[HANDSHAKE_FINISHED_SIZE] = {0};
  // <Handshake.Finished> 1 byte
  handshake_finished[0] = SCTR_FINISHED;
  // SSEQ 4 bytes
  memcpy(&handshake_finished[1], sseq, SEC_SEQ_SIZE);
  LOG_DEBUG(MODULE_MASTER, "Handshake.Finished");
  LOG_DEBUG(MODULE_MASTER, "SCTR: %02x", SCTR_FINISHED);
  LOG_HEXDUMP_DEBUG(MODULE_MASTER, "SSEQ", sseq, SEC_SEQ_SIZE);

  // Associated: <Handshake.Finished[1]>|SSEQ[4]|PVER[1]|<Payload length[2]>|
  uint8_t associated[8];
  // <Handshake.Finished> 1 byte
  associated[0] = SCTR_FINISHED;
  // SSEQ 4 bytes
  memcpy(&associated[1], sseq, SEC_SEQ_SIZE);
  // PVER
  associated[5] = PVER_FLAG_PRE_SHARED_SECRET;
  // Payload length 2 bytes, big endian
  associated[6] = 0;
  associated[7] = SEC_RND_SIZE + SEC_SEQ_SIZE;

  uint8_t* ciphertext = &handshake_finished[SEC_SCTR_SIZE + SEC_SEQ_SIZE];

  // nonce N: <Encrypt Nonce>|SSEQ
  encryption_nonce_t enc_nonce;
  memcpy(enc_nonce.nonce, iris_sec_ctx.enc_nonce.nonce, SEC_NONCE_SIZE);
  memcpy(enc_nonce.seq, sseq, SEC_SEQ_SIZE);

  // encrypted payload with mac
  int aes_ret =
      aes_ccm_encrypt(&iris_sec_ctx.enc_ctx, enc_nonce.encryption_nonce,
                      SEC_ENC_NONCE_SIZE, associated, sizeof(associated),
                      payload, payload_size, SEC_MAC_SIZE, ciphertext);
  if (aes_ret != EXIT_SUCCESS) {
    return IRIS_ERROR_SEC_FAILED;
  }

  LOG_DEBUG(MODULE_MASTER, "Handshake.Finished generating");
  LOG_DEBUG(MODULE_MASTER, "ciphertext = aes_ccm_encrypt(key, nonce, associated, plain)");
  LOG_HEXDUMP_DEBUG(MODULE_MASTER, "key", keys, 16);
  LOG_HEXDUMP_DEBUG(MODULE_MASTER, "nonce", enc_nonce.encryption_nonce, 8);
  LOG_HEXDUMP_DEBUG(MODULE_MASTER, "associated", associated, sizeof(associated));
  LOG_HEXDUMP_DEBUG(MODULE_MASTER, "plain", payload, payload_size);
  LOG_HEXDUMP_DEBUG(MODULE_MASTER, "ciphertext", ciphertext, 44);
  uart_log_flush();

  // step4. Send `Handshake.Finished` message
  // Response: <Handshake.Finished[1]>|MSEQ[4]|<ciphertext[44]>
  req.id = IRIS_MSG_ID_HANDSHAKE_FINISHED,
  req.payload = handshake_finished,
  req.payload_size = HANDSHAKE_FINISHED_SIZE,
  ret = iris_transmit_mock(&req, &reply, work_buf, work_buf_size);
  if (ret < 0) {
    return ret;
  }

  size_t finished_resp_len = 0;
  finished_resp_len = reply.size;
  if (finished_resp_len != HANDSHAKE_FINISHED_SIZE) {
    return IRIS_ERROR_SEC_FAILED;
  }
  uint8_t finished_resp[HANDSHAKE_FINISHED_SIZE] = {0};
  // cache response, free work_buf
  memcpy(finished_resp, reply.data, finished_resp_len);
  if (finished_resp[0] != SCTR_FINISHED) {
    return IRIS_ERROR_SEC_FAILED;
  }

  // step5. verify payload
  uint8_t* mseq = &finished_resp[1];
  ciphertext = &finished_resp[SEC_SCTR_SIZE + SEC_SEQ_SIZE];

  LOG_DEBUG(MODULE_MASTER, "Handshake.Finished response");
  LOG_DEBUG(MODULE_MASTER, "SCTR: %02x", SCTR_FINISHED);
  LOG_HEXDUMP_DEBUG(MODULE_MASTER, "MSEQ", mseq, SEC_SEQ_SIZE);
  LOG_HEXDUMP_DEBUG(MODULE_MASTER, "ciphertext", ciphertext, 44);
  uart_log_flush();

  // Associated: <Handshake.Finished[1]>|MSEQ[4]|PVER[1]|<Payload length[2]>|
  // <Handshake.Finished> 1 byte
  associated[0] = SCTR_FINISHED;
  // MSEQ 4 bytes
  memcpy(&associated[1], mseq, SEC_SEQ_SIZE);
  // PVER
  associated[5] = PVER_FLAG_PRE_SHARED_SECRET;
  // Payload length 2 bytes, big endian
  associated[6] = 0;
  associated[7] = SEC_RND_SIZE + SEC_SEQ_SIZE;

  // nonce N: <Decrypt Nonce>|MSEQ
  encryption_nonce_t dec_nonce;
  memcpy(dec_nonce.nonce, iris_sec_ctx.dec_nonce.nonce, SEC_NONCE_SIZE);
  memcpy(dec_nonce.seq, mseq, SEC_SEQ_SIZE);
  uint8_t finished_payload[SEC_RND_SIZE + SEC_SEQ_SIZE];

  aes_ret = aes_ccm_decrypt(&iris_sec_ctx.dec_ctx, dec_nonce.encryption_nonce,
                            SEC_ENC_NONCE_SIZE, associated, sizeof(associated),
                            ciphertext, CIPHERTEXT_SIZE, SEC_MAC_SIZE,
                            finished_payload);
  if (aes_ret != EXIT_SUCCESS) {
    return IRIS_ERROR_SEC_FAILED;
  }

  LOG_DEBUG(MODULE_MASTER, "Handshake.Finished verifing");
  LOG_DEBUG(MODULE_MASTER, "plain = aes_ccm_decrypt(key, nonce, associated, plain)");
  LOG_HEXDUMP_DEBUG(MODULE_MASTER, "key", keys, 16);
  LOG_HEXDUMP_DEBUG(MODULE_MASTER, "nonce", dec_nonce.encryption_nonce, 8);
  LOG_HEXDUMP_DEBUG(MODULE_MASTER, "associated", associated, sizeof(associated));
  LOG_HEXDUMP_DEBUG(MODULE_MASTER, "ciphertext", ciphertext, 44);
  LOG_HEXDUMP_DEBUG(MODULE_MASTER, "plain", finished_payload, sizeof(finished_payload));
  uart_log_flush();
  if (memcmp(rnd, finished_payload, SEC_RND_SIZE) != 0) {
    return IRIS_ERROR_SEC_FAILED;
  }
  if (memcmp(mseq, &finished_payload[SEC_RND_SIZE], SEC_SEQ_SIZE) != 0) {
    return IRIS_ERROR_SEC_FAILED;
  }

  LOG_HEXDUMP_DEBUG(MODULE_MASTER, "verified rnd", finished_payload, SEC_RND_SIZE);
  LOG_HEXDUMP_DEBUG(MODULE_MASTER, "verified mseq", &finished_payload[SEC_RND_SIZE], SEC_SEQ_SIZE);
  LOG_DEBUG(MODULE_MASTER, "Handshake.Finished verified");
  uart_log_flush();

  // cache MSEQ SSEQ
  memcpy(iris_sec_ctx.enc_nonce.seq, mseq, SEC_SEQ_SIZE);
  memcpy(iris_sec_ctx.dec_nonce.seq, sseq, SEC_SEQ_SIZE);
  // YEAH! connected!
  iris_sec_ctx.sec_chan_established = true;
  LOG_DEBUG(MODULE_MASTER, "Handshake completed");
  return IRIS_ERROR_OK;
}
#endif
