#pragma once

#include <stddef.h>
#include "khg_utils/hashmap_base.h"

#define __HASHMAP_ITER_RESET(iter) ({\
    ((iter)->iter_pos = hashmap_base_iter((iter)->iter_map, (iter)->iter_pos)) != NULL;\
})

#define __HASHMAP_CONCAT_2(x, y) x ## y
#define __HASHMAP_CONCAT(x, y) __HASHMAP_CONCAT_2(x, y)
#define __HASHMAP_MAKE_UNIQUE(prefix) __HASHMAP_CONCAT(__HASHMAP_CONCAT(prefix, __COUNTER__), _)
#define __HASHMAP_UNIQUE(unique, name) __HASHMAP_CONCAT(unique, name)
#define __HASHMAP_FOREACH(x, key, data, h) for\ 
  (HASHMAP_ITER(*(h)) __HASHMAP_UNIQUE(x, it) = hashmap_iter(h);\
  ((key) = hashmap_iter_get_key(&__HASHMAP_UNIQUE(x, it))) && ((data) = hashmap_iter_get_data(&__HASHMAP_UNIQUE(x, it)));\
  hashmap_iter_next(&__HASHMAP_UNIQUE(x, it)))
#define __HASHMAP_FOREACH_SAFE(x, key, data, h, temp_ptr) for\
  (HASHMAP_ITER(*(h)) __HASHMAP_UNIQUE(x, it) = hashmap_iter(h);\
  ((temp_ptr) = (void *)((key) = hashmap_iter_get_key(&__HASHMAP_UNIQUE(x, it)))) && ((data) = hashmap_iter_get_data(&__HASHMAP_UNIQUE(x, it)));\
  ((temp_ptr) == (void *)hashmap_iter_get_key(&__HASHMAP_UNIQUE(x, it))) ? hashmap_iter_next(&__HASHMAP_UNIQUE(x, it)) : __HASHMAP_ITER_RESET(&__HASHMAP_UNIQUE(x, it)))
#define __HASHMAP_FOREACH_KEY(x, key, h) for\
  (HASHMAP_ITER(*(h)) __HASHMAP_UNIQUE(x, it) = hashmap_iter(h);\
  (key = hashmap_iter_get_key(&__HASHMAP_UNIQUE(x, it)));\
  hashmap_iter_next(&__HASHMAP_UNIQUE(x, it)))
#define __HASHMAP_FOREACH_KEY_SAFE(x, key, h, temp_ptr) for\
  (HASHMAP_ITER(*(h)) __HASHMAP_UNIQUE(x, it) = hashmap_iter(h);\
  ((temp_ptr) = (void *)((key) = hashmap_iter_get_key(&__HASHMAP_UNIQUE(x, it))));\
  ((temp_ptr) == (void *)hashmap_iter_get_key(&__HASHMAP_UNIQUE(x, it))) ? hashmap_iter_next(&__HASHMAP_UNIQUE(x, it)) : __HASHMAP_ITER_RESET(&__HASHMAP_UNIQUE(x, it)))
#define __HASHMAP_FOREACH_DATA(x, data, h) for\
  (HASHMAP_ITER(*(h)) __HASHMAP_UNIQUE(x, it) = hashmap_iter(h);\
  (data = hashmap_iter_get_data(&__HASHMAP_UNIQUE(x, it)));\
  hashmap_iter_next(&__HASHMAP_UNIQUE(x, it)))
#define __HASHMAP_FOREACH_DATA_SAFE(x, data, h, temp_ptr) for\
  (HASHMAP_ITER(*(h)) __HASHMAP_UNIQUE(x, it) = hashmap_iter(h);  \
  ((temp_ptr) = (void *)hashmap_iter_get_key(&__HASHMAP_UNIQUE(x, it))) && ((data) = hashmap_iter_get_data(&__HASHMAP_UNIQUE(x, it))); \
  ((temp_ptr) == (void *)hashmap_iter_get_key(&__HASHMAP_UNIQUE(x, it))) ? hashmap_iter_next(&__HASHMAP_UNIQUE(x, it)) : __HASHMAP_ITER_RESET(&__HASHMAP_UNIQUE(x, it)))

#define HASHMAP(key_type, data_type)\
  struct {\
    struct hashmap_base map_base;\
    struct {\
      const key_type *t_key;\
      data_type *t_data;\
      size_t (*t_hash_func)(const key_type *);\
      int (*t_compare_func)(const key_type *, const key_type *);\
      key_type *(*t_key_dup_func)(const key_type *);\
      void (*t_key_free_func)(key_type *);\
      int (*t_foreach_func)(const key_type *, data_type *, void *); \
      struct {\
        struct hashmap_base *iter_map;\
        struct hashmap_entry *iter_pos;\
        struct {\
          const key_type *t_key;\
          data_type *t_data;\
        } iter_types[0];\
      } t_iterator;\
    } map_types[0];\
  }

