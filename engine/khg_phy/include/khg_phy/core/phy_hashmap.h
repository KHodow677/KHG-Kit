#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct {
  uint64_t hash: 48;
  uint64_t dib: 16;
} phy_hashmap_bucket;

typedef struct {
  size_t elsize;
  size_t cap;
  uint64_t (*hash_func)(void *item);
  size_t count;
  bool oom;
  size_t bucketsz;
  size_t nbuckets;
  size_t mask;
  size_t growat;
  size_t shrinkat;
  uint8_t growpower;
  void *buckets;
  void *spare;
  void *edata;
} phy_hashmap;

phy_hashmap *phy_hashmap_new(size_t item_size, size_t cap, uint64_t (*hash_func)(void *item));

void phy_hashmap_free(phy_hashmap *hashmap);
void phy_hashmap_clear(phy_hashmap *hashmap);

void *phy_hashmap_get(phy_hashmap *map, void *item);
void *phy_hashmap_set(phy_hashmap *map, void *item);

void *phy_hashmap_remove(phy_hashmap *hashmap, void *key);

bool phy_hashmap_iter(phy_hashmap *hashmap, size_t *index, void **item);

