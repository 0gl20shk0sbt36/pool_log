# pool_log — Embedded C Log Module

[English](README.md) | [中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![C99](https://img.shields.io/badge/C-C99-blue)]()
[![Tests](https://img.shields.io/badge/tests-8%20passed-brightgreen)]()

A lightweight, bare-metal-friendly logging library in pure C99. Zero system calls — no malloc, no file I/O, no threading. Designed for embedded MCUs where every resource counts.

## Features

- **C99 only** — depends solely on `snprintf` (widely available in embedded toolchains)
- **Zero system calls** — no `malloc`, `FILE*`, or threads; works on bare metal
- **Weak default stubs** — links out of the box; provide two callbacks to activate
- **Level filtering** — TRACE / DEBUG / INFO / WARN / ERROR / NONE
- **Delta tick timing** — each entry shows elapsed ticks since the previous log
- **Standalone** — no dependency on MemoryPool or any plugin system

## Quick Start

```c
#include "pool_log.h"

/* User-provided callbacks */
uint32_t pool_log_tick_now(void) { return systick_ms; }
void     pool_log_write(const char *msg) { uart_puts(msg); }

/* Use the logging API */
pool_log(POOL_LOG_INFO, "sensor=%d val=%d", id, value);
pool_log_set_level(POOL_LOG_WARN);  /* suppress below WARN */
```

## Log Format

```
[INFO]  +0   | init ok: page_size=256 page_count=64 ...
[DEBUG] +142 | locked: owner=128 handle=0x00010001 offset=+0
[INFO]  +35  | defrag done: free=12 pages
```

Each line: `[LEVEL] +Δtick | message`

- **LEVEL** — TRACE / DEBUG / INFO / WARN / ERROR
- **+Δtick** — elapsed ticks since the previous log entry (0 on first entry)
- **message** — formatted log content

## Build

```sh
# Library only
cmake -B build
cmake --build build
# Output: build/libpool_log.a

# Library + tests
cmake -B build -DPOOL_LOG_BUILD_TEST=ON
cmake --build build
./build/test/test_pool_log
```

## Integration (CMake)

```cmake
add_subdirectory(path/to/pool_log)
target_link_libraries(your_target PRIVATE pool_log)
target_include_directories(your_target PRIVATE path/to/pool_log/include)
```

## As a MemoryPool Plugin

pool_log ships a plugin adapter that hooks into MemoryPool's event system.
See [MemoryPool Plugin System](https://github.com/your-org/MemoryPool) for details.

Directory layout in MemoryPool:

```
lib/pool_log/                          ← git submodule
├── src/pool_log.c                     ← core library
├── include/pool_log.h                 ← public API
└── plugins/logging/                   ← MemoryPool adapter (plugin.h, plugin.c, plugin.cmake)
```

Enable via `pool_plugin_config.cmake`:
```cmake
option(POOL_USE_LOGGING "Use logging plugin" ON)
```

## License

MIT License
