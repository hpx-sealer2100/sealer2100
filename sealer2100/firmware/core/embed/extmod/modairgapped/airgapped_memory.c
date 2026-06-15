#include "common.h"
#include <stdlib.h>
#ifdef EMULATOR
#include "memory.h"
#else
#include "secure_heap.h"
#endif
void* RustMalloc(int32_t size)
{
#ifdef EMULATOR
    return malloc(size);
#else
    return pvPortMalloc(size);
#endif
}

void RustFree(void* p)
{
#ifdef EMULATOR
    free(p);
#else
    vPortFree(p);
#endif
}

void LogRustMalloc(void* p, uint32_t size)
{
    (void)p;
    (void)size;
}

void LogRustFree(void* p)
{ (void)p; }

void LogRustPanic(char* panic_info)
{ __fatal_error(NULL, panic_info, NULL, 0, NULL); }

#ifdef EMULATOR
int32_t GenerateTRNGRandomness(uint8_t *randomness, uint8_t len)
{
    for (size_t i = 0; i < len; i++) {
        randomness[i] = rand() % 256;
    }
    return 0;
}

#endif
