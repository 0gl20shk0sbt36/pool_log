# pool_log — 嵌入式 C 日志模块

[English](README.md) | [中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![C99](https://img.shields.io/badge/C-C99-blue)]()
[![Tests](https://img.shields.io/badge/tests-8%20passed-brightgreen)]()

纯 C99 日志库，零系统调用依赖，适用于单片机裸机环境。无 malloc、无文件 I/O、无线程，资源紧张时也能安心使用。

## 特点

- **纯 C99** — 仅依赖 `snprintf`（嵌入式工具链普遍支持）
- **零系统调用** — 无 malloc / FILE I/O / threading，裸机可用
- **弱符号默认实现** — 链接即可编译，提供两个回调即可运行
- **等级过滤** — TRACE / DEBUG / INFO / WARN / ERROR / NONE
- **时间间隔** — 每条日志自动附带距上一条的滴答差值
- **独立模块** — 不依赖 MemoryPool 或任何插件系统

## 快速上手

```c
#include "pool_log.h"

/* 用户必须提供的回调 */
uint32_t pool_log_tick_now(void) { return systick_ms; }
void     pool_log_write(const char *msg) { uart_puts(msg); }

/* 使用日志 API */
pool_log(POOL_LOG_INFO, "sensor=%d val=%d", id, value);
pool_log_set_level(POOL_LOG_WARN);  /* 屏蔽低于 WARN 的日志 */
```

## 日志格式

```
[INFO]  +0   | init ok: page_size=256 page_count=64 ...
[DEBUG] +142 | locked: owner=128 handle=0x00010001 offset=+0
[INFO]  +35  | defrag done: free=12 pages
```

每行格式：`[LEVEL] +Δtick | message`

- **LEVEL** — TRACE / DEBUG / INFO / WARN / ERROR
- **+Δtick** — 距上一条日志的滴答差值（首条为 0）
- **message** — 格式化日志内容

## 构建

```sh
# 仅编译库
cmake -B build
cmake --build build
# 产物: build/libpool_log.a

# 库 + 测试
cmake -B build -DPOOL_LOG_BUILD_TEST=ON
cmake --build build
./build/test/test_pool_log
```

## 集成 (CMake)

```cmake
add_subdirectory(path/to/pool_log)
target_link_libraries(your_target PRIVATE pool_log)
target_include_directories(your_target PRIVATE path/to/pool_log/include)
```

## 作为 MemoryPool 插件

pool_log 内置了适配层，可接入 MemoryPool 的插件事件系统。
详见 [MemoryPool 插件系统](https://github.com/0gl20shk0sbt36/MemoryPool)。

MemoryPool 中的目录布局：

```
lib/pool_log/                          ← git submodule
├── src/pool_log.c                     ← 核心库
├── include/pool_log.h                 ← 公共 API
└── plugins/logging/                   ← MemoryPool 适配层 (plugin.h, plugin.c, plugin.cmake)
```

通过 `pool_plugin_config.cmake` 启用：
```cmake
option(POOL_USE_LOGGING "Use logging plugin" ON)
```

## 许可

MIT License
