/**
 * @file test_pool_log.c
 * @brief pool_log 独立模块测试
 */

#include "pool_log.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

/* 测试用输出缓冲区 */
static char last_msg[256];
static uint32_t mock_tick;

uint32_t pool_log_tick_now(void) { return mock_tick; }

void pool_log_write(const char *msg)
{
    strncpy(last_msg, msg, sizeof(last_msg) - 1);
    last_msg[sizeof(last_msg) - 1] = '\0';
}

static void clear_buffer(void)
{
    last_msg[0] = '\0';
}

static int run_tests(void)
{
    unsigned fail = 0;

    /* ── 1. 默认等级是 INFO ── */
    assert(pool_log_get_level() == POOL_LOG_INFO);
    printf("PASS: default level is INFO\n");

    /* ── 2. 设置等级 ── */
    pool_log_set_level(POOL_LOG_WARN);
    assert(pool_log_get_level() == POOL_LOG_WARN);
    pool_log_set_level(POOL_LOG_INFO);
    printf("PASS: set/get level\n");

    /* ── 3. level_name ── */
    assert(strcmp(pool_log_level_name(POOL_LOG_TRACE), "TRACE") == 0);
    assert(strcmp(pool_log_level_name(POOL_LOG_INFO),  "INFO") == 0);
    assert(strcmp(pool_log_level_name(POOL_LOG_NONE),  "NONE") == 0);
    assert(strcmp(pool_log_level_name(99), "?") == 0);
    printf("PASS: level_name\n");

    /* ── 4. 日志输出（低于等级应被过滤）─ */
    mock_tick = 0;
    pool_log_set_level(POOL_LOG_WARN);

    clear_buffer();
    pool_log(POOL_LOG_INFO, "should be filtered");
    assert(last_msg[0] == '\0');
    printf("PASS: below-level filtering\n");

    pool_log_set_level(POOL_LOG_INFO);
    clear_buffer();
    pool_log(POOL_LOG_INFO, "hello %s", "world");
    assert(strstr(last_msg, "[INFO]") != NULL);
    assert(strstr(last_msg, "hello world") != NULL);
    assert(strstr(last_msg, "+0") != NULL);
    printf("PASS: basic output format: %s\n", last_msg);

    /* ── 5. 时间间隔 ── */
    mock_tick = 100;
    clear_buffer();
    pool_log(POOL_LOG_INFO, "second msg");
    assert(strstr(last_msg, "+100") != NULL);
    printf("PASS: delta tick: %s\n", last_msg);

    /* ── 6. NONE 等级应过滤全部 ── */
    pool_log_set_level(POOL_LOG_NONE);
    clear_buffer();
    pool_log(POOL_LOG_ERROR, "urgent!");
    assert(last_msg[0] == '\0');
    printf("PASS: NONE level suppresses all\n");

    /* ── 7. TRACE 等级应输出全部 ── */
    pool_log_set_level(POOL_LOG_TRACE);
    clear_buffer();
    pool_log(POOL_LOG_TRACE, "trace msg");
    assert(strstr(last_msg, "[TRACE]") != NULL);
    printf("PASS: TRACE level: %s\n", last_msg);

    /* ── 8. 设置回默认等级 ── */
    pool_log_set_level(POOL_LOG_INFO);

    return (int)fail;
}

int main(void)
{
    printf("=== pool_log standalone tests ===\n\n");
    int f = run_tests();
    printf("\n--- %u failures ---\n", f);
    return f;
}
