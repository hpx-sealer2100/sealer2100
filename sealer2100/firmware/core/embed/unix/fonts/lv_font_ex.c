#include "lv_font_ex.h"

#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <stdlib.h>
#include <stdbool.h>

#define font_malloc malloc
#define font_free free

#define FONT_CACHE_NUM 256

#define LOCA_OFFSET 0x4C
#define LOCA_VALUE_OFFSET 0x0C
#define GLYPH_OFFSET 0x08

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
  int ret = 0;
  uint8_t loca[4];
  uint32_t len;

  if (binary->f) return 0;
  binary->f = fopen(binary->path, "r");
  if (!binary->f) {
    ret = 1;
    goto err;
  }
  ret = fseek(binary->f, LOCA_OFFSET, SEEK_SET);
  if (ret) {
    goto err;
  }

  ret = fread(&len, 1, 4, binary->f);
  if (ret != 4) {
    goto err;
  }
  ret = fread(loca, 1, 4, binary->f);
  if (ret != 4) {
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
  if (binary->f) {
    fclose(binary->f);
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
  ret = fseek(binary->f, offset + LOCA_OFFSET + LOCA_VALUE_OFFSET, SEEK_SET);
  if (ret) {
    return 1;
  }
  ret = fread(buf, 1, 8, binary->f);
  if (ret != 8 ) {
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
  ret = fseek(binary->f, binary->glyph_location + pos[0], SEEK_SET);
  if (ret) {
    font_free(font_data);
    return 1;
  }
  ret = fread(font_data->data, 1, len, binary->f);
  if (ret != len) {
    font_free(font_data);
    return 1;
  }
  // printf("cache : %x\n", letter);
  font_cache_add(&binary->cache, font_data);
  *data = font_data->data;
  return 0;
}

const uint8_t* user_font_get_bitmap(const lv_font_t *font, uint32_t unicode_letter) {
  binary_font_t *binary = (binary_font_t *)font->user_data;
  uint8_t *data = NULL;
  if (font_get_data(binary, unicode_letter, &data)) {
    return NULL;
  }
  return data + sizeof(glyph_dsc_t);
}
bool user_font_get_glyph_dsc(const lv_font_t *font, lv_font_glyph_dsc_t *dsc_out, uint32_t unicode_letter, uint32_t unicode_letter_next) {
  binary_font_t *binary = (binary_font_t *)font->user_data;
  uint8_t *data = NULL;

  if (font_get_data(binary, unicode_letter, &data)) {
    return false;
  }
  glyph_dsc_t *gdsc = (glyph_dsc_t *)data;
  dsc_out->adv_w = (gdsc->adv_w + (1 << 3)) >> 4;
  dsc_out->box_h = gdsc->box_h;
  dsc_out->box_w = gdsc->box_w;
  dsc_out->ofs_x = gdsc->ofs_x;
  dsc_out->ofs_y = gdsc->ofs_y;
  dsc_out->bpp = binary->xbf.bpp;
  return true;
}
