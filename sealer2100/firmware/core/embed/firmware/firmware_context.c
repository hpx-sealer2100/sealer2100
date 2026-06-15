#include "firmware_context.h"

#include <string.h>

firmware_context_t firmware_ctx;

void firmware_context_init(void) {
    memset(&firmware_ctx, 0, sizeof(firmware_context_t));
}
