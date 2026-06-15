#include "thd89.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "aes/aesccm.h"
#include "hmac.h"
#include "memzero.h"
#include "sec_trans.h"
#include "sha2.h"

#ifndef MIN
#define MIN(a, b) ({ a < b ? a : b; })
#endif
#ifndef UNUSED
#define UNUSED(x) (void)(x)
#endif

#define THD89_OP_CHECK(op)       \
  do {                           \
    thd89_result_t _ret = (op);  \
    if (_ret != THD89_SUCCESS) { \
      return _ret;               \
    }                            \
  } while (0)

#define THD89_FAILED(code) \
  do {                     \
    return code;           \
  } while (0)

#define THD89_MAX_RETRY_COUNT 16

#define THD89_MAX_APDU_SIZE 1557
// data size per packet
#define THD89_MAX_DATA_SIZE 256
// PCTR[1]|<data>
#define THD89_MAX_PACKET_SIZE (THD89_MAX_DATA_SIZE + 1)
// FCTR[1]|<LEN[2]>|<packet>|FCS[2]
#define THD89_MAX_FRAME_SIZE (THD89_MAX_PACKET_SIZE + 5)

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

// Packet control bytes
enum {
  PCTR_PRESENTATION_LAYER = 0x08,  // Presentation layer present.
  PCTR_CHAIN_NONE = 0x00,          // No chaining. Single frame.
  PCTR_CHAIN_FIRST = 0x01,         // First packet of a chain.
  PCTR_CHAIN_MIDDLE = 0x02,        // Intermediate packet(s) of a chain.
  PCTR_CHAIN_LAST = 0x04,          // Last packet of a chain.
  PCTR_CHAIN_MASK = 0x07,          // Mask of chain field.
};

// Frame control types
#define FCTR_TYPE_MASK 0x80
enum {
  FCTR_TYPE_DATA = 0x00,
  FCTR_TYPE_CONTROL = 0x80,
};

// Frame control sequences
#define SEQCTR_MASK 0x60
enum {
  SEQCTR_ACK = 0x00,
  SEQCTR_NAK = 0x20,
  SEQCTR_RESET = 0x40,
  SEQCTR_PING = 0x60,
};

#define FCTR_SET_FRNR(FCTR, FRNR) \
  do {                            \
    FCTR |= (FRNR & 0x03) << 2;   \
  } while (0)

#define FCTR_SET_ACKNR(FCTR, ACKNR) \
  do {                              \
    FCTR |= ACKNR & 0x03;           \
  } while (0)

#define ACK_FOR(N) (FCTR_TYPE_CONTROL | SEQCTR_ACK | (N & 0x03))
#define NAK_FOR(N) (FCTR_TYPE_CONTROL | SEQCTR_NAK | ((N & 0x03) << 3))

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
  uint8_t frame_number_sending;
  uint8_t frame_number_receiving;
} se_context_t;

static se_context_t thd89_ctx;

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
  memzero(&thd89_ctx, sizeof(se_context_t));
}
static uint16_t calc_crc_byte(uint16_t seed, uint8_t c) {
  uint16_t h1 = (seed ^ c) & 0xFF;
  uint16_t h2 = h1 & 0x0F;
  uint16_t h3 = (h2 << 4) ^ h1;
  uint16_t h4 = h3 >> 4;
  return (((((h3 << 1) ^ h4) << 4) ^ h2) << 3) ^ h4 ^ (seed >> 8);
}

static uint16_t calc_crc(uint8_t* data, size_t data_size) {
  uint16_t crc = 0;
  for (size_t i = 0; i < data_size; ++i) {
    crc = calc_crc_byte(crc, data[i]);
  }
  return crc;
}

static thd89_result_t thd89_write(const uint8_t* frame, size_t frame_size) {
  return sec_trans_write(frame, frame_size, 500);
}
static thd89_result_t thd89_read(uint8_t* frame, size_t frame_buf_size) {
  return sec_trans_read(frame, frame_buf_size, 500);
}

static thd89_result_t thd89_send_frame_control(uint8_t fctr) {
  uint8_t frame[THD89_MAX_FRAME_SIZE] = {0};
  frame[0] = fctr;
  // not have data
  frame[1] = 0;
  frame[2] = 0;
  uint16_t crc = calc_crc(frame, 3);
  frame[3] = crc >> 8;
  frame[4] = crc & 0xFF;
  return thd89_write(frame, 5);
}

#define thd89_send_ack(fctr) thd89_send_frame_control(fctr)
#define thd89_send_nak(fctr) thd89_send_frame_control(fctr)
#define thd89_send_ping(fctr) thd89_send_frame_control(fctr)
#define thd89_send_reset(fctr) thd89_send_frame_control(fctr)

