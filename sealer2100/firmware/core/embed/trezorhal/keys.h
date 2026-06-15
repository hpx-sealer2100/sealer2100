#ifndef __KEYS_H__
#define __KEYS_H__
#include <stdint.h>

#if PRODUCTION
#define KEY_M  2
#define KEY_N  3
#else
#define KEY_M  2
#define KEY_N  3
#endif
#define KEY_SIZE 32

extern const uint8_t* const KEYS[];
#endif
