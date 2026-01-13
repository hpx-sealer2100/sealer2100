#include "lv_font_ex.h"

// Apple PingFang, Regular, 20
static binary_font_t binary_20 = {
    .path = FONT_DIR"/res/lv_font_ping_fang_regular_20.bin",
    .f = NULL,
    .glyph_location = 0,
    .xbf = {
        .min = 0x20,
        .max = 0xFFFF,
        .bpp = 4,
    },
};

const lv_font_t lv_font_ping_fang_regular_20 = {
    .get_glyph_bitmap = user_font_get_bitmap,
    .get_glyph_dsc = user_font_get_glyph_dsc,
    .line_height = 22,
    .base_line = 4,
    .user_data = &binary_20,
};

// Apple PingFang, Regular, 24
static binary_font_t binary_24 = {
    .path = FONT_DIR"/res/lv_font_ping_fang_regular_24.bin",
    .f = NULL,
    .glyph_location = 0,
    .xbf = {
        .min = 0x20,
        .max = 0xFFFF,
        .bpp = 4,
    },
};

const lv_font_t lv_font_ping_fang_regular_24 = {
    .get_glyph_bitmap = user_font_get_bitmap,
    .get_glyph_dsc = user_font_get_glyph_dsc,
    .line_height = 27,
    .base_line = 5,
    .user_data = &binary_24,
};