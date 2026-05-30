cmake_minimum_required(VERSION 3.10)
project(pool_log C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# ================================================================
# 静态库
# ================================================================

add_library(pool_log STATIC
    src/pool_log.c
)
target_include_directories(pool_log
    PUBLIC include
)

# ================================================================
# 测试（可选）
# ================================================================
option(POOL_LOG_BUILD_TEST "Build test executable" OFF)

if(POOL_LOG_BUILD_TEST)
    enable_testing()
    add_subdirectory(test)
endif()
