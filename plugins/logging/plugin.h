/**
 * @file plugin.h
 * @brief logging 插件 — 薄适配层
 *
 * 此文件是 configure.py 插件系统的适配头文件。
 * 核心日志功能由独立模块 pool_log 提供；
 * 此头文件只做两件事：
 *   1. #include "pool.h" / "pool_log.h" — 复用池类型和日志 API
 *   2. 声明所有 hook 函数 — 供 configure.py 自动发现并生成调用链
 *
 * configure.py 的命名约定要求：
 *   - 函数名: {插件名}_{hook名}  (e.g. logging_post_init)
 *   - 上下文: &{插件名}_ctx     (e.g. &logging_ctx)
 *
 * 为满足此约定，pool_log_ctx 通过 #define 映射为 logging_ctx，
 * 这样 configure.py 生成的宏定义可以透明地引用 pool_log 的状态。
 *
 * ## 函数命名: logging_{hook名}
 * 首参: void *ctx (→ &logging_ctx / &pool_log_ctx)
 * 其余参数: 与 pool_hooks.h 中宏参数一致
 */

#ifndef POOL_PLUGIN_LOGGING_H
#define POOL_PLUGIN_LOGGING_H

#include "pool.h"
#include "pool_log.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
 * 插件上下文别名 — configure.py 生成 &logging_ctx
 *===========================================================================*/

#define logging_ctx   pool_log_ctx

/*===========================================================================
 * Hook 函数声明 — 由 configure.py 自动发现并生成调用链
 *===========================================================================*/

/* --- init --- */
void logging_post_init(void *ctx, pool_cfg_t *cfg);

/* --- alloc --- */
void logging_post_alloc(void *ctx, pool_cfg_t *cfg, pool_owner_t *owner,
                         uint32_t start, uint32_t count, uint32_t handle);

/* --- free --- */
void logging_pre_free(void *ctx, pool_cfg_t *cfg, pool_owner_t *owner, uint32_t handle);
void logging_post_free(void *ctx, pool_cfg_t *cfg, pool_owner_t *owner, uint32_t handle);

/* --- lock --- */
void logging_pre_lock(void *ctx, pool_cfg_t *cfg, pool_owner_t *owner, uint32_t handle);
void logging_post_lock(void *ctx, pool_cfg_t *cfg, pool_owner_t *owner,
                        uint32_t handle, void *addr);

/* --- unlock --- */
void logging_pre_unlock(void *ctx, pool_cfg_t *cfg, pool_owner_t *owner, uint32_t handle);
void logging_post_unlock(void *ctx, pool_cfg_t *cfg, pool_owner_t *owner, uint32_t handle);

/* --- resize --- */
void logging_pre_resize(void *ctx, pool_cfg_t *cfg, pool_owner_t *owner,
                         uint32_t handle, uint32_t old_pages, uint32_t new_pages);
void logging_post_resize(void *ctx, pool_cfg_t *cfg, pool_owner_t *owner,
                          uint32_t handle, uint32_t old_pages, uint32_t new_pages);

/* --- defrag --- */
void logging_pre_defrag(void *ctx, pool_cfg_t *cfg);
void logging_defrag_move(void *ctx, pool_cfg_t *cfg,
                          uint32_t src_page, uint32_t dst_page,
                          uint32_t count, uint32_t handle_idx);
void logging_post_defrag(void *ctx, pool_cfg_t *cfg);

/* --- free_all --- */
void logging_pre_free_all(void *ctx, pool_cfg_t *cfg, pool_owner_t *owner, uint32_t forced);
void logging_post_free_all(void *ctx, pool_cfg_t *cfg, pool_owner_t *owner, uint32_t forced);

#ifdef __cplusplus
}
#endif

#endif /* POOL_PLUGIN_LOGGING_H */
