#include "khg_phy/core/phy_hashmap.h"
#include "khg_phy/core/phy_constants.h"
#include "khg_utl/error_func.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static inline phy_hashmap_bucket *phy_hashmap_get_bucket_at(phy_hashmap *map, unsigned int index) {
  return (phy_hashmap_bucket *)(((char *)map->buckets) + (map->z_buckets * index));
}

static inline void *phy_hashmap_get_bucket_item(phy_hashmap_bucket *entry) {
  return ((char *)entry) + sizeof(phy_hashmap_bucket);
}

static inline unsigned long phy_hashmap_clip(unsigned long hash) {
  return hash & 0xFFFFFFFFFFFF;
}

static inline bool phy_hashmap_resize(phy_hashmap *hashmap, unsigned int new_cap) {
  phy_hashmap *hashmap2 = phy_hashmap_new(hashmap->size, new_cap, hashmap->hash_func);
  if (!hashmap2) {
    return false;
  }
  for (unsigned int i = 0; i < hashmap->n_buckets; i++) {
    phy_hashmap_bucket *entry = phy_hashmap_get_bucket_at(hashmap, i);
    if (!entry->dib) {
      continue;
    }
    entry->dib = 1;
    unsigned int j = entry->hash & hashmap2->mask;
    while (true) {
      phy_hashmap_bucket *bucket = phy_hashmap_get_bucket_at(hashmap2, j);
      if (bucket->dib == 0) {
        memcpy(bucket, entry, hashmap->z_buckets);
        break;
      }
      if (bucket->dib < entry->dib) {
        memcpy(hashmap2->spare, bucket, hashmap->z_buckets);
        memcpy(bucket, entry, hashmap->z_buckets);
        memcpy(entry, hashmap2->spare, hashmap->z_buckets);
      }
      j = (j + 1) & hashmap2->mask;
      entry->dib += 1;
    }
  }
  free(hashmap->buckets);
  hashmap->buckets = hashmap2->buckets;
  hashmap->n_buckets = hashmap2->n_buckets;
  hashmap->mask = hashmap2->mask;
  hashmap->grow_at = hashmap2->grow_at;
  hashmap->shrink_at = hashmap2->shrink_at;
  free(hashmap2);
  return true;
}

phy_hashmap *phy_hashmap_new(unsigned int item_size, unsigned int cap, unsigned long (*hash_func)(void *item)) {
  unsigned int ncap = PHY_HASHMAP_CAPACITY;
  if (cap < ncap) {
    cap = ncap;
  }
  else {
    while (ncap < cap) {
      ncap *= 2;
    }
    cap = ncap;
  }
  unsigned int bucketsz = sizeof(phy_hashmap_bucket) + item_size;
  while (bucketsz & (sizeof(unsigned int *) - 1)) {
    bucketsz++;
  }
  unsigned int size = sizeof(phy_hashmap) + bucketsz * 2;
  phy_hashmap *hashmap = malloc(size);
  if (!hashmap) {
    utl_error_func("Failed to allocate memory", utl_user_defined_data);
  }
  hashmap->count = 0;
  hashmap->oom = false;
  hashmap->size = item_size;
  hashmap->hash_func = hash_func;
  hashmap->z_buckets = bucketsz;
  hashmap->spare = ((char*)hashmap) + sizeof(phy_hashmap);
  hashmap->edata = (char*)hashmap->spare + bucketsz;
  hashmap->cap = cap;
  hashmap->n_buckets = cap;
  hashmap->mask = hashmap->n_buckets - 1;
  hashmap->buckets = malloc(hashmap->z_buckets * hashmap->n_buckets);
  if (!hashmap->buckets) {
    free(hashmap);
    utl_error_func("Failed to allocate memory", utl_user_defined_data);
    return NULL;
  }
  memset(hashmap->buckets, 0, hashmap->z_buckets * hashmap->n_buckets);
  hashmap->grow_power = 1;
  hashmap->grow_at = (unsigned int)(hashmap->n_buckets * 0.6);
  hashmap->shrink_at = (unsigned int)(hashmap->n_buckets * 0.1);
  return hashmap;
}

void phy_hashmap_free(phy_hashmap *hashmap) {
  free(hashmap->buckets);
  free(hashmap);
}

