# pool_log — 嵌入式 C 日志模块

纯 C99 日志库，零系统调用依赖，适用于单片机裸机环境。

## 特点

- **纯 C99** — 仅依赖 `snprintf`（嵌入式工具链普遍支持）
- **零系统调用** — 无 malloc / FILE I/O / threading，裸机可用
- **弱符号默认实现** — 链接即可编译，用户提供两个回调即可运行
- **等级过滤** — TRACE / DEBUG / INFO / WARN / ERROR / NONE
- **时间间隔** — 每条日志自动附带距上一条的滴答差值

## 使用

```c
#include "pool_log.h"

/* 必须实现的回调 */
uint32_t pool_log_tick_now(void) { return systick_ms; }
void     pool_log_write(const char *msg) { uart_puts(msg); }

/* 输出日志 */
pool_log(POOL_LOG_INFO, "sensor=%d val=%d", id, value);

/* 调节等级 */
pool_log_set_level(POOL_LOG_WARN);
```

## 构建

```bash
cmake -B build
cmake --build build
```

## 测试

```bash
cmake -B build -DPOOL_LOG_BUILD_TEST=ON
cmake --build build
./build/test/test_pool_log
```

## 日志格式

```
[INFO] +0 | sensor=3 val=42
[INFO] +100 | second message
```

- `LEVEL` — TRACE / DEBUG / INFO / WARN / ERROR
- `+Δtick` — 距上一条日志的滴答差值
- `message` — 日志内容
