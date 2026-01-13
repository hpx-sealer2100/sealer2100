#define ZBAR_MEMORY_IN_SRAM 0
#define LOG_ALLOC 0

#if LOG_ALLOC
#define LOG(...) printf
#else
#define LOG(...)
#endif

#if ZBAR_MEMORY_IN_SRAM
#include "micropython.h"
#else
#include "secure_heap.h"
#endif
#include "zbar_malloc.h"
#include "string.h"
#include <stdio.h>


void *zbar_malloc(size_t size) {
#if ZBAR_MEMORY_IN_SRAM
  void* ptr = m_malloc(size);
#else
  void* ptr = pvPortMalloc(size);
#endif
  LOG("zbar malloc: %p, size: %d\n", ptr, size);
  return ptr;
}

void zbar_free(void *ptr) {
  LOG("zbar free: %p\n", ptr);
#if ZBAR_MEMORY_IN_SRAM
  m_free(ptr);
#else
  vPortFree(ptr);
#endif
}

void *zbar_realloc(void *ptr, size_t size) {
#if ZBAR_MEMORY_IN_SRAM
  ptr = m_realloc(ptr, size);
#else
  ptr = pvPortReMalloc(ptr, size);
#endif
  LOG("zbar realloc: %p, size: %d\n", ptr, size);
  return ptr;
}

void *zbar_calloc(size_t nmemb, size_t size) {
#if ZBAR_MEMORY_IN_SRAM
  void *ptr = m_malloc(nmemb * size);
#else
  void *ptr = pvPortMalloc(nmemb * size);
#endif
  if (ptr != NULL) {
    memset(ptr, 0, nmemb * size);
  }
  LOG("zbar calloc: %p, size: %d\n", ptr, size);
  return ptr;
}
