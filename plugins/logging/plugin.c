/**
 * @file plugin.c
 * @brief logging 插件 — 薄适配层实现
 *
 * 所有 hook 函数的实现均委托给独立模块 pool_log。
 * 核心日志逻辑（等级过滤、时间戳、格式化、输出）在 pool_log.c 中。
 */

#include "plugin.h"
#include "pool_hooks.h"

/*===========================================================================
 * Hook 实现
 *===========================================================================*/

/* --- init --- */

void logging_post_init(void *ctx, pool_cfg_t *cfg)
{
    (void)ctx;
    pool_log(POOL_LOG_INFO,
             "init ok: page_size=%u page_count=%u handle_count=%u meta=%uB data=%uB",
             (unsigned)cfg->page_size,
             (unsigned)cfg->page_count,
             (unsigned)cfg->handle_count,
             (unsigned)cfg->metadata_size,
             (unsigned)((size_t)cfg->page_count * cfg->page_size));
}

/* --- alloc --- */

void logging_post_alloc(void *ctx, pool_cfg_t *cfg, pool_owner_t *owner,
                         uint32_t start, uint32_t count, uint32_t handle)
{
    (void)ctx;
    (void)cfg;
    pool_log(POOL_LOG_INFO,
             "alloc: owner=%u pages=%u start=%u handle=0x%08x",
             (unsigned)owner->owner_id, (unsigned)count,
             (unsigned)start, (unsigned)handle);
}

/* --- free --- */

void logging_pre_free(void *ctx, pool_cfg_t *cfg, pool_owner_t *owner, uint32_t handle)
{
    (void)ctx;
    /* 读取句柄信息以输出释放详情 */
    uint32_t idx = handle & cfg->handle_index_mask;
    pool_handle_entry_t *e = &cfg->handle_table[idx];
    pool_log(POOL_LOG_DEBUG,
             "free: owner=%u handle=0x%08x pages=%u start=%u",
             (unsigned)owner->owner_id, (unsigned)handle,
             (unsigned)e->page_count, (unsigned)e->page_start);
}

void logging_post_free(void *ctx, pool_cfg_t *cfg, pool_owner_t *owner, uint32_t handle)
{
    (void)ctx;
    (void)cfg;
    pool_log(POOL_LOG_DEBUG,
             "freed: owner=%u handle=0x%08x",
             (unsigned)owner->owner_id, (unsigned)handle);
}

/* --- lock --- */

void logging_pre_lock(void *ctx, pool_cfg_t *cfg, pool_owner_t *owner, uint32_t handle)
{
    (void)ctx;
    (void)cfg;
    pool_log(POOL_LOG_TRACE,
             "locking: owner=%u handle=0x%08x",
             (unsigned)owner->owner_id, (unsigned)handle);
}

void logging_post_lock(void *ctx, pool_cfg_t *cfg, pool_owner_t *owner,
                        uint32_t handle, void *addr)
{
    (void)ctx;
    (void)cfg;
    /* 地址差 = 数据区内的偏移量 */
    uint32_t offset = (uint32_t)((uintptr_t)addr - (uintptr_t)cfg->data_base);
    pool_log(POOL_LOG_DEBUG,
             "locked: owner=%u handle=0x%08x offset=+%u",
             (unsigned)owner->owner_id, (unsigned)handle, (unsigned)offset);
}

/* --- unlock --- */

void logging_pre_unlock(void *ctx, pool_cfg_t *cfg, pool_owner_t *owner, uint32_t handle)
{
    (void)ctx;
    (void)cfg;
    pool_log(POOL_LOG_TRACE,
             "unlocking: owner=%u handle=0x%08x",
             (unsigned)owner->owner_id, (unsigned)handle);
}

void logging_post_unlock(void *ctx, pool_cfg_t *cfg, pool_owner_t *owner, uint32_t handle)
{
    (void)ctx;
    (void)cfg;
    pool_log(POOL_LOG_TRACE,
             "unlocked: owner=%u handle=0x%08x",
             (unsigned)owner->owner_id, (unsigned)handle);
}

/* --- resize --- */

void logging_pre_resize(void *ctx, pool_cfg_t *cfg, pool_owner_t *owner,
                         uint32_t handle, uint32_t old_pages, uint32_t new_pages)
{
    (void)ctx;
    (void)cfg;
    pool_log(POOL_LOG_DEBUG,
             "resizing: owner=%u handle=0x%08x %u->%u pages",
             (unsigned)owner->owner_id, (unsigned)handle,
             (unsigned)old_pages, (unsigned)new_pages);
}

void logging_post_resize(void *ctx, pool_cfg_t *cfg, pool_owner_t *owner,
                          uint32_t handle, uint32_t old_pages, uint32_t new_pages)
{
    (void)ctx;
    (void)cfg;
    if (old_pages == new_pages) return; /* no-op */
    pool_log(POOL_LOG_INFO,
             "resized: owner=%u handle=0x%08x %u->%u pages",
             (unsigned)owner->owner_id, (unsigned)handle,
             (unsigned)old_pages, (unsigned)new_pages);
}

/* --- defrag --- */

void logging_pre_defrag(void *ctx, pool_cfg_t *cfg)
{
    (void)ctx;
    (void)cfg;
    pool_log(POOL_LOG_INFO, "defrag start");
}

void logging_defrag_move(void *ctx, pool_cfg_t *cfg,
                          uint32_t src_page, uint32_t dst_page,
                          uint32_t count, uint32_t handle_idx)
{
    (void)ctx;
    (void)cfg;
    pool_log(POOL_LOG_DEBUG,
             "defrag move: handle_idx=%u src=%u->%u pages=%u",
             (unsigned)handle_idx, (unsigned)src_page,
             (unsigned)dst_page, (unsigned)count);
}

void logging_post_defrag(void *ctx, pool_cfg_t *cfg)
{
    (void)ctx;
    /* 统计剩余空闲页 */
    uint32_t free_pages = 0;
    for (uint32_t i = 0; i < cfg->page_count; i++) {
        uint32_t byte_idx = i >> 3u;
        uint32_t bit_mask = (uint8_t)(1u << (i & 7u));
        if ((cfg->bitmap[byte_idx] & bit_mask) == 0) {
            free_pages++;
        }
    }
    pool_log(POOL_LOG_INFO, "defrag done: free=%u pages",
             (unsigned)free_pages);
}

/* --- free_all --- */

void logging_pre_free_all(void *ctx, pool_cfg_t *cfg, pool_owner_t *owner, uint32_t forced)
{
    (void)ctx;
    (void)cfg;
    pool_log(POOL_LOG_INFO,
             "free_all: owner=%u forced=%u",
             (unsigned)owner->owner_id, (unsigned)forced);
}

void logging_post_free_all(void *ctx, pool_cfg_t *cfg, pool_owner_t *owner, uint32_t forced)
{
    (void)ctx;
    (void)cfg;
    (void)forced;
    pool_log(POOL_LOG_INFO,
             "free_all done: owner=%u",
             (unsigned)owner->owner_id);
}
