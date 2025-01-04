#include "khg_phy/core/phy_pool.h"
#include "khg_utl/error_func.h"
#include <stdlib.h>
#include <string.h>

phy_memory_pool *phy_memory_pool_new(unsigned int chunk_size, unsigned int initial_num_chunks) {
    phy_memory_pool *pool = malloc(sizeof(phy_memory_pool));
    if (!pool) {
      utl_error_func("Failed to allocate memory", utl_user_defined_data);
    }
    pool->current_size = 0;
    pool->chunk_size = chunk_size;
    pool->pool_size = chunk_size * initial_num_chunks;
    pool->pool = malloc(pool->pool_size);
    if (!pool->pool) {
      utl_error_func("Failed to allocate memory", utl_user_defined_data);
    }
    return pool;
}

void phy_memory_pool_free(phy_memory_pool *pool) {
  free(pool->pool);
  free(pool);
}

int phy_memory_pool_add(phy_memory_pool *pool, void *chunk) {
  if (pool->current_size * pool->chunk_size >= pool->pool_size) {
    unsigned int new_pool_size = pool->pool_size * 2;
    void *new_pool = realloc(pool->pool, new_pool_size);
    if (!new_pool) {
      utl_error_func("Failed to allocate memory", utl_user_defined_data);
    }
    pool->pool = new_pool;
    pool->pool_size = new_pool_size;
  }
  memcpy((char *)pool->pool + (pool->current_size++) * pool->chunk_size, chunk, pool->chunk_size);
  return 0;
}

void phy_memory_pool_clear(phy_memory_pool *pool) {
  pool->current_size = 0;
}

