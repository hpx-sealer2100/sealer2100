#include "se.h"

#include <string.h>
#include "hash_to_curve.h"
#include "rand.h"
#include "hmac.h"
#include "storage.h"

#define PIN_STRETCH_ITERATIONS 2
#define SE_USER_PIN_SIZE 32

#define OID_USER_PIN_CMAC_KEY (OID_USER_OBJ_BASE+0)
#define OID_USER_PIN_HMAC_KEY (OID_USER_OBJ_BASE+1)
#define OID_USER_PIN_ECDH_KEY_PAIR (OID_USER_OBJ_BASE+2)
#define OID_USER_PIN_SECRET (OID_USER_OBJ_BASE+3)

// b ^= a;
static inline void xor(const uint8_t* a, uint8_t *b, size_t n) {
    while (n--) {
        *b++ ^= *a++;
    }
}

static int se_user_pin_stretch(HMAC_SHA256_CTX* ctx, const uint8_t input[SE_USER_PIN_SIZE]) {

    int ret = 0;
    // step 1. AES256_CMAC(OID_USER_PIN_CMAC_KEY, input)
    uint8_t cmac[16] = {0};
    if ((ret = se_cmac(OID_USER_PIN_CMAC_KEY, input, SE_USER_PIN_SIZE, cmac)) != 0) {
        goto err;
    }

    // step 2. hmac_sha256_update(cmac)
    hmac_sha256_Update(ctx, cmac, sizeof(cmac));

    uint8_t point[65] = {0};
    if (!hash_to_curve_thd89(input, point)) {
        ret = -1;
        goto err;
    }

    // step 3. ECDH(OID_USER_PIN_ECDH_KEY_PAIR, input)
    uint8_t secret[SE_USER_PIN_SIZE] = {0};
    if ((ret = se_ecdh(OID_USER_PIN_ECDH_KEY_PAIR, point, sizeof(point), secret)) != 0) {
        goto err;
    }

    // step 4. hmac_sha256_udpate(secret)
    hmac_sha256_Update(ctx, secret, sizeof(secret));

err:
    memset(cmac, 0, sizeof(cmac));
    memset(point, 0, sizeof(point));
    memset(secret, 0, sizeof(secret));
    return ret;
}

static int se_user_pin_stretch_cmac_ecdh(uint8_t pin[SE_USER_PIN_SIZE]) {
  // for _ in range(PIN_STRETCH_ITERATIONS):
  //   digest = HMAC-SHA256(stretched_pin, "")
  //   cmac_out = CMAC(OID_PIN_CMAC, digest)
  //   ecdh_out = ECDH(OID_PIN_ECDH, digest)
  //   stretched_pin = HMAC-SHA256(stretched_pin, cmac_out || ecdh_out)

  int ret = 0;
  uint8_t digest[SE_USER_PIN_SIZE] = {0};

  for (int i = 0; i < PIN_STRETCH_ITERATIONS; i++) {
    hmac_sha256(pin, SE_USER_PIN_SIZE, NULL, 0, digest);
    HMAC_SHA256_CTX ctx;
    hmac_sha256_Init(&ctx, pin, SE_USER_PIN_SIZE);
    if ((ret = se_user_pin_stretch(&ctx, digest)) != 0) {
      goto err;
    }
    hmac_sha256_Final(&ctx, pin);
  }
err:
  memset(digest, 0, sizeof(digest));
  return ret;
}

static int se_user_pin_stretch_hmac(uint8_t pin[SE_USER_PIN_SIZE]) {
  int ret = 0;
  uint8_t digest[SE_USER_PIN_SIZE] = {0};
  hmac_sha256(pin, SE_USER_PIN_SIZE, NULL, 0, digest);
  if ((ret = se_hmac(OID_USER_PIN_HMAC_KEY, digest, sizeof(digest), digest)) != 0) {
    goto err;
  }

  hmac_sha256(pin, SE_USER_PIN_SIZE, digest, sizeof(digest), pin);
err:
  memset(digest, 0, sizeof(digest));
  return ret;
}

int se_set_user_pin(uint8_t pin[32]) {

  // generate 3 keys for: cmac, ecdh, hmac
  int ret = 0;
  if ((ret = se_gen_sym_key(OID_USER_PIN_CMAC_KEY, KEY_TYPE_AES_256)) != 0) {
      goto err;
  }

  if ((ret = se_gen_keypair(OID_USER_PIN_ECDH_KEY_PAIR, KEY_TYPE_NISTP256)) != 0) {
      goto err;
  }

  if ((ret = se_gen_secret(OID_USER_PIN_HMAC_KEY, KEY_TYPE_AES_256)) != 0) {
      goto err;
  }

  // compute pin', set se pin'
  if ((ret = se_user_pin_stretch_cmac_ecdh(pin)) != 0) {
    goto err;
  }

  if ((ret = se_user_pin_stretch_hmac(pin)) != 0) {
    goto err;
  }

  uint8_t digest[SE_USER_PIN_SIZE] = {0};

  hmac_sha256(pin, SE_USER_PIN_SIZE, NULL, 0, digest);

  if ((ret = se_set_pin(digest, sizeof(digest))) != 0) {
    goto err;
  }
  if ((ret = se_set_pin_user_max_retry(PIN_MAX_TRIES)) != 0) {
    goto err;
  }

  // create a secret
  uint8_t secret[32] = {0};
  if ((ret = se_random(sizeof(secret), secret)) != 0) {
    goto err;
  }

  uint8_t rnd[32] = {0};
  random_buffer(rnd, sizeof(rnd));
  xor(rnd, secret, sizeof(secret));
  if ((ret = se_write_file(OID_USER_PIN_SECRET, secret, sizeof(secret))) != 0) {
    goto err;
  }
  if ((ret = se_set_file_access(OID_USER_PIN_SECRET, 0)) != 0) {
    goto err;
  }

  hmac_sha256(pin, SE_USER_PIN_SIZE, secret, sizeof(secret), pin);
err:
  memset(digest, 0, sizeof(digest));
  memset(rnd, 0, sizeof(rnd));
  memset(secret, 0, sizeof(secret));
  return ret;
}

int se_verify_user_pin(uint8_t pin[32]) {
  int ret = 0;
  if ((ret = se_user_pin_stretch_cmac_ecdh(pin)) != 0) {
    goto err;
  }

  if ((ret = se_user_pin_stretch_hmac(pin)) != 0) {
    goto err;
  }

  uint8_t digest[SE_USER_PIN_SIZE] = {0};

  hmac_sha256(pin, SE_USER_PIN_SIZE, NULL, 0, digest);

  ret = se_verify_pin(digest, sizeof(digest));
  if (ret != 0) {
    goto err;
  }

  // read out secret
  uint8_t secret[SE_USER_PIN_SIZE] = {0};
  size_t len = 0;
  if ((ret = se_read_file(OID_USER_PIN_SECRET, secret, &len)) != 0) {
    goto err;
  }

  hmac_sha256(pin, SE_USER_PIN_SIZE, secret, len, pin);
err:
  memset(digest, 0, sizeof(digest));
  memset(secret, 0, sizeof(secret));
  return ret;
}
