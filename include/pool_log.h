/**
 * @file pool_log.h
 * @brief 独立日志模块 — 页内存池的日志输出基础设施
 *
 * 纯 C99，零系统调用依赖，适用于单片机裸机环境。
 * 用户须提供 pool_log_tick_now() 和 pool_log_write() 两个函数。
 *
 * ## 使用方式
 *
 * ```c
 * #include "pool_log.h"
 *
 * // 必须实现的回调
 * uint32_t pool_log_tick_now(void) { return sys_tick_ms; }
 * void     pool_log_write(const char *msg) { uart_puts(msg); }
 *
 * // 手动日志
 * pool_log(POOL_LOG_INFO, "sensor=%d val=%d", id, val);
 *
 * // 调节等级
 * pool_log_set_level(POOL_LOG_WARN);
 * ```
 *
 * ## 日志格式
 *
 * ```
 * [LEVEL] +Δtick | message
 * ```
 *
 * - `LEVEL`: TRACE / DEBUG / INFO / WARN / ERROR
 * - `Δtick`: 距上一条日志的滴答差值（首条显示 +0）
 * - `message`: 操作描述及参数
 *
 * ## 日志等级
 *
 * | 等级    | 值 | 说明                            |
 * |---------|----|---------------------------------|
 * | TRACE   | 0  | 最细粒度追踪                    |
 * | DEBUG   | 1  | 调试信息                        |
 * | INFO    | 2  | 常规运行信息（默认）             |
 * | WARN    | 3  | 警告                            |
 * | ERROR   | 4  | 严重错误                        |
 * | NONE    | 5  | 关闭全部日志输出                 |
 */

#ifndef POOL_LOG_H
#define POOL_LOG_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
 * 日志等级
 *===========================================================================*/

typedef enum {
    POOL_LOG_TRACE = 0,
    POOL_LOG_DEBUG = 1,
    POOL_LOG_INFO  = 2,
    POOL_LOG_WARN  = 3,
    POOL_LOG_ERROR = 4,
    POOL_LOG_NONE  = 5
} pool_log_level_t;

/*===========================================================================
 * 用户必须提供的外部函数（弱符号默认实现）
 *===========================================================================*/

/**
 * @brief 获取当前滴答计数。
 *
 * 32 位无符号整数，须单调递增。用于计算日志条目之间的时间间隔。
 * 绝对数值无意义 —— 仅差值重要。
 *
 * 弱符号默认实现始终返回 0；用户可在主程序中覆盖。
 */
extern uint32_t pool_log_tick_now(void);

/**
 * @brief 输出一条完整的日志行。
 *
 * msg 为 NUL 结尾字符串，不含换行符。
 *
 * 弱符号默认实现为空操作；用户可在主程序中覆盖。
 */
extern void pool_log_write(const char *msg);

/*===========================================================================
 * 日志上下文（内部状态，用户通常不直接访问）
 *===========================================================================*/

typedef struct {
    uint8_t  level;       /**< 当前日志等级 */
    uint32_t last_tick;   /**< 上一条日志的滴答值 */
    bool     first;       /**< 首次输出标记 */
} pool_log_ctx_t;

/** @brief 全局日志上下文（声明在 pool_log.c） */
extern pool_log_ctx_t pool_log_ctx;

/*===========================================================================
 * 公共 API
 *===========================================================================*/

/**
 * @brief 输出一条日志。
 *
 * @param level 日志等级（低于全局等级时不会输出）
 * @param fmt   printf 风格格式串
 * @param ...   格式参数
 */
void pool_log(uint8_t level, const char *fmt, ...);

/** @brief 动态设置全局日志等级 */
void pool_log_set_level(uint8_t level);

/** @brief 获取当前全局日志等级 */
uint8_t pool_log_get_level(void);

/** @brief 返回等级名称字符串（"TRACE" / "DEBUG" / ...） */
const char *pool_log_level_name(uint8_t level);

#ifdef __cplusplus
}
#endif

#endif /* POOL_LOG_H */
