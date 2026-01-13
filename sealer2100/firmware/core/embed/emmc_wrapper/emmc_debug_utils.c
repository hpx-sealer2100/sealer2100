#include "emmc_debug_utils.h"

void __debug_print(bool wait_click, const char* file, int line, const char* func, const char* fmt, ...)
{
    display_orientation(0);
    display_backlight(255);
    display_clear();
    display_refresh();
    display_print_color(RGB16(0x69, 0x69, 0x69), COLOR_BLACK);

    display_printf("\n");
    display_printf("=== Debug Info ===\n");

    display_printf("file: %s:%d\n", file, line);
    display_printf("func: %s\n", func);

    display_printf("message:\n");
    va_list va;
    va_start(va, fmt);
    char buf[256] = {0};
    int len = vsnprintf(buf, sizeof(buf), fmt, va);
    display_print(buf, len);
    va_end(va);
    display_printf("\n");

    display_text(8, 784, "Tap to continue ...", -1, FONT_NORMAL, COLOR_WHITE, COLOR_BLACK);
    while ( wait_click && !touch_click() ) {}
}

static char __HEX_TABLE__[] = "0123456789ABCDEF";
void buffer_to_hex_string(const void* buff, size_t buff_len, char* str)
{
    uint8_t *p = (uint8_t*)buff;
    while (buff_len--)
    {
        char c = *((char*)(p++));
        *str++ = __HEX_TABLE__[c >> 4];
        *str++ = __HEX_TABLE__[c & 0x0F];
    }
    *str++ = '\0';
}