static thd89_result_t thd89_read_frame_control(uint8_t* fctr) {
  // Frame: FCTR[1]|<LEN[2]>|<packet>|FCS[2]
  uint8_t frame[THD89_MAX_FRAME_SIZE];
  thd89_read(frame, THD89_MAX_FRAME_SIZE);
  size_t len = frame[1];
  len <<= 8;
  len |= frame[2];

  // only accept control frame
  if (len != 0) {
    return THD89_ERR_UNEXPECTED;
  }

  // verify CRC
  uint16_t crc = frame[5 - 2];
  crc <<= 8;
  crc |= frame[5 - 1];

  if (crc != calc_crc(frame, 3)) {
    return THD89_ERR_CRC;
  }

  *fctr = frame[0];
  if (((*fctr) & FCTR_TYPE_MASK) != FCTR_TYPE_CONTROL) {
    *fctr = 0;
    return THD89_ERR_UNEXPECTED;
  }

  return THD89_SUCCESS;
}

#define thd89_read_ack(fctr) thd89_read_frame_control(fctr)
#define thd89_read_nak(fctr) thd89_read_frame_control(fctr)
#define thd89_read_ping(fctr) thd89_read_frame_control(fctr)
#define thd89_read_reset(fctr) thd89_read_frame_control(fctr)

static thd89_result_t thd89_send_packet(uint8_t pctr, const uint8_t* packet,
                                        size_t packet_size) {
  thd89_result_t ret = THD89_SUCCESS;

  uint8_t frame[THD89_MAX_FRAME_SIZE];
  size_t frame_size = packet_size + 1 + 5;
  uint8_t fctr;
  fctr = FCTR_TYPE_DATA | SEQCTR_ACK;
  FCTR_SET_FRNR(fctr, thd89_ctx.frame_number_sending);

  // Frame: FCTR[1]|LEN[2]|<packet>|FCS[2]
  frame[0] = fctr;
  frame[1] = (packet_size + 1) >> 8;
  frame[2] = (packet_size + 1) & 0xff;
  frame[3] = pctr;
  memcpy(&frame[4], packet, packet_size);

  // CRC(frame[:-2])
  uint16_t crc = calc_crc(frame, frame_size - 2);
  frame[frame_size - 2] = crc >> 8;
  frame[frame_size - 1] = crc & 0xff;

  int retry = THD89_MAX_RETRY_COUNT;
  while (retry--) {
    ret = thd89_write(frame, frame_size);
    if (ret == THD89_ERR_TIMEOUT) {
      continue;
    }
    THD89_OP_CHECK(ret);

    // read ack
    ret = thd89_read_ack(&fctr);
    if (ret == THD89_ERR_TIMEOUT) {
      continue;
    }
    THD89_OP_CHECK(ret);

    // check ack
    if (fctr == NAK_FOR(thd89_ctx.frame_number_sending)) {
      continue;
    } else if (fctr == ACK_FOR(thd89_ctx.frame_number_sending)) {
      thd89_ctx.frame_number_sending++;
      break;
    } else {
      return THD89_ERR_UNEXPECTED;
    }
  }

  return ret;
}

static thd89_result_t thd89_receive_packet(uint8_t* pctr, uint8_t* packet,
                                           size_t packet_buf_size,
                                           size_t* packet_size) {
  thd89_result_t ret;

  uint8_t frame[THD89_MAX_FRAME_SIZE];
  size_t len = 0;
  uint8_t fctr;

  *packet_size = 0;
  int retry = THD89_MAX_RETRY_COUNT;
  while (retry--) {
    ret = thd89_read(frame, THD89_MAX_FRAME_SIZE);
    if (ret == THD89_ERR_TIMEOUT) {
      continue;
    }
    THD89_OP_CHECK(ret);

    // Frame: FCTR[1]|LEN[2]|<packet>|FCS[2]
    fctr = FCTR_TYPE_DATA | SEQCTR_ACK;
    FCTR_SET_FRNR(fctr, thd89_ctx.frame_number_receiving);
    if (fctr != frame[0]) {
      return THD89_ERR_UNEXPECTED;
    }

    len = frame[1];
    len <<= 8;
    len += frame[2];

    if (len > THD89_MAX_PACKET_SIZE) {
      return THD89_ERR_UNEXPECTED;
    }

    // add FCTR[1]|LEN[2] and FCS[2] size
    len += 5;
    uint16_t crc = frame[len - 2];
    crc <<= 8;
    crc += frame[len - 1];

    if (calc_crc(frame, len - 2) != crc) {
      ret = thd89_send_nak(NAK_FOR(thd89_ctx.frame_number_receiving));
      THD89_OP_CHECK(ret);
    }

    // remove FCTR[1]|LEN[2]|PCTR[1] and FCS[2] size
    len -= 6;

    ret = thd89_send_ack(ACK_FOR(thd89_ctx.frame_number_receiving));
    THD89_OP_CHECK(ret);
    thd89_ctx.frame_number_receiving++;
    if (packet_buf_size < len) {
      return THD89_ERR_BUFFER_SMALL;
    }
    *pctr = frame[3];
    memcpy(packet, &frame[4], len);
    packet_buf_size -= len;
    *packet_size += len;
    break;
  }

  return THD89_SUCCESS;
}