void phy_hashmap_clear(phy_hashmap *hashmap) {
  hashmap->count = 0;
  if (hashmap->n_buckets != hashmap->cap) {
    void *new_buckets = malloc(hashmap->z_buckets*hashmap->cap);
    if (new_buckets) {
      free(hashmap->buckets);
      hashmap->buckets = new_buckets;
    }
    hashmap->n_buckets = hashmap->cap;
  }
  memset(hashmap->buckets, 0, hashmap->z_buckets*hashmap->n_buckets);
  hashmap->mask = hashmap->n_buckets - 1;
  hashmap->grow_at = (unsigned int)(hashmap->n_buckets * 0.75);
  hashmap->shrink_at = (unsigned int)(hashmap->n_buckets * 0.1);
}

void *phy_hashmap_set(phy_hashmap *hashmap, void *item) {
  unsigned long hash = phy_hashmap_clip(hashmap->hash_func(item));
  hash = phy_hashmap_clip(hash);
  hashmap->oom = false;
  if (hashmap->count == hashmap->grow_at) {
    if (!phy_hashmap_resize(hashmap, hashmap->n_buckets * (1<<hashmap->grow_power))) {
      hashmap->oom = true;
      return NULL;
    }
  }
  phy_hashmap_bucket *entry = hashmap->edata;
  entry->hash = hash;
  entry->dib = 1;
  void *eitem = phy_hashmap_get_bucket_item(entry);
  memcpy(eitem, item, hashmap->size);
  void *bitem;
  unsigned int i = entry->hash & hashmap->mask;
  while (true) {
    phy_hashmap_bucket *bucket = phy_hashmap_get_bucket_at(hashmap, i);
    if (bucket->dib == 0) {
        memcpy(bucket, entry, hashmap->z_buckets);
        hashmap->count++;
        return NULL;
    }
    bitem = phy_hashmap_get_bucket_item(bucket);
    if (entry->hash == bucket->hash) {
      memcpy(hashmap->spare, bitem, hashmap->size);
      memcpy(bitem, eitem, hashmap->size);
      return hashmap->spare;
    }
    if (bucket->dib < entry->dib) {
      memcpy(hashmap->spare, bucket, hashmap->z_buckets);
      memcpy(bucket, entry, hashmap->z_buckets);
      memcpy(entry, hashmap->spare, hashmap->z_buckets);
      eitem = phy_hashmap_get_bucket_item(entry);
    }
    i = (i + 1) & hashmap->mask;
    entry->dib += 1;
  }
}

void *phy_hashmap_get(phy_hashmap *hashmap, void *key) {
  unsigned long hash = phy_hashmap_clip(hashmap->hash_func(key));
  hash = phy_hashmap_clip(hash);
  unsigned int i = hash & hashmap->mask;
  while (true) {
    phy_hashmap_bucket *bucket = phy_hashmap_get_bucket_at(hashmap, i);
    if (!bucket->dib) {
      return NULL;
    }
    if (bucket->hash == hash) {
      void *bitem = phy_hashmap_get_bucket_item(bucket);
      if (bitem != NULL) {
        return bitem;
      }
    }
    i = (i + 1) & hashmap->mask;
  }
}

void *phy_hashmap_remove(phy_hashmap *hashmap, void *key) {
  unsigned long hash = phy_hashmap_clip(hashmap->hash_func(key));
  hash = phy_hashmap_clip(hash);
  hashmap->oom = false;
  unsigned int i = hash & hashmap->mask;
  while (true) {
    phy_hashmap_bucket *bucket = phy_hashmap_get_bucket_at(hashmap, i);
    if (!bucket->dib) {
      return NULL;
    }
    void *bitem = phy_hashmap_get_bucket_item(bucket);
    if (bucket->hash == hash) {
      memcpy(hashmap->spare, bitem, hashmap->size);
      bucket->dib = 0;
      while (true) {
        phy_hashmap_bucket *prev = bucket;
        i = (i + 1) & hashmap->mask;
        bucket = phy_hashmap_get_bucket_at(hashmap, i);
        if (bucket->dib <= 1) {
          prev->dib = 0;
          break;
        }
        memcpy(prev, bucket, hashmap->z_buckets);
        prev->dib--;
      }
      hashmap->count--;
      if (hashmap->n_buckets > hashmap->cap && hashmap->count <= hashmap->shrink_at) {
        phy_hashmap_resize(hashmap, hashmap->n_buckets / 2);
      }
      return hashmap->spare;
    }
    i = (i + 1) & hashmap->mask;
  }
}

bool phy_hashmap_iter(phy_hashmap *hashmap, unsigned int *index, void **item) {
  phy_hashmap_bucket *bucket;
  do {
    if (*index >= hashmap->n_buckets) {
      return false;
    }
    bucket = phy_hashmap_get_bucket_at(hashmap, *index);
    (*index)++;
  } while (!bucket->dib);
  *item = phy_hashmap_get_bucket_item(bucket);
  return true;
}

