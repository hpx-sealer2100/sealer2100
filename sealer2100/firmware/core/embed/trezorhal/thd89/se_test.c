#include "se.h"

#include <stdio.h>
#include <string.h>

#include "stm32h7xx_hal.h"

#include "ecdsa.h"
#include "nist256p1.h"
#include "hmac.h"
#include "rand.h"

#define ERROR() do { \
    printf("failed at %s: %d\n", __FILE__, __LINE__); \
    while(1); \
} while(0)

extern void log_data(uint8_t* data, size_t data_size);

static void test_common(void) {
    int ret = se_verify_app();
    // in se app state, this command is not supported
    printf("verify app: 0x%02x \n", ret);

    // 0. 获取设备状态
    se_state_t state;
    if (!se_get_running_state(&state)) {
        if (state == 0) {
            printf("bootloader\n");
        } else if (state == 1) {
            printf("app\n");
        } else {
            printf("unknown\n");
        }
    } else {
        printf("get state failed\n");
        ERROR();
    }
}

static const uint8_t __pre_shared_key__[] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
};
static void test_factory(void) {
    // 0. 获取生产状态
    life_cycle_t life_cycle;
    if (!se_get_life_cycle(&life_cycle)) {
        if (life_cycle == 0) {
            printf("factory\n");
        } else if (life_cycle == 1) {
            printf("user\n");
        } else {
            printf("unknown\n");
        }
    } else {
        printf("get life cycle failed\n");
        ERROR();
    }

    // 1. 擦除设备信息
    if (!se_erase_storage()) {
        printf("erase storage success\n");
    } else {
        printf("erase storage failed\n");
        ERROR();
    }

    // need reboot after erase device
    se_reboot();
    // delay a moment wait se start up
    HAL_Delay(50);
    se_conn_reset();

    // 2. 设置序列号
    char *sn = "DS202505170001";
    if (!se_set_sn((uint8_t*)sn, strlen(sn))) {
        printf("set sn success\n");
    } else {
        printf("set sn failed\n");
        ERROR();
    }

    // 3. 绑定密钥
    if (!se_set_sheared_key(__pre_shared_key__, sizeof(__pre_shared_key__))) {
        printf("set sheared key success\n");
    } else {
        printf("set sheared key failed\n");
        ERROR();
    }

    // 4. 生成设备密钥
    if (!se_gen_dev_keypair()) {
        printf("gen dev keypair success\n");
    } else {
        printf("gen dev keypair failed\n");
        ERROR();
    }

    // 5. 写入设备证书
    uint8_t cert[0x201] = {0x11};
    if (!se_write_certificate(cert, sizeof(cert))) {
        printf("write certificate success\n");
    } else {
        printf("write certificate failed\n");
        ERROR();
    }

    // 6. 切换生命周期
    if (!se_switch_life_cycle()) {
        printf("switch life cycle success\n");
    } else {
        printf("switch life cycle failed\n");
        ERROR();
    }
    // reboot for switch to user command list
    se_reboot();
    // delay a moment wait se start up
    HAL_Delay(50);
    se_conn_reset();

}

static void test_device(void) {
    char version[17] = {0};
    if (!se_get_version(version)) {
        printf("version: %s\n", version);
    } else {
        printf("get version failed\n");
        ERROR();
    }

    char sn[33] = {0};
    if (!se_get_sn(sn)) {
        printf("sn: %s\n", sn);
    } else {
        printf("get sn failed\n");
        ERROR();
    }

    uint8_t pubkey[65] = {0};
    if (!se_get_dev_pubkey(pubkey)) {
        printf("pubkey: \n");
        log_data(pubkey, sizeof(pubkey));
    } else {
        printf("get pubkey failed\n");
        ERROR();
    }

    uint8_t cert[0x201] = {0};
    size_t cert_len = 0;
    if (!se_get_certificate_len(&cert_len)) {
        printf("cert len: %d\n", cert_len);
    } else {
        printf("get cert len failed\n");
        ERROR();
    }
    if (!se_read_certificate(cert, &cert_len)) {
        printf("read certificate success\n");
    } else {
        printf("read certificate failed\n");
        ERROR();
    }

    char* msg = "1234567890ABCDEF";
    uint8_t sig[64] = {0};
    if (!se_sign_message((uint8_t*)msg, strlen(msg), sig)) {
        printf("sign message success\n");
    } else {
        printf("sign message failed\n");
        ERROR();
    }
    uint8_t digest[32] = {0};
    sha256_Raw((uint8_t*)msg, strlen(msg), digest);
    if (!ecdsa_verify_digest(&nist256p1, pubkey,  sig, digest)) {
        printf("verify success\n");
    } else {
        printf("verify failed\n");
        ERROR();
    }
}

