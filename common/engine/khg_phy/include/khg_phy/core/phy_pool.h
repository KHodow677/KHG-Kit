#pragma once

#include <stdlib.h>

typedef struct phy_memory_pool {
  size_t chunk_size;
  size_t pool_size;
  size_t current_size;
  void *pool;
} phy_memory_pool;

phy_memory_pool *phy_memory_pool_new(size_t chunk_size, size_t initial_num_chunks);
void phy_memory_pool_free(phy_memory_pool *pool);

int phy_memory_pool_add(phy_memory_pool *pool, void *chunk);
void phy_memory_pool_clear(phy_memory_pool *pool);

