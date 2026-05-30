add_executable(test_pool_log
    test_pool_log.c
    ../src/pool_log.c
)
target_include_directories(test_pool_log
    PRIVATE ../include
)

add_test(NAME pool_log_test COMMAND test_pool_log)
