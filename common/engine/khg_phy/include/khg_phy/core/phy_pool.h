#pragma once

typedef struct phy_memory_pool {
  unsigned int chunk_size;
  unsigned int pool_size;
  unsigned int current_size;
  void *pool;
} phy_memory_pool;

phy_memory_pool *phy_memory_pool_new(unsigned int chunk_size, unsigned int initial_num_chunks);
void phy_memory_pool_free(phy_memory_pool *pool);

int phy_memory_pool_add(phy_memory_pool *pool, void *chunk);
void phy_memory_pool_clear(phy_memory_pool *pool);

