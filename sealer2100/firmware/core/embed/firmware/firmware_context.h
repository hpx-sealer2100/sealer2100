#ifndef __FIRMWARE_CONTEXT_H__
#define __FIRMWARE_CONTEXT_H__

#include "fs.h"

typedef struct {
    fs_t fs;
} firmware_context_t;

extern firmware_context_t firmware_ctx;

void firmware_context_init(void);

#endif
