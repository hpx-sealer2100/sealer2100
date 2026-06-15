#include "se.h"

#include <stdint.h>
#include <stddef.h>

int se_erase_storage(void) {
    return se_execute_case1(CMD_ID_ERASE_STORAGE);
}

int se_set_sn(const uint8_t *sn, size_t sn_len) {
    if (sn_len > 32) {
      return 1;
    }
    return se_execute_case2(CMD_ID_SET_SN, sn, sn_len);
}

int se_set_sheared_key(const uint8_t *key, size_t key_len) {
    if (key_len > 32) {
      return 1;
    }
    return se_execute_case2(CMD_ID_SET_PRE_SHARED_KEY, key, key_len);
}

int se_gen_dev_keypair(void) {
    return se_execute_case1(CMD_ID_GEN_DEV_KEY);
}

int se_write_certificate(const uint8_t *cert, size_t cert_len) {
    if (cert_len > 1024) {
      return 1;
    }

    return se_execute_case2(CMD_ID_SET_DEV_CERT, cert, cert_len);
}


int se_switch_life_cycle(void) {
    return se_execute_case1(CMD_ID_SWITCH_LIFE_CYCLE);
}
