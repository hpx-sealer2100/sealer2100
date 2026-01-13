#include "lv_font_ex.h"

// Apple PingFang, Medium, 20
static binary_font_t binary_20 = {
    .path = FONT_DIR"/res/lv_font_ping_fang_medium_20.bin",
    .f = NULL,
    .glyph_location = 0,
    .xbf = {
        .min = 0x20,
        .max = 0xFFFF,
        .bpp = 4,
    },
};

const lv_font_t lv_font_ping_fang_medium_20 = {
    .get_glyph_bitmap = user_font_get_bitmap,
    .get_glyph_dsc = user_font_get_glyph_dsc,
    .line_height = 24,
    .base_line = 4,
    .user_data = &binary_20,
};

// Apple PingFang, Medium, 32
static binary_font_t binary_32 = {
    .path = FONT_DIR"/res/lv_font_ping_fang_medium_32.bin",
    .f = NULL,
    .glyph_location = 0,
    .xbf = {
        .min = 0x20,
        .max = 0xFFFF,
        .bpp = 4,
    },
};

const lv_font_t lv_font_ping_fang_medium_32 = {
    .get_glyph_bitmap = user_font_get_bitmap,
    .get_glyph_dsc = user_font_get_glyph_dsc,
    .line_height = 34,
    .base_line = 6,
    .user_data = &binary_32,
};

// Apple PingFang, Medium, 44
static binary_font_t binary_44 = {
    .path = FONT_DIR"/res/lv_font_ping_fang_medium_44.bin",
    .f = NULL,
    .glyph_location = 0,
    .xbf = {
        .min = 0x20,
        .max = 0xFFFF,
        .bpp = 4,
    },
};

const lv_font_t lv_font_ping_fang_medium_44 = {
    .get_glyph_bitmap = user_font_get_bitmap,
    .get_glyph_dsc = user_font_get_glyph_dsc,
    .line_height = 48,
    .base_line = 10,
    .user_data = &binary_44,
};
