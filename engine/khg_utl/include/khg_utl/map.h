#pragma once

#include <stddef.h>
#include <stdbool.h>

typedef struct utl_map utl_map;
typedef struct utl_map_node utl_map_node;

typedef void *utl_key_type;
typedef void *utl_value_type;

typedef int (*compare_func_map)(const utl_key_type, const utl_key_type);
typedef void (*value_dealloc_func)(void *);

struct utl_map_node {
  void *key;
  void *value;
  utl_map_node *left;
  utl_map_node *right;
  utl_map_node *parent;
  int color;
};

typedef struct utl_map_entry {
  utl_key_type key;
  utl_value_type value;
} utl_map_entry;

typedef struct utl_map_iterator {
  utl_map_node *node;
} utl_map_iterator;

typedef struct utl_map_iterator_pair {
  utl_map_iterator first;
  utl_map_iterator second;
} utl_map_iterator_pair;

compare_func_map utl_map_key_comp(const utl_map *map);
utl_map_iterator_pair utl_map_equal_range(const utl_map *map, utl_key_type key);
utl_key_type utl_map_node_get_key(utl_map_node *node);

utl_map *utl_map_create(compare_func_map comp, value_dealloc_func deallocKey, value_dealloc_func deallocValue);
utl_map *utl_map_copy(const utl_map *src);

void utl_map_deallocate(utl_map *map);
void utl_map_iterator_increment(utl_map_iterator *it);
void utl_map_iterator_decrement(utl_map_iterator *it);
void utl_map_swap(utl_map *map1, utl_map *map2);
void utl_map_clear(utl_map *map);

utl_map_iterator utl_map_begin(const utl_map *map);
utl_map_iterator utl_map_end(const utl_map *map);
utl_map_iterator utl_map_rbegin(const utl_map *map);
utl_map_iterator utl_map_rend(const utl_map *map);
utl_map_iterator utl_map_cbegin(const utl_map *map);
utl_map_iterator utl_map_cend(const utl_map *map);
utl_map_iterator utl_map_crbegin(const utl_map *map);
utl_map_iterator utl_map_crend(const utl_map *map);
utl_map_iterator utl_map_lower_bound(const utl_map *map, utl_key_type key);
utl_map_iterator utl_map_upper_bound(const utl_map *map, utl_key_type key);
utl_map_iterator utl_map_find(const utl_map *map, utl_key_type key);

size_t utl_map_size(const utl_map *map);
size_t utl_map_max_size(const utl_map *map);
size_t utl_map_count(const utl_map *map, utl_key_type key);

utl_value_type utl_map_at(const utl_map *map, utl_key_type key);
utl_value_type utl_map_node_get_value(utl_map_node *node);

bool utl_map_insert(utl_map *map, utl_key_type key, utl_value_type value);
bool utl_map_erase(utl_map *map, utl_key_type key);
bool utl_map_emplace(utl_map *map, utl_key_type key, utl_value_type value);
bool utl_map_emplace_hint(utl_map *map, utl_map_iterator hint, utl_key_type key, utl_value_type value);
bool utl_map_empty(const utl_map *map);

