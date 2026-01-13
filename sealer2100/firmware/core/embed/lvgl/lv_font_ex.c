#include "lv_font_ex.h"

#include <string.h>
#include <memory.h>

#include "secure_heap.h"

#define font_malloc pvPortMalloc
#define font_free vPortFree

// 缓存的fat文件不连续的簇数量，用于提高 f_seek 的速度
// 这个值和文件碎片化有关，文件碎片化越严重，需要缓存的数量越多
// SZ_TBL = 2 * N + 2 这里的 N是碎片数量，即不连续的簇的数量
// 经常反复写的文件一般碎片化比较严重，但对于字体文件，我们不会经常写，或者几乎写进去就不会再修改了
// 所以字体文件基本上都是连续存储的，也就是只有一个簇
// 假设不连续的簇为128（文件碎片化已经非常严重），这里只需要设置为 258
// 即使没有命中缓存，fat文件系统也是可以遍历文件查找到对应的数据，只是少慢一点
#define SZ_TBL 258

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
  FRESULT ret;
  uint8_t loca[4];
  UINT nums;
  uint32_t len;

  if (binary->f) return 0;
  binary->f = font_malloc(sizeof(FIL));
  memset(binary->f, 0, sizeof(FIL));
  if (!binary->f) return -1;
  ret = f_open(binary->f, binary->path, FA_READ);
  if (ret != FR_OK) {
    goto err;
  }
  binary->f->cltbl = font_malloc(sizeof(DWORD) * SZ_TBL);
  if (!binary->f->cltbl) {
    goto err;
  }
  binary->f->cltbl[0] = SZ_TBL; /* Set table size */
  f_lseek(binary->f, CREATE_LINKMAP); /* Create CLMT */

  ret = f_lseek(binary->f, LOCA_OFFSET);
  if (FR_OK != ret) {
    goto err;
  }

  if (f_read(binary->f, &len, 4, &nums) != FR_OK ||
      f_read(binary->f, loca, 4, &nums) != FR_OK ||
      memcmp("loca", loca, 4) != 0) {
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
    if (binary->f->cltbl) {
      font_free(binary->f->cltbl);
      binary->f->cltbl = NULL;
    }
    f_close(binary->f);
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
  UINT readed = 0;
  FRESULT ret = 0;

  // get the infomation of letter: (from, to)
  ret = f_lseek(binary->f, offset + LOCA_OFFSET + LOCA_VALUE_OFFSET);
  if (ret != FR_OK) {
    return 1;
  }
  ret = f_read(binary->f, buf, 8, &readed);
  if (ret != 0 && readed != 8) {
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
  ret = f_lseek(binary->f, binary->glyph_location + pos[0]);
  if (ret != FR_OK) {
    font_free(font_data);
    return 1;
  }
  ret = f_read(binary->f, font_data->data, len, &readed);
  if (ret != 0 && readed != len) {
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