#define PIN_LEN 4
static const uint8_t pin[PIN_LEN] = {'1', '2', '3', '4'};
static void test_pin(void) {
    int ret = 0;
    // pin test
    bool exist = false;
    if (!se_has_pin(&exist)) {
        printf("has pin: %d\n", exist);
    } else {
        printf("has pin failed\n");
        ERROR();
    }

    if (!se_set_pin(pin, PIN_LEN)) {
        printf("set pin success\n");
    } else {
        printf("set pin failed\n");
        ERROR();
    }
    if (!se_has_pin(&exist)) {
        printf("has pin: %d\n", exist);
    } else {
        printf("has pin failed\n");
        ERROR();
    }
    ret = se_verify_pin(pin, PIN_LEN);
    if (!ret) {
        printf("verify pin success\n");
    } else {
        printf("verify pin failed: %x\n", ret);
        ERROR();
    }
    int retry = 0;
    if (!se_get_pin_retry(&retry)) {
        printf("pin retry: %d\n", retry);
    } else {
        printf("get pin retry failed\n");
        ERROR();
    }
    if (!se_set_pin_user_max_retry(10)) {
        printf("set pin user max retry success\n");
    } else {
        printf("set pin user max retry failed\n");
        ERROR();
    }
    if (!se_get_pin_retry(&retry)) {
        printf("pin retry: %d\n", retry);
    } else {
        printf("get pin retry failed\n");
        ERROR();
    }
    // verify failed
    printf("use wrong pin to verify, should failed\n");
    uint8_t pin2[PIN_LEN] = {'1', '2', '3', '5'};
    ret = se_verify_pin(pin2, PIN_LEN);
    if (!ret) {
        printf("verify pin success\n");
        ERROR();
    } else {
        printf("verify pin failed: %x\n", ret);
    }
    if (!se_get_pin_retry(&retry)) {
        printf("pin retry: %d\n", retry);
    } else {
        printf("get pin retry failed\n");
        ERROR();
    }

    uint8_t pin3[PIN_LEN] = {'4', '3', '2', '1'};
    if (!se_change_pin(pin, PIN_LEN, pin3, PIN_LEN)) {
        printf("change pin success\n");
    } else {
        printf("change pin failed\n");
        ERROR();
    }

    // other way to change pin
    // user can set pin again when verified
    if (!se_set_pin(pin, PIN_LEN)) {
        printf("set pin success\n");
    } else {
        printf("set pin failed\n");
        ERROR();
    }

    // reset pin can do when pin locked or verified
    // change pin success will make device verified
    if (!se_reset_pin()) {
        printf("reset pin success\n");
    } else {
        printf("reset pin failed\n");
        ERROR();
    }
    if (!se_has_pin(&exist)) {
        printf("has pin: %d\n", exist);
    } else {
        printf("has pin failed\n");
        ERROR();
    }
}

