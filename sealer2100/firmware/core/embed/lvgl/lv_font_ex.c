#include "lv_font_ex.h"

#include <string.h>
#include <memory.h>
#include "log.h"

#include "fs.h"

#ifndef TREZOR_EMULATOR
#include "secure_heap.h"
#include "uart_log.h"

#define font_malloc pvPortMalloc
#define font_free vPortFree
#else
#include <stdlib.h>
#define font_malloc malloc
#define font_free free
#endif

#define FONT_CACHE_NUM 256

#define LOCA_OFFSET 0x4C
#define LOCA_VALUE_OFFSET 0x0C
#define GLYPH_OFFSET 0x08


#define MODULE "lvgl font"
#define ENABLE_LOG 0
#define ENABLE_DETAIL 0

#if !ENABLE_LOG
// disable log
#undef LOG_DEBUG
#undef LOG_HEXDUMP_DEBUG
#define LOG_DEBUG(module, fmt, ...)                log_dummy(module, fmt, ##__VA_ARGS__)
#define LOG_HEXDUMP_DEBUG(module, label, buf, len) log_dummy(module, label, buf, len)
#endif

// need user provide the file system handle
extern fs_t* __fs__;

int font_cache_init(font_cache_t *cache) {
  cache->fonts = font_malloc(sizeof(font_ptr_t) * FONT_CACHE_NUM);
  if (!cache->fonts) {
    return 1;
  }
  cache->cur = cache->fonts;
  memset(cache->fonts, 0, sizeof(font_ptr_t) * FONT_CACHE_NUM);
  return 0;
}

void font_cache_add(font_cache_t *cache, const font_t* font) {
  // the cache is ring buffer

  // have cached, free prev cached
  if (*cache->cur) {
    font_free(*cache->cur);
  }

  // cache
  *cache->cur = (font_t*)font;

  // update cur pointer
  cache->cur++;
  font_ptr_t* end = cache->fonts + FONT_CACHE_NUM;
  if (cache->cur == end) {
    cache->cur = cache->fonts;
  }
}

const font_t* font_cache_get(const font_cache_t *cache, uint32_t letter) {
  font_ptr_t *p = cache->fonts;
  font_ptr_t *end = cache->fonts + FONT_CACHE_NUM;

  while (p < end && *p) {
    if ((*p)->letter == letter) {
      return *p;
    }
    p++;
  }
  return NULL;
}

int binary_font_open(binary_font_t *binary) {
  uint8_t loca[4];
  uint32_t len;
  int ret;

  if (binary->f) return 0;
  binary->f = font_malloc(sizeof(lv_fs_file_t));
  memset(binary->f, 0, sizeof(lv_fs_file_t));
  if (!binary->f) return -1;
  LOG_DEBUG(MODULE, "open font: %s", binary->path);
  ret = fs_file_open(__fs__, binary->f, binary->path, FS_O_RDONLY);
  if (ret < 0) {
    goto err;
  }

  ret = fs_file_seek(binary->f, LOCA_OFFSET, FS_SEEK_SET);
  if (ret < 0) {
    goto err;
  }

  ret = fs_file_read(binary->f, &len, 4);
  if (ret < 0) {
    goto err;
  }
  ret = fs_file_read(binary->f, loca, 4);
  if (ret < 0) {
    goto err;
  }

  if (memcmp("loca", loca, 4) != 0) {
    goto err;
  }

  binary->glyph_location = len + LOCA_OFFSET;

  ret = font_cache_init(&binary->cache);
  if(ret) {
    goto err;
  }
  return 0;
err:
  LOG_DEBUG(MODULE, "open font failed: %s", binary->path);
  if (binary->f) {
    fs_file_close(binary->f);
    font_free(binary->f);
    binary->f = NULL;
  }
  return ret;
}