static thd89_result_t thd89_transmit(const uint8_t* req, size_t req_size,
                                     uint8_t* resp, size_t resp_buf_size,
                                     size_t* resp_size) {
  uint8_t pctr = 0;
  if (thd89_ctx.sec_chan_established) {
    pctr |= PCTR_PRESENTATION_LAYER;
  }

  uint8_t chain = PCTR_CHAIN_NONE;
  thd89_result_t ret;

  // send
  do {
    // PCTR[1]|<packet>
    size_t packet_size = MIN(THD89_MAX_DATA_SIZE, req_size);

    // need chain?
    if (req_size > THD89_MAX_DATA_SIZE) {
      if (chain == PCTR_CHAIN_NONE) {
        chain = PCTR_CHAIN_FIRST;
      } else {
        chain = PCTR_CHAIN_MIDDLE;
      }
    } else {
      // maybe last packet or single packet
      if (chain != PCTR_CHAIN_NONE) {
        chain = PCTR_CHAIN_LAST;
      }
    }

    ret = thd89_send_packet(pctr | chain, req, packet_size);
    THD89_OP_CHECK(ret);
    req_size -= packet_size;
    req += packet_size;
  } while (req_size);

  *resp_size = 0;
  // receive
  do {
    size_t packet_size = 0;
    ret = thd89_receive_packet(&pctr, resp, resp_buf_size, &packet_size);
    if (ret != THD89_SUCCESS) {
      *resp_size = 0;
      return ret;
    }
    resp += packet_size;
    resp_buf_size -= packet_size;
    *resp_size += packet_size;
    pctr &= PCTR_CHAIN_MASK;
  } while (pctr == PCTR_CHAIN_FIRST || pctr == PCTR_CHAIN_MIDDLE);

  return THD89_SUCCESS;
}

/// export functions

thd89_result_t thd89_init(void) {
  memzero(&thd89_ctx, sizeof(thd89_ctx));
  return THD89_SUCCESS;
}

bool thd89_is_secure_channel_established(void) {
  return thd89_ctx.sec_chan_established;
}

void thd89_deinit(void) { memzero(&thd89_ctx, sizeof(se_context_t)); }

thd89_result_t thd89_ping(void) {
  // send control frame
  uint8_t fctr;
  fctr = FCTR_TYPE_CONTROL;

  // ping
  fctr |= SEQCTR_PING;
  thd89_result_t ret = thd89_send_ping(fctr);
  if (ret != THD89_SUCCESS) {
    return ret;
  }

  // read back
  ret = thd89_read_ping(&fctr);
  if (ret != THD89_SUCCESS) {
    return ret;
  }
  if (fctr != (FCTR_TYPE_CONTROL | SEQCTR_PING)) {
    return THD89_ERR_UNEXPECTED;
  }

  return THD89_SUCCESS;
}

thd89_result_t thd89_reset(void) {
  thd89_result_t ret = thd89_send_reset(FCTR_TYPE_CONTROL | SEQCTR_RESET);
  if (ret) {
    return ret;
  }
  uint8_t fctr;
  thd89_read_reset(&fctr);
  if (fctr != (FCTR_TYPE_CONTROL | SEQCTR_RESET)) {
    return THD89_ERR_UNEXPECTED;
  }
  return THD89_SUCCESS;
}

