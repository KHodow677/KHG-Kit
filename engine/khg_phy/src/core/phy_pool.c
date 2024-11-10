/*

  This file is a part of the Nova Physics Engine
  project and distributed under the MIT license.

  Copyright © Kadir Aksoy
  https://github.com/kadir014/nova-physics

*/

#include "khg_phy/core/phy_pool.h"
#include "khg_phy/internal.h"
#include "khg_utl/error_func.h"


/**
 * @file core/pool.c
 * 
 * @brief Fixed-size memory pool implementation.
 */


phy_memory_pool *phy_memory_pool_new(size_t chunk_size, size_t initial_num_chunks) {
    phy_memory_pool *pool = NV_NEW(phy_memory_pool);
    if (!pool) {
      utl_error_func("Failed to allocate memory", utl_user_defined_data);
    }

    pool->current_size = 0;
    pool->chunk_size = chunk_size;
    pool->pool_size = chunk_size * initial_num_chunks;
    pool->pool = NV_MALLOC(pool->pool_size);
    if (!pool->pool) {
      utl_error_func("Failed to allocate memory", utl_user_defined_data);
    }

    return pool;
}

void phy_memory_pool_free(phy_memory_pool *pool) {
    NV_FREE(pool->pool);
    NV_FREE(pool);
}

int phy_memory_pool_add(phy_memory_pool *pool, void *chunk) {
    // Expand the bool if necessary
    if (pool->current_size * pool->chunk_size >= pool->pool_size) {
        size_t new_pool_size = pool->pool_size * 2;
        void *new_pool = NV_REALLOC(pool->pool, new_pool_size);
        if (!new_pool) {
          utl_error_func("Failed to allocate memory", utl_user_defined_data);
        }

        pool->pool = new_pool;
        pool->pool_size = new_pool_size;
    }

    memcpy(
        (char *)pool->pool + (pool->current_size++) * pool->chunk_size,
        chunk,
        pool->chunk_size
    );

    return 0;
}

void phy_memory_pool_clear(phy_memory_pool *pool) {
    pool->current_size = 0;
}
