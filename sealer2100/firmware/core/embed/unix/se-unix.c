#include "se.h"
#include "string.h"
#include <stdint.h>
#include <string.h>

static const uint8_t FIXED_SALT[32] = {0xd0, 0xaa, 0x97, 0x03, 0xad, 0xa5, 0x1d, 0xb7, 0x05, 0x47, 0xe7,
                                       0xc8, 0xa2, 0x1e, 0x4c, 0x37, 0xb1, 0xbb, 0x9e, 0xcf, 0xb8, 0xdb,
                                       0xd6, 0x65, 0x71, 0x0e, 0x9b, 0x47, 0x77, 0x41, 0x63, 0x39};

static const uint8_t TEST_PK[65] = {0x04, 0xdb, 0x2b, 0xbb, 0xaf, 0xea, 0xc8, 0x02, 0x19, 0x5b, 0xd7,
                                    0x8b, 0x0e, 0x99, 0x4a, 0x90, 0x7f, 0x58, 0xd9, 0xe4, 0x64, 0x9a,
                                    0x4a, 0xf9, 0xd0, 0x2e, 0x70, 0x72, 0xe0, 0x0b, 0x54, 0x6f, 0x7f,
                                    0x9d, 0x0e, 0xcf, 0x2a, 0x88, 0x3b, 0xce, 0x5a, 0x07, 0x80, 0x1c,
                                    0xe2, 0xc0, 0x1c, 0x77, 0x25, 0x50, 0x03, 0x29, 0x42, 0xb8, 0xf6,
                                    0x5b, 0x18, 0xa2, 0x47, 0xd3, 0x9d, 0x39, 0xfd, 0xd7, 0x4d};

static const char* TEST_CERT =
    "-----BEGIN CERTIFICATE-----"
    "MIIB6jCCAZGgAwIBAgIUJ0TSaUgbyL0n1tU9La7qbAm+26IwCgYIKoZIzj0EAwIw"
    "ZTELMAkGA1UEBhMCQ04xEjAQBgNVBAgMCUhvbmcgS29uZzESMBAGA1UEBwwJSG9u"
    "ZyBLb25nMRYwFAYDVQQKDA1oeXBlcnBheSBJbmMuMRYwFAYDVQQDDA1oeXBlcm1h"
    "dGUuY29tMB4XDTI1MDgyMDAxMjYyOVoXDTM1MDgxODAxMjYyOVowdDELMAkGA1UE"
    "BhMCQ04xEjAQBgNVBAgMCUhvbmcgS29uZzESMBAGA1UEBwwJSG9uZyBLb25nMRcw"
    "FQYDVQQKDA5IeXBlck1hdGUgSW5jLjELMAkGA1UECwwCSFAxFzAVBgNVBAMMDkhQ"
    "MjAyNTA4MTkwMDAxMFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAE2yu7r+rIAhlb"
    "14sOmUqQf1jZ5GSaSvnQLnBy4AtUb3+dDs8qiDvOWgeAHOLAHHclUAMpQrj2Wxii"
    "R9OdOf3XTaMQMA4wDAYDVR0TAQH/BAIwADAKBggqhkjOPQQDAgNHADBEAiBilhmj"
    "3Rv1Tb0NJU1qSWzXy+Ya+qoRr5aT3nLP7qufoAIgT36WxoJL3LPbH1x2ftA3evm2"
    "N51m0nL1F12Uwdc9FGc="
    "-----END CERTIFICATE-----";

static inline void xor
    (const uint8_t* a, uint8_t* b, size_t n) {
        while ( n-- )
        {
            *b++ ^= *a++;
        }
    }

    int
    se_verify_user_pin(uint8_t pin[32])
{
    xor(FIXED_SALT, pin, 32);
    return 0;
}

int se_set_user_pin(uint8_t pin[32])
{
    xor(FIXED_SALT, pin, 32);
    return 0;
}

int se_wipe_user_storage(void)
{
    return 0;
}

int se_forget_pin(void)
{
    return 0;
}

int se_protect_user_storage()
{
    return 0;
}

int se_get_dev_pubkey(uint8_t pubkey[65])
{
    memcpy(pubkey, TEST_PK, 65);
    return 0;
}

int se_get_version(char version[17])
{
    strcpy(version, "9.9.99");
    return 0;
}

int se_read_certificate(uint8_t* cert, size_t* cert_len)
{
    memcpy(cert, TEST_CERT, strlen(TEST_CERT));
    *cert_len = strlen(TEST_CERT);
    return 0;
}

int se_sign_message(uint8_t* msg, size_t msg_len, uint8_t* signature)
{
    (void)msg;
    (void)msg_len;
    (void)signature;
    return -1;
}
