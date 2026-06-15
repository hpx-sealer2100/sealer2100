#ifndef __LOG_H__
#define __LOG_H__
#include <stddef.h>

// clang-format off
// 日志等级定义
#define LOG_LEVEL_DEBUG 0
#define LOG_LEVEL_INFO  1
#define LOG_LEVEL_WARN  2
#define LOG_LEVEL_ERROR 3

// 当前日志等级（可配置）
#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_LEVEL_DEBUG
#endif

// 是否启用日志输出（可配置）
#ifndef LOG_ENABLED
#define LOG_ENABLED 0
#endif

#define COLOR_RESET "\033[0m"
#define COLOR_DEBUG "\033[36m"  // 青色
#define COLOR_INFO  "\033[32m"  // 绿色
#define COLOR_WARN  "\033[33m"  // 黄色
#define COLOR_ERROR "\033[31m"  // 红色

static inline void log_dummy(char* module, ...){(void)module;}
// 日志宏定义
#if !defined(LOG_ENABLED) || !LOG_ENABLED
#define LOG_DEBUG(module, fmt, ...)                log_dummy(module, fmt, ##__VA_ARGS__)
#define LOG_INFO(module, fmt, ...)                 log_dummy(module, fmt, ##__VA_ARGS__)
#define LOG_WARN(module, fmt, ...)                 log_dummy(module, fmt, ##__VA_ARGS__)
#define LOG_ERROR(module, fmt, ...)                log_dummy(module, fmt, ##__VA_ARGS__)
#define LOG_HEXDUMP_DEBUG(module, label, buf, len) log_dummy(module, label, buf, len)
#define LOG_HEXDUMP_INFO(module, label, buf, len)  log_dummy(module, label, buf, len)
#define LOG_HEXDUMP_WARN(module, label, buf, len)  log_dummy(module, label, buf, len)
#define LOG_HEXDUMP_ERROR(module, label, buf, len) log_dummy(module, label, buf, len)
#else
void log_message(const char *level, const char *color, const char *module, const char *fmt, ...);
void log_hexdump(const char *level, const char *color, const char *module, const char *label, const void *buf, size_t len);
#if LOG_LEVEL <= LOG_LEVEL_DEBUG
#define LOG_DEBUG(module, fmt, ...)                log_message("DEBUG", COLOR_DEBUG, module, fmt, ##__VA_ARGS__)
#define LOG_HEXDUMP_DEBUG(module, label, buf, len) log_hexdump("DEBUG", COLOR_DEBUG, module, label, buf, len)
#else
#define LOG_DEBUG(module, fmt, ...)                log_dummy(module, fmt, ##__VA_ARGS__)
#define LOG_HEXDUMP_DEBUG(module, label, buf, len) log_dummy(module, lable, buf, len)
#endif

#if LOG_LEVEL <= LOG_LEVEL_INFO
#define LOG_INFO(module, fmt, ...)                log_message("INFO", COLOR_INFO, module, fmt, ##__VA_ARGS__)
#define LOG_HEXDUMP_INFO(module, label, buf, len) log_hexdump("INFO", COLOR_INFO, module, label, buf, len)
#else
#define LOG_INFO(module, fmt, ...)                log_dummy(module, fmt, ##__VA_ARGS__)
#define LOG_HEXDUMP_INFO(module, label, buf, len) log_dummy(module, lable, buf, len)
#endif

#if LOG_LEVEL <= LOG_LEVEL_WARN
#define LOG_WARN(module, fmt, ...)                log_message("WARN", COLOR_WARN, module, fmt, ##__VA_ARGS__)
#define LOG_HEXDUMP_WARN(module, label, buf, len) log_hexdump("WARN", COLOR_WARN, module, label, buf, len)
#else
#define LOG_WARN(module, fmt, ...)                log_dummy(module, fmt, ##__VA_ARGS__)
#define LOG_HEXDUMP_WARN(module, label, buf, len) log_dummy(module, lable, buf, len)
#endif

#if LOG_LEVEL <= LOG_LEVEL_ERROR
#define LOG_ERROR(module, fmt, ...) \
  log_message("ERROR", COLOR_ERROR, module, fmt " [File: %s, Line: %d]", ##__VA_ARGS__, __FILE__, __LINE__)
#define LOG_HEXDUMP_ERROR(module, label, buf, len) log_hexdump("ERROR", COLOR_ERROR, module, label, buf, len)
#else
#define LOG_ERROR(module, fmt, ...)                log_dummy(module, fmt, ##__VA_ARGS__)
#define LOG_HEXDUMP_ERROR(module, label, buf, len) log_dummy(module, lable, buf, len)
#endif
#endif

#endif  // __LOG_H__
