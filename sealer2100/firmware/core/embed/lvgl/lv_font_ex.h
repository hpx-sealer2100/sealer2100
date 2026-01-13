#ifndef LV_FONT_EX_H
#define LV_FONT_EX_H

#include "lvgl.h"
#include "ff.h"


typedef struct {
  uint16_t min;
  uint16_t max;
  uint8_t bpp;
  uint8_t reserved[3];
} font_header_t;

typedef struct __attribute__((packed)) {
  uint16_t adv_w;
  uint8_t box_w;
  uint8_t box_h;
  int8_t ofs_x;
  int8_t ofs_y;
} glyph_dsc_t;

typedef struct {
  uint32_t letter;
  uint32_t size;
  uint8_t data[];
} font_t, *font_ptr_t;

typedef struct {
  font_ptr_t* cur;
  font_ptr_t* fonts;
} font_cache_t;

// the user data set to lv_font_t.user_data
typedef struct {
  char* path;             // the font binary file
  FIL* f;                 // the fatfs handle
  uint32_t glyph_location;
  font_header_t xbf;
  font_cache_t cache;
}binary_font_t;

const uint8_t* user_font_get_bitmap(const lv_font_t *font, uint32_t unicode_letter);
bool user_font_get_glyph_dsc(const lv_font_t *font, lv_font_glyph_dsc_t *dsc_out, uint32_t unicode_letter, uint32_t unicode_letter_next);


#endif