#define HASHMAP_ITER(hashmap_type) typeof((hashmap_type).map_types->t_iterator)

#define hashmap_init(h, hash_func, compare_func) {\
  typeof((h)->map_types->t_hash_func) __map_hash = (hash_func);\
  typeof((h)->map_types->t_compare_func) __map_compare = (compare_func);\
  hashmap_base_init(&(h)->map_base, (size_t (*)(const void *))__map_hash, (int (*)(const void *, const void *))__map_compare);\
}

#define hashmap_cleanup(h) hashmap_base_cleanup(&(h)->map_base)

#define hashmap_set_key_alloc_funcs(h, key_dup_func, key_free_func) {\
    typeof((h)->map_types->t_key_dup_func) __map_key_dup = (key_dup_func);\
    typeof((h)->map_types->t_key_free_func) __map_key_free = (key_free_func);\
    hashmap_base_set_key_alloc_funcs(&(h)->map_base, (void *(*)(const void *))__map_key_dup, (void(*)(void *))__map_key_free);\
}

#define hashmap_size(h) ((typeof((h)->map_base.size))(h)->map_base.size)
#define hashmap_reserve(h, capacity) hashmap_base_reserve(&(h)->map_base, capacity)

#define hashmap_put(h, key, data) ({\
  typeof((h)->map_types->t_key) __map_key = (key);\
  typeof((h)->map_types->t_data) __map_data = (data);\
  hashmap_base_put(&(h)->map_base, (const void *)__map_key, (void *)__map_data);\
})

#define hashmap_get(h, key) ({\
  typeof((h)->map_types->t_key) __map_key = (key);\
  (typeof((h)->map_types->t_data))hashmap_base_get(&(h)->map_base, (const void *)__map_key);\
})

#define hashmap_remove(h, key) ({\
  typeof((h)->map_types->t_key) __map_key = (key);\
  (typeof((h)->map_types->t_data))hashmap_base_remove(&(h)->map_base, (const void *)__map_key);\
})

#define hashmap_clear(h) hashmap_base_clear(&(h)->map_base)
#define hashmap_reset(h) hashmap_base_reset(&(h)->map_base)

#define hashmap_iter(h) ((HASHMAP_ITER(*(h))){ &(h)->map_base, hashmap_base_iter(&(h)->map_base, NULL) })
#define hashmap_iter_valid(iter) hashmap_base_iter_valid((iter)->iter_map, (iter)->iter_pos)
#define hashmap_iter_next(iter) hashmap_base_iter_next((iter)->iter_map, &(iter)->iter_pos)
#define hashmap_iter_remove(iter) hashmap_base_iter_remove((iter)->iter_map, &(iter)->iter_pos)

#define hashmap_iter_get_key(iter) ((typeof((iter)->iter_types->t_key))hashmap_base_iter_get_key((iter)->iter_pos))
#define hashmap_iter_get_data(iter) ((typeof((iter)->iter_types->t_data))hashmap_base_iter_get_data((iter)->iter_pos))
#define hashmap_iter_set_data(iter, data) ({\
  (typeof((iter)->iter_types->t_data)) __map_data = (data);\
  hashmap_base_iter_set_data((iter)->iter_pos, (void *)__map_data);\
})

#define hashmap_foreach(key, data, h) __HASHMAP_FOREACH(__HASHMAP_MAKE_UNIQUE(__map), (key), (data), (h))
#define hashmap_foreach_safe(key, data, h, temp_ptr) __HASHMAP_FOREACH_SAFE(__HASHMAP_MAKE_UNIQUE(__map), (key), (data), (h), (temp_ptr))
#define hashmap_foreach_key(key, h) __HASHMAP_FOREACH_KEY(__HASHMAP_MAKE_UNIQUE(__map), (key), (h))
#define hashmap_foreach_key_safe(key, h, temp_ptr) __HASHMAP_FOREACH_KEY_SAFE(__HASHMAP_MAKE_UNIQUE(__map), (key), (h), (temp_ptr))
#define hashmap_foreach_data(data, h) __HASHMAP_FOREACH_DATA(__HASHMAP_MAKE_UNIQUE(__map), (data), (h))
#define hashmap_foreach_data_safe(data, h, temp_ptr) __HASHMAP_FOREACH_DATA_SAFE(__HASHMAP_MAKE_UNIQUE(__map), (data), (h), (temp_ptr))

#define hashmap_load_factor(h) hashmap_base_load_factor(&(h)->map_base)

#define hashmap_collisions(h, key) ({\
    typeof((h)->map_types->t_key) __map_key = (key);\
    hashmap_base_collisions(&(h)->map_base, (const void *)__map_key);\
})

#define hashmap_collisions_mean(h) hashmap_base_collisions_mean(&(h)->map_base)
#define hashmap_collisions_variance(h) hashmap_base_collisions_variance(&(h)->map_base)