uint8_t __data__[] = {0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38};
static void test_file(void) {
    int ret = 0;
    // set a pin for file test
    if (!se_set_pin(pin, PIN_LEN)) {
        printf("set pin success\n");
    } else {
        printf("set pin failed\n");
        ERROR();
    }
    se_forget_pin();

    uint16_t __id__ = OID_USER_OBJ_BASE + 0;
    if (!se_write_file(__id__, __data__, sizeof(__data__))) {
        printf("write file success\n");
    } else {
        printf("write file failed\n");
        ERROR();
    }

    // the default access is public

    size_t __data_size__ = 0;
    if (!se_get_file_size(__id__, &__data_size__) && __data_size__ == sizeof(__data__)) {
        printf("get file size success\n");
    } else {
        printf("get file size failed\n");
        ERROR();
    }

    uint8_t __data2__[8] = {0};
    size_t __data2_size__ = 0;
    if (!se_read_file(__id__, __data2__, &__data2_size__)) {
        printf("read file success\n");
    } else {
        printf("read file failed\n");
        ERROR();
    }
    if (__data2_size__ != __data_size__ || memcmp(__data2__, __data__, __data_size__) != 0) {
        printf("read file data error\n");
        ERROR();
    }

    // set access to private
    if (!se_set_file_access(__id__, 0)) {
        printf("set file access success\n");
    } else {
        printf("set file access failed\n");
        ERROR();
    }

    // can't delete, because have write access
    printf("can't delete file, because have write access\n");
    if (!se_delete_file(__id__)) {
        printf("delete file success\n");
        ERROR();
    } else {
        printf("delete file failed\n");
    }

    // verify user pin
    ret = se_verify_pin(pin, PIN_LEN);
    if (!ret) {
        printf("verify pin success\n");
    } else {
        printf("verify pin failed: %x\n", ret);
        ERROR();
    }

    // now can delete file
    printf("now can delete file, because have verify pin\n");
    if (!se_delete_file(__id__)) {
        printf("delete file success\n");
    } else {
        printf("delete file failed\n");
        ERROR();
    }

    // pin have verified we can wipe user storeage
    if (!se_wipe_user_storage()) {
        printf("wipe user storage success\n");
    } else {
        printf("wipe user storage failed\n");
        ERROR();
    }
    // after wipe user storage, `ALL` object be deleted, include the user pin
}

static void test_crypto(void) {
    uint8_t rnd[32] = {0};
    if (!se_random(sizeof(rnd), rnd)) {
        printf("random success\n");
        printf("rnd: ");
        log_data(rnd, sizeof(rnd));
    } else {
        printf("random failed\n");
        ERROR();
    }

    #define SECRET_KEY_ID (OID_USER_OBJ_BASE + 0x10)
    if (!se_gen_secret(SECRET_KEY_ID, 0x10)) {
        printf("gen secret key success\n");
    } else {
        printf("gen secret key failed\n");
        ERROR();
    }

    #define AES_128_KEY_ID (OID_USER_OBJ_BASE + 0x11)
    if (!se_gen_sym_key(AES_128_KEY_ID, KEY_TYPE_AES_128)) {
        printf("gen sym key success\n");
    } else {
        printf("gen sym key failed\n");
        ERROR();
    }

    #define ECC_KEY_ID (OID_USER_OBJ_BASE + 0x12)
    if (!se_gen_keypair(ECC_KEY_ID, KEY_TYPE_NISTP256)) {
        printf("gen sym key success\n");
    } else {
        printf("gen sym key failed\n");
        ERROR();
    }

    uint8_t __pk__[65] = {0};
    size_t __pk_size__ = 0;
    if (!se_get_pubkey(ECC_KEY_ID, __pk__, &__pk_size__)) {
        printf("get public key success\n");
        printf("pk: ");
        log_data(__pk__, sizeof(__pk__));
    } else {
        printf("get public key failed\n");
        ERROR();
    }

    // we a fixed `random` as key for test
    // uint8_t __key__[16] = {
    //     0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
    //     0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c,
    // };
    // uint8_t __cmac__[16] = {
    //     0xb0, 0x8c, 0x1b, 0x98, 0xfd, 0x48, 0xc0, 0xec,
    //     0x29, 0xc7, 0x49, 0xb2, 0x02, 0x7d, 0x22, 0xbd
    // };

    uint8_t cmac[16] = {0};
    if (!se_cmac(AES_128_KEY_ID, __data__, sizeof(__data__), cmac)) {
        printf("cmac success\n");
    } else {
        printf("cmac failed\n");
        ERROR();
    }

    // if (memcmp(__cmac__, cmac, sizeof(__cmac__)) == 0) {
    //     printf("cmac success\n");
    // }else {
    //     printf("cmac failed\n");
    // }

    uint8_t hmac[32] = {0};
    if (!se_hmac(SECRET_KEY_ID, __data__, sizeof(__data__), hmac)) {
        printf("hmac success\n");
    } else {
        printf("hmac failed\n");
        ERROR();
    }

    // uint8_t __hmac__[32] = {0};
    // hmac_sha256(__key__, 16, __data__, sizeof(__data__), __hmac__);
    // if (memcmp(__hmac__, hmac, sizeof(__hmac__)) == 0) {
    //     printf("hmac success\n");
    // } else {
    //     printf("hmac failed\n");
    // }

    uint8_t __sk2__[32] = {0};
    uint8_t __pk2__[65] = {0};
    random_buffer(__sk2__, sizeof(__sk2__));

    ecdsa_get_public_key65(&nist256p1, __sk2__, __pk2__);
    printf("pk2: ");
    log_data(__pk2__, sizeof(__pk2__));

    uint8_t __shared__[32] = {0};
    if (!se_ecdh(ECC_KEY_ID, __pk2__, sizeof(__pk2__), __shared__)) {
        printf("ecdh success\n");
        printf("shared: ");
        log_data(__shared__, sizeof(__shared__));
    } else {
        printf("ecdh failed\n");
        ERROR();
    }

    uint8_t __shared2__[65] = {0};
    ecdh_multiply(&nist256p1, __sk2__, __pk__, __shared2__);
    printf("shared2: ");
    log_data(__shared2__, sizeof(__shared2__));

    if (memcmp(__shared__, &__shared2__[1], sizeof(__shared__)) == 0) {
        printf("ecdh success\n");
    } else {
        printf("ecdh failed\n");
        ERROR();
    }

}