static int font_get_data(binary_font_t* binary, uint32_t letter, uint8_t** data) {
  // not in the range
  if (letter > binary->xbf.max || letter < binary->xbf.min) {
    return 1;
  }

  // not open the file and open failed
  if (!binary->f && binary_font_open(binary)) {
    return 1;
  }

  // find cache first
  font_t *font_data = (font_t*)font_cache_get(&binary->cache, letter);
  if (font_data) {
    // printf("get cached: %d\n", letter);
    *data = font_data->data;
    return 0;
  }

  /// not cached, read from file
  uint32_t offset = (letter - binary->xbf.min + 1) * 4;
  uint32_t len = 0;
  uint8_t buf[8] = {0};
  int ret = 0;

  // get the infomation of letter: (from, to)
  ret = fs_file_seek(binary->f, offset + LOCA_OFFSET + LOCA_VALUE_OFFSET, FS_SEEK_SET);
  if (ret < 0) {
    return 1;
  }
  ret = fs_file_read(binary->f, buf, 8);
  if (ret != 8) {
    return 1;
  }
  uint32_t *pos = (uint32_t*)buf;
  len = pos[1] - pos[0];
  font_data = font_malloc(len + sizeof(font_t));
  if (!font_data) {
    return 1;
  }
  font_data->letter = letter;
  font_data->size = len;
  ret = fs_file_seek(binary->f, binary->glyph_location + pos[0], FS_SEEK_SET);
  if (ret < 0) {
    font_free(font_data);
    return 1;
  }
  ret = fs_file_read(binary->f, font_data->data, len);
  if (ret != len) {
    font_free(font_data);
    return 1;
  }
#if ENABLE_DETAIL
  LOG_DEBUG(MODULE, "cache : %c", letter);
#endif
  font_cache_add(&binary->cache, font_data);
  *data = font_data->data;
  return 0;
}

const uint8_t* user_font_get_bitmap(const lv_font_t *font, uint32_t unicode_letter) {
#if ENABLE_DETAIL
  LOG_DEBUG(MODULE, "geting bitmap: %c", unicode_letter);
#endif
  binary_font_t *binary = (binary_font_t *)font->user_data;
  uint8_t *data = NULL;
  if (font_get_data(binary, unicode_letter, &data)) {
#if ENABLE_DETAIL
    LOG_DEBUG(MODULE, "get bitmap failed: %c", unicode_letter);
#endif
    return NULL;
  }
#if ENABLE_DETAIL
  LOG_DEBUG(MODULE, "get bitmap: %c", unicode_letter);
  uart_log_flush();
#endif
  return data + sizeof(glyph_dsc_t);
}
bool user_font_get_glyph_dsc(const lv_font_t *font, lv_font_glyph_dsc_t *dsc_out, uint32_t unicode_letter, uint32_t unicode_letter_next) {
  binary_font_t *binary = (binary_font_t *)font->user_data;
  uint8_t *data = NULL;

  if (font_get_data(binary, unicode_letter, &data)) {
#if ENABLE_DETAIL
    LOG_DEBUG(MODULE, "get glyph dsc failed: %c", unicode_letter);
#endif
    return false;
  }
  glyph_dsc_t *gdsc = (glyph_dsc_t *)data;
  dsc_out->adv_w = (gdsc->adv_w + (1 << 3)) >> 4;
  dsc_out->box_h = gdsc->box_h;
  dsc_out->box_w = gdsc->box_w;
  dsc_out->ofs_x = gdsc->ofs_x;
  dsc_out->ofs_y = gdsc->ofs_y;
  dsc_out->bpp = binary->xbf.bpp;
  dsc_out->is_placeholder = false;
  #if ENABLE_DETAIL
  LOG_DEBUG(MODULE, "get glyph dsc: %c", unicode_letter);
  LOG_DEBUG(MODULE, "adv_w: %d, w: %d, h: %d, x: %d, y: %d, bpp: %d",
    dsc_out->adv_w,
    dsc_out->box_w, dsc_out->box_h,
    dsc_out->ofs_x, dsc_out->ofs_y,
    dsc_out->bpp);
  uart_log_flush();
  #endif
  return true;
}
