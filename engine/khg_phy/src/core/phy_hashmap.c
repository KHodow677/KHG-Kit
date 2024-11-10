/*

  This file is a part of the Nova Physics Engine
  project and distributed under the MIT license.

  Copyright © Kadir Aksoy
  https://github.com/kadir014/nova-physics

*/

#include "khg_phy/core/phy_hashmap.h"
#include "khg_phy/core/phy_constants.h"
#include "khg_utl/error_func.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>


/**
 * @file core/hashmap.c
 * 
 * @brief Hash map implementation.
 * 
 * Thanks to @tidwall for their great hash map implementation that served as
 * a strong foundation for this one.
 * (https://github.com/tidwall/hashmap.c)
 */


static inline phy_hashmap_bucket *_nvHashMap_get_bucket_at(
    phy_hashmap *map,
    size_t index
) {
    return (phy_hashmap_bucket *)(((char *)map->buckets) + (map->bucketsz * index));
}

static inline void *_nvHashMap_get_bucket_item(phy_hashmap_bucket *entry) {
    return ((char *)entry) + sizeof(phy_hashmap_bucket);
}

static inline uint64_t _nvHashMap_clip(uint64_t hash) {
    return hash & 0xFFFFFFFFFFFF;
}

static inline bool _nvHashMap_resize(phy_hashmap *hashmap, size_t new_cap) {
    phy_hashmap *hashmap2 = phy_hashmap_new(hashmap->elsize, new_cap, hashmap->hash_func);
    if (!hashmap2) return false;

    for (size_t i = 0; i < hashmap->nbuckets; i++) {
        phy_hashmap_bucket *entry = _nvHashMap_get_bucket_at(hashmap, i);
        if (!entry->dib)continue;
        entry->dib = 1;

        size_t j = entry->hash & hashmap2->mask;
        while (true) {
            phy_hashmap_bucket *bucket = _nvHashMap_get_bucket_at(hashmap2, j);

            if (bucket->dib == 0) {
                memcpy(bucket, entry, hashmap->bucketsz);
                break;
            }

            if (bucket->dib < entry->dib) {
                memcpy(hashmap2->spare, bucket, hashmap->bucketsz);
                memcpy(bucket, entry, hashmap->bucketsz);
                memcpy(entry, hashmap2->spare, hashmap->bucketsz);
            }

            j = (j + 1) & hashmap2->mask;
            entry->dib += 1;
        }
    }

    free(hashmap->buckets);

    hashmap->buckets = hashmap2->buckets;
    hashmap->nbuckets = hashmap2->nbuckets;
    hashmap->mask = hashmap2->mask;
    hashmap->growat = hashmap2->growat;
    hashmap->shrinkat = hashmap2->shrinkat;

    free(hashmap2);

    return true;
}


phy_hashmap *phy_hashmap_new(
    size_t item_size,
    size_t cap,
    uint64_t (*hash_func)(void *item)
) {
    // Capacity must be a power of 2 and higher than the default value.
    size_t ncap = PHY_HASHMAP_CAPACITY;
    if (cap < ncap) cap = ncap;
    else {
        while (ncap < cap) ncap *= 2;
        cap = ncap;
    }

    size_t bucketsz = sizeof(phy_hashmap_bucket) + item_size;
    while (bucketsz & (sizeof(uintptr_t) - 1)) {
        bucketsz++;
    }

    size_t size = sizeof(phy_hashmap)+bucketsz*2;
    phy_hashmap *hashmap = malloc(size);
    if (!hashmap) {
      utl_error_func("Failed to allocate memory", utl_user_defined_data);
    }

    hashmap->count = 0;
    hashmap->oom = false;
    hashmap->elsize = item_size;
    hashmap->hash_func = hash_func;
    hashmap->bucketsz = bucketsz;
    hashmap->spare = ((char*)hashmap) + sizeof(phy_hashmap);
    hashmap->edata = (char*)hashmap->spare + bucketsz;
    hashmap->cap = cap;
    hashmap->nbuckets = cap;
    hashmap->mask = hashmap->nbuckets - 1;

    hashmap->buckets = malloc(hashmap->bucketsz * hashmap->nbuckets);
    if (!hashmap->buckets) {
        free(hashmap);
        utl_error_func("Failed to allocate memory", utl_user_defined_data);
        return NULL;
    }
    memset(hashmap->buckets, 0, hashmap->bucketsz * hashmap->nbuckets);

    hashmap->growpower = 1;
    hashmap->growat = (size_t)(hashmap->nbuckets * 0.6);
    hashmap->shrinkat = (size_t)(hashmap->nbuckets * 0.1);

    return hashmap;
}

void phy_hashmap_free(phy_hashmap *hashmap) {
    free(hashmap->buckets);
    free(hashmap);
}

