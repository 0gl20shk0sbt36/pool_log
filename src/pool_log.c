/**
 * @file pool_log.c
 * @brief 独立日志模块 — 实现
 *
 * 纯 C99，依赖 snprintf（嵌入式工具链普遍支持）。
 * 用户须提供 pool_log_tick_now() 和 pool_log_write() 两个函数。
 */

#include "pool_log.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

/*===========================================================================
 * 用户接口的弱符号默认实现
 *
 * 若用户未在自己的代码中提供更强的定义，将使用下面的空实现。
 * 用户可在主程序中以普通（强符号）函数覆盖任意一个。
 *===========================================================================*/

/** @brief 滴答计数的弱实现：始终返回 0 */
__attribute__((weak))
uint32_t pool_log_tick_now(void)
{
    return 0;
}

/** @brief 日志输出的弱实现：丢弃输出 */
__attribute__((weak))
void pool_log_write(const char *msg)
{
    (void)msg;
}

/*===========================================================================
 * 全局上下文 — pool_log_ctx
 *
 * 日志插件的 hook 函数通过别名 &logging_ctx 引用此变量，
 * 由 plugins/logging/plugin.h 中的 #define 建立映射。
 *===========================================================================*/

pool_log_ctx_t pool_log_ctx = {
    .level     = POOL_LOG_INFO,    /* 默认 INFO 级别 */
    .last_tick = 0,
    .first     = true
};

/*===========================================================================
 * 内部常量
 *===========================================================================*/

/** 单条日志消息最大长度（含时间戳和格式头） */
#define LOG_MSG_MAX  128

/*===========================================================================
 * 内部工具
 *===========================================================================*/

/** @brief 日志等级名称查找表 */
static const char *k_level_names[] = {
    "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "NONE"
};

/*===========================================================================
 * 公共 API 实现
 *===========================================================================*/

void pool_log(uint8_t level, const char *fmt, ...)
{
    if (level < pool_log_ctx.level) return;
    if (pool_log_ctx.level >= POOL_LOG_NONE) return;

    /* 获取时间间隔 */
    uint32_t now   = pool_log_tick_now();
    uint32_t delta;
    if (pool_log_ctx.first) {
        delta = 0;
        pool_log_ctx.first = false;
    } else {
        delta = now - pool_log_ctx.last_tick;
    }
    pool_log_ctx.last_tick = now;

    /* 格式化前缀 */
    char buf[LOG_MSG_MAX];
    int off = snprintf(buf, sizeof(buf), "[%s] +%u | ",
                       k_level_names[level], (unsigned)delta);
    if (off < 0 || (size_t)off >= sizeof(buf)) return; /* 溢出 */

    /* 追加消息体 */
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf + off, sizeof(buf) - (size_t)off, fmt, ap);
    va_end(ap);

    /* 确保 NUL 结尾 */
    buf[sizeof(buf) - 1] = '\0';

    pool_log_write(buf);
}

void pool_log_set_level(uint8_t level)
{
    pool_log_ctx.level = level;
}

uint8_t pool_log_get_level(void)
{
    return pool_log_ctx.level;
}

const char *pool_log_level_name(uint8_t level)
{
    if (level > POOL_LOG_NONE) return "?";
    return k_level_names[level];
}