thd89_result_t thd89_handshake(const uint8_t* secret, size_t secret_size) {
  // step1. Send `Handshake.Hello` message
  // use pre shared secret key agreement
  // Message: <Handshake.Hello[1]>|PVER[1]
  const uint8_t HANDSHAKE_HELLO[] = {
      SCTR_HELLO,                   // Handshake hello message
      PVER_FLAG_PRE_SHARED_SECRET,  // security protocol
  };
#define HELLO_RESPONSE_SIZE \
  (SEC_SCTR_SIZE + SEC_PVER_SIZE + SEC_RND_SIZE + SEC_SEQ_SIZE)
  // Response: <Handshake.Hello[1]>|PVER[1]|RND[32]|SSEQ[4]
  uint8_t hello_resp[HELLO_RESPONSE_SIZE] = {0};
  size_t hello_resp_size = 0;
  thd89_result_t ret =
      thd89_transmit(HANDSHAKE_HELLO, sizeof(HANDSHAKE_HELLO), hello_resp,
                     HELLO_RESPONSE_SIZE, &hello_resp_size);
  THD89_OP_CHECK(ret);

  // step2. Process Handshake.Hello response
  if (hello_resp_size != HELLO_RESPONSE_SIZE) {
    return THD89_ERR_UNEXPECTED;
  }

  if (hello_resp[0] != SCTR_HELLO ||
      hello_resp[1] != PVER_FLAG_PRE_SHARED_SECRET) {
    return THD89_ERR_UNEXPECTED;
  }
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
  aes_encrypt_key128(&keys[0], &thd89_ctx.enc_ctx);
  aes_encrypt_key128(&keys[16], &thd89_ctx.dec_ctx);
  memcpy(thd89_ctx.enc_nonce.nonce, &keys[32], SEC_NONCE_SIZE);
  memcpy(thd89_ctx.dec_nonce.nonce, &keys[36], SEC_NONCE_SIZE);
  memzero(keys, sizeof(keys));

  // step3. Prepare Handshake Finished message
#define CIPHERTEXT_SIZE ((SEC_RND_SIZE + SEC_SEQ_SIZE) + SEC_MAC_SIZE)
#define HANDSHAKE_FINISHED_SIZE (SEC_SCTR_SIZE + SEC_SEQ_SIZE + CIPHERTEXT_SIZE)
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
  memcpy(enc_nonce.nonce, thd89_ctx.enc_nonce.nonce, SEC_NONCE_SIZE);
  memcpy(enc_nonce.seq, sseq, SEC_SEQ_SIZE);

  // encrypted payload with mac
  int aes_ret =
      aes_ccm_encrypt(&thd89_ctx.enc_ctx, enc_nonce.encryption_nonce,
                      SEC_ENC_NONCE_SIZE, associated, sizeof(associated),
                      payload, payload_size, SEC_MAC_SIZE, ciphertext);
  if (aes_ret != EXIT_SUCCESS) {
    return THD89_ERR_UNEXPECTED;
  }

  // step4. Send `Handshake.Finished` message
  // Response: <Handshake.Finished[1]>|MSEQ[4]|<ciphertext[44]>
  uint8_t finished_resp[HANDSHAKE_FINISHED_SIZE];
  size_t finished_resp_len = 0;
  ret =
      thd89_transmit(handshake_finished, HANDSHAKE_FINISHED_SIZE, finished_resp,
                     sizeof(finished_resp), &finished_resp_len);
  THD89_OP_CHECK(ret);

  if (finished_resp_len != HANDSHAKE_FINISHED_SIZE) {
    return THD89_ERR_UNEXPECTED;
  }
  if (finished_resp[0] != SCTR_FINISHED) {
    return THD89_ERR_UNEXPECTED;
  }

  // step5. verify payload
  uint8_t* mseq = &finished_resp[1];
  ciphertext = &finished_resp[SEC_SCTR_SIZE + SEC_SEQ_SIZE];

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
  memcpy(dec_nonce.nonce, thd89_ctx.dec_nonce.nonce, SEC_NONCE_SIZE);
  memcpy(dec_nonce.seq, mseq, SEC_SEQ_SIZE);
  uint8_t finished_payload[SEC_RND_SIZE + SEC_SEQ_SIZE];

  aes_ret = aes_ccm_decrypt(&thd89_ctx.dec_ctx, dec_nonce.encryption_nonce,
                            SEC_ENC_NONCE_SIZE, associated, sizeof(associated),
                            ciphertext, CIPHERTEXT_SIZE, SEC_MAC_SIZE,
                            finished_payload);
  if (aes_ret != EXIT_SUCCESS) {
    return THD89_ERR_UNEXPECTED;
  }
  if (memcmp(rnd, finished_payload, SEC_RND_SIZE) != 0) {
    return THD89_ERR_UNEXPECTED;
  }
  if (memcmp(mseq, &finished_payload[SEC_RND_SIZE], SEC_SEQ_SIZE) != 0) {
    return THD89_ERR_UNEXPECTED;
  }

  // cache MSEQ SSEQ
  memcpy(thd89_ctx.enc_nonce.seq, mseq, SEC_SEQ_SIZE);
  memcpy(thd89_ctx.dec_nonce.seq, sseq, SEC_SEQ_SIZE);
  // YEAH! connected!
  thd89_ctx.sec_chan_established = true;

  return THD89_SUCCESS;
}