void phy_hashmap_clear(phy_hashmap *hashmap) {

    hashmap->count = 0;
    if (hashmap->nbuckets != hashmap->cap) {
        void *new_buckets = malloc(hashmap->bucketsz*hashmap->cap);
        if (new_buckets) {
            free(hashmap->buckets);
            hashmap->buckets = new_buckets;
        }
        hashmap->nbuckets = hashmap->cap;
    }

    memset(hashmap->buckets, 0, hashmap->bucketsz*hashmap->nbuckets);
    hashmap->mask = hashmap->nbuckets - 1;
    hashmap->growat = (size_t)(hashmap->nbuckets * 0.75); // Why does growing factor change?
    hashmap->shrinkat = (size_t)(hashmap->nbuckets * 0.1);

}

void *phy_hashmap_set(phy_hashmap *hashmap, void *item) {

    uint64_t hash = _nvHashMap_clip(hashmap->hash_func(item));
    hash = _nvHashMap_clip(hash);

    // Does adding one more entry overflow memory?
    hashmap->oom = false;
    if (hashmap->count == hashmap->growat) {
        if (!_nvHashMap_resize(hashmap, hashmap->nbuckets * (1<<hashmap->growpower))) {
            hashmap->oom = true;
            return NULL;
        }
    }

    phy_hashmap_bucket *entry = hashmap->edata;
    entry->hash = hash;
    entry->dib = 1;
    void *eitem = _nvHashMap_get_bucket_item(entry);
    memcpy(eitem, item, hashmap->elsize);

    void *bitem;
    size_t i = entry->hash & hashmap->mask;
    while (true) {
        phy_hashmap_bucket *bucket = _nvHashMap_get_bucket_at(hashmap, i);

        if (bucket->dib == 0) {
            memcpy(bucket, entry, hashmap->bucketsz);
            hashmap->count++;
            return NULL;
        }

        bitem = _nvHashMap_get_bucket_item(bucket);

        if (entry->hash == bucket->hash) {
            memcpy(hashmap->spare, bitem, hashmap->elsize);
            memcpy(bitem, eitem, hashmap->elsize);
            return hashmap->spare;
        }

        if (bucket->dib < entry->dib) {
            memcpy(hashmap->spare, bucket, hashmap->bucketsz);
            memcpy(bucket, entry, hashmap->bucketsz);
            memcpy(entry, hashmap->spare, hashmap->bucketsz);
            eitem = _nvHashMap_get_bucket_item(entry);
        }

        i = (i + 1) & hashmap->mask;
        entry->dib += 1;
    }

}

void *phy_hashmap_get(phy_hashmap *hashmap, void *key) {

    uint64_t hash = _nvHashMap_clip(hashmap->hash_func(key));
    hash = _nvHashMap_clip(hash);

    size_t i = hash & hashmap->mask;
    while (true) {
        phy_hashmap_bucket *bucket = _nvHashMap_get_bucket_at(hashmap, i);

        if (!bucket->dib) {
            return NULL;
        }

        if (bucket->hash == hash) {
            void *bitem = _nvHashMap_get_bucket_item(bucket);
            if (bitem != NULL) {
                return bitem;
            }
        }

        i = (i + 1) & hashmap->mask;
    }
}

void *phy_hashmap_remove(phy_hashmap *hashmap, void *key) {

    uint64_t hash = _nvHashMap_clip(hashmap->hash_func(key));
    hash = _nvHashMap_clip(hash);

    hashmap->oom = false;
    size_t i = hash & hashmap->mask;
    
    while (true) {
        phy_hashmap_bucket *bucket = _nvHashMap_get_bucket_at(hashmap, i);
        if (!bucket->dib) {
            return NULL;
        }

        void *bitem = _nvHashMap_get_bucket_item(bucket);
        if (bucket->hash == hash) {
            memcpy(hashmap->spare, bitem, hashmap->elsize);
            bucket->dib = 0;
            while (true) {
                phy_hashmap_bucket *prev = bucket;
                i = (i + 1) & hashmap->mask;
                
                bucket = _nvHashMap_get_bucket_at(hashmap, i);
                if (bucket->dib <= 1) {
                    prev->dib = 0;
                    break;
                }

                memcpy(prev, bucket, hashmap->bucketsz);
                prev->dib--;
            }

            hashmap->count--;

            if (hashmap->nbuckets > hashmap->cap && hashmap->count <= hashmap->shrinkat) {
                // It's OK for the _nvHashMap_resize operation to fail to allocate enough
                // memory because shriking does not change the integrity of the data.
                _nvHashMap_resize(hashmap, hashmap->nbuckets / 2);
            }

            return hashmap->spare;
        }
        i = (i + 1) & hashmap->mask;
    }

}

bool phy_hashmap_iter(phy_hashmap *hashmap, size_t *index, void **item) {

    phy_hashmap_bucket *bucket;
    do {
        if (*index >= hashmap->nbuckets) {
            return false;
        }
        bucket = _nvHashMap_get_bucket_at(hashmap, *index);
        (*index)++;
    } while (!bucket->dib);

    *item = _nvHashMap_get_bucket_item(bucket);
    
    return true;
}