__attribute__((noreturn))
static void test_create_files(void) {

    if (!se_set_pin(pin, PIN_LEN)) {
        printf("set pin success\n");
    } else {
        printf("set pin failed\n");
        ERROR();
    }
    int count = 0;
    uint8_t buf[128] = {0};
    while (1) {
        HAL_Delay(100);
        size_t size = 0;
        if (!se_user_storage_size(&size)) {
            printf("[%03d] get user storage size: %d\n", count, size);
        } else {
            printf("[%03d] get user storage size failed\n", count);
            ERROR();
        }
        if (!se_verify_pin(pin, PIN_LEN)) {
            printf("[%03d] verify pin success\n", count);
        } else {
            printf("[%03d] verify pin failed\n", count);
            ERROR();
        }
        // test_crypto();
        random_buffer(buf, sizeof(buf));
        if (!se_write_file(OID_USER_OBJ_BASE + 0x20, buf, sizeof(buf))) {
            printf("[%03d] write file success\n", count);
        } else {
            printf("[%03d] write file failed\n", count);
            ERROR();
        }
        uint8_t buf2[128] = {0};
        size_t buf2_size = 0;
        if (!se_read_file(OID_USER_OBJ_BASE + 0x20, buf2, &buf2_size)) {
            printf("[%03d] read file success\n", count);
        } else {
            printf("[%03d] read file failed\n", count);
            ERROR();
        }
        if (memcmp(buf, buf2, sizeof(buf)) != 0) {
            printf("[%03d] read file data error\n", count);
            ERROR();
        }
        count++;
    }
}

void se_test(void) {
    se_init();

    test_common();
    test_factory();
    test_device();

    if (!se_handshake(__pre_shared_key__, sizeof(__pre_shared_key__))) {
        printf("handshake success\n");
    } else{
        printf("handshake failed\n");
        ERROR();
    }

    test_pin();
    test_file();
    test_crypto();

    // test create file to compact storage
    (void)test_create_files;
    // test_create_files();

    // set a pin for regenerate keys
    if (!se_set_pin(pin, PIN_LEN)) {
        printf("set pin success\n");
    } else {
        printf("set pin failed\n");
        ERROR();
    }
    int count = 0;
    while (1) {
        printf("[%06d] test regenerate keys\n", count);
        HAL_Delay(1000);
        test_device();
        // re-generate need verify pin
        if (!se_verify_pin(pin, PIN_LEN)) {
            printf("verify pin success\n");
        } else {
            printf("verify pin failed\n");
            ERROR();
        }
        test_crypto();
        printf("[%06d] test regenerate keys\n", count);
        count++;
    }

}
