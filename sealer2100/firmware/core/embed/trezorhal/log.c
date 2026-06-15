#include "log.h"

#if defined(LOG_ENABLED) && LOG_ENABLED
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>

#if EMULATOR
#include <sys/time.h>
#include <time.h>
#else
#include "common.h"
#define delay_get_ticks hal_ticks_ms
#endif

// 内部日志函数
void log_message(const char *level, const char *color, const char *module, const char *fmt, ...) {
  if (!LOG_ENABLED) return;
  va_list args;
  va_start(args, fmt);

#if EMULATOR
  // 获取当前时间
  struct timeval tv;
  struct tm tm;

  gettimeofday(&tv, NULL);      // 获取当前时间（秒和微秒）
  tm = *localtime(&tv.tv_sec);  // 转换为本地时间

  printf("%s[%02d:%02d:%02d.%03ld] %s [%s]: ", color, tm.tm_hour, tm.tm_min, tm.tm_sec, (long)tv.tv_usec / 1000, level,
         module);
#else
  uint32_t ticks = delay_get_ticks();
  printf("%s[%08ld] %s [%s]: ", color, ticks, level, module);
#endif

  vprintf(fmt, args);
  printf("%s\n", COLOR_RESET);

  va_end(args);
}
void log_hexdump(const char *level, const char *color, const char *module, const char *label, const void *buf,
                 size_t len) {
  if (!LOG_ENABLED) return;

#if EMULATOR
  // 获取当前时间
  struct timeval tv;
  struct tm tm;

  gettimeofday(&tv, NULL);      // 获取当前时间（秒和微秒）
  tm = *localtime(&tv.tv_sec);  // 转换为本地时间
#else
  uint32_t ticks = delay_get_ticks();
#endif

  if (!buf || len == 0) {
#if EMULATOR
    printf("%s[%02d:%02d:%02d.%03ld] %s [%s]: HEXDUMP: %s: Empty buffer%s\n", color, tm.tm_hour, tm.tm_min, tm.tm_sec,
           (long)tv.tv_usec / 1000, level, module, label, COLOR_RESET);
#else
    printf("%s[%08ld] %s [%s]: HEXDUMP: %s: Empty buffer%s\n", color, ticks, level, module, label, COLOR_RESET);
#endif
    return;
  }

  // 输出第一行：标签和长度信息
#if EMULATOR
  printf("%s[%02d:%02d:%02d.%03ld] %s [%s]: HEXDUMP: %s: Length=%zu%s\n", color, tm.tm_hour, tm.tm_min, tm.tm_sec,
         (long)tv.tv_usec / 1000, level, module, label, len, COLOR_RESET);
#else
  printf("%s[%08ld] %s [%s]: HEXDUMP: %s: Length=%u%s\n", color, ticks, level, module, label, len, COLOR_RESET);
#endif

  const unsigned char *data = (const unsigned char *)buf;

  // 输出每行16字节的内容
  for (size_t i = 0; i < len; i += 16) {
    // 输出偏移地址
    printf("  %08x: ", (int)i);

    // 输出16进制数据
    for (size_t j = 0; j < 16; ++j) {
      if (i + j < len) {
        printf("%02x ", data[i + j]);
      } else {
        printf("   ");  // 不足16字节时填充空格
      }
    }

    // 输出ASCII部分
    printf(" |");
    for (size_t j = 0; j < 16; ++j) {
      if (i + j < len) {
        unsigned char c = data[i + j];
        printf("%c", isprint(c) ? c : '.');  // 可打印字符或用'.'替代
      } else {
        printf(" ");  // 不足16字节时填充空格
      }
    }
    printf("|\n");
  }
}

#endif  // LOG_ENABLED
