#pragma once

#include <stddef.h>
#include <stdbool.h>

typedef struct map map;
typedef struct map_node map_node;

typedef void *key_type;
typedef void *value_type;

typedef int (*compare_func_map)(const key_type, const key_type);
typedef void (*value_dealloc_func)(void *);

struct map_node {
  void *key;
  void *value;
  map_node *left;
  map_node *right;
  map_node *parent;
  int color;
};

typedef struct map_entry {
  key_type key;
  value_type value;
} map_entry;

typedef struct map_iterator {
  map_node *node;
} map_iterator;

typedef struct map_iterator_pair {
  map_iterator first;
  map_iterator second;
} map_iterator_pair;

compare_func_map map_key_comp(const map* map);
map_iterator_pair map_equal_range(const map* map, key_type key);
key_type map_node_get_key(map_node* node);

map *map_create(compare_func_map comp, value_dealloc_func deallocKey, value_dealloc_func deallocValue);
map *map_copy(const map* src);

void map_deallocate(map *map);
void map_iterator_increment(map_iterator *it);
void map_iterator_decrement(map_iterator *it);
void map_swap(map *map1, map *map2);
void map_clear(map* map);

map_iterator map_begin(const map *map);
map_iterator map_end(const map *map);
map_iterator map_rbegin(const map *map);
map_iterator map_rend(const map *map);
map_iterator map_cbegin(const map *map);
map_iterator map_cend(const map *map);
map_iterator map_crbegin(const map *map);
map_iterator map_crend(const map *map);
map_iterator map_lower_bound(const map *map, key_type key);
map_iterator map_upper_bound(const map *map, key_type key);
map_iterator map_find(const map *map, key_type key);

size_t map_size(const map *map);
size_t map_max_size(const map *map);
size_t map_count(const map *map, key_type key);

value_type map_at(const map* map, key_type key);
value_type map_node_get_value(map_node* node);

bool map_insert(map *map, key_type key, value_type value);
bool map_erase(map *map, key_type key);
bool map_emplace(map *map, key_type key, value_type value);
bool map_emplace_hint(map *map, map_iterator hint, key_type key, value_type value);
bool map_empty(const map *map);
