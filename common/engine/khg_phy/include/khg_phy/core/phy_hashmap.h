#pragma once

#include <stdbool.h>

typedef struct {
  unsigned long hash: 48;
  unsigned long dib: 16;
} phy_hashmap_bucket;

typedef struct {
  unsigned int size;
  unsigned int cap;
  unsigned long (*hash_func)(void *item);
  unsigned int count;
  bool oom;
  unsigned int z_buckets;
  unsigned int n_buckets;
  unsigned int mask;
  unsigned int grow_at;
  unsigned int shrink_at;
  unsigned char grow_power;
  void *buckets;
  void *spare;
  void *edata;
} phy_hashmap;

phy_hashmap *phy_hashmap_new(unsigned int item_size, unsigned int cap, unsigned long (*hash_func)(void *item));

void phy_hashmap_free(phy_hashmap *hashmap);
void phy_hashmap_clear(phy_hashmap *hashmap);

void *phy_hashmap_get(phy_hashmap *map, void *item);
void *phy_hashmap_set(phy_hashmap *map, void *item);

void *phy_hashmap_remove(phy_hashmap *hashmap, void *key);

bool phy_hashmap_iter(phy_hashmap *hashmap, unsigned int *index, void **item);