thd89_result_t thd89_execute(const uint8_t* command,
                                     size_t command_size, uint8_t* response,
                                     size_t response_buf_size,
                                     size_t* response_size) {
  return thd89_transmit(command, command_size, response, response_buf_size, response_size);
}

thd89_result_t thd89_secure_execute(const uint8_t* command,
                                     size_t command_size, uint8_t* response,
                                     size_t response_buf_size,
                                     size_t* response_size) {
  // security channel not established, useful for debug
  if (!thd89_ctx.sec_chan_established) {
    return THD89_ERR_UNEXPECTED;
  }

  if (command_size > THD89_MAX_APDU_SIZE) {
    return THD89_ERR_DATA_TOO_LONG;
  }

  size_t sec_size = command_size + SEC_OVERHEAD_SIZE;
  uint8_t sec_buffer[THD89_MAX_APDU_SIZE + SEC_OVERHEAD_SIZE] = {0};

  // step1. increase master sequence number
  increment_seq(thd89_ctx.enc_nonce.seq);

  // step2. Create <Record Exchange> message
  // Message: <Record Exchange[1]>|MSEQ|ciphertext
  // Response: <Record Exchange[1]>|SSEQ|ciphertext
  sec_buffer[0] = SCTR_PROTECTED;
  memcpy(&sec_buffer[1], thd89_ctx.enc_nonce.seq, SEC_SEQ_SIZE);

  uint8_t* ciphertext = &sec_buffer[SEC_SCTR_SIZE + SEC_SEQ_SIZE];
  // Associated: <Record Exchange[1]>|MSEQ[4]|PVER[1]|<Payload length[2]>|
  uint8_t associated[8] = {0};
  associated[0] = SCTR_PROTECTED;
  memcpy(&associated[1], thd89_ctx.enc_nonce.seq, SEC_SEQ_SIZE);
  associated[5] = PVER_FLAG_PRE_SHARED_SECRET;
  associated[6] = command_size >> 8;
  associated[7] = command_size & 0xff;
  int aes_ret =
      aes_ccm_encrypt(&thd89_ctx.enc_ctx, thd89_ctx.enc_nonce.encryption_nonce,
                      SEC_ENC_NONCE_SIZE, associated, sizeof(associated),
                      command, command_size, SEC_MAC_SIZE, ciphertext);
  if (aes_ret) {
    return THD89_ERR_UNEXPECTED;
  }

  // step3. Transmit encrypted command and receive response
  thd89_result_t ret = thd89_transmit(sec_buffer, sec_size, sec_buffer,
                                      sizeof(sec_buffer), &sec_size);
  THD89_OP_CHECK(ret);

  // step4. increase slave sequence number
  increment_seq(thd89_ctx.dec_nonce.seq);

  // check response
  if (sec_size < SEC_OVERHEAD_SIZE) {
    return THD89_ERR_UNEXPECTED;
  }

  if (sec_buffer[0] != SCTR_PROTECTED) {
    return THD89_ERR_UNEXPECTED;
  }
  if (memcmp(&sec_buffer[1], thd89_ctx.dec_nonce.seq, SEC_SEQ_SIZE) != 0) {
    return THD89_ERR_UNEXPECTED;
  }

  size_t payload_size = sec_size - SEC_OVERHEAD_SIZE;
  if (payload_size > response_buf_size) {
    return THD89_ERR_BUFFER_SMALL;
  }

  // step5.Decrypt response
  // Associated: <Record Exchange[1]>|SSEQ[4]|PVER[1]|Payload Length[2]
  associated[0] = SCTR_PROTECTED;
  memcpy(&associated[1], thd89_ctx.dec_nonce.seq, SEC_SEQ_SIZE);
  associated[5] = PVER_FLAG_PRE_SHARED_SECRET;
  associated[6] = payload_size >> 8;
  associated[7] = payload_size & 0xff;
  aes_ret = aes_ccm_decrypt(
      &thd89_ctx.dec_ctx, thd89_ctx.dec_nonce.encryption_nonce,
      SEC_ENC_NONCE_SIZE, associated, sizeof(associated), ciphertext,
      payload_size + SEC_MAC_SIZE, SEC_MAC_SIZE, response);
  if (aes_ret != EXIT_SUCCESS) {
    return THD89_ERR_UNEXPECTED;
  }
  *response_size = payload_size;
  return THD89_SUCCESS;
}
