#pragma once

#include <stddef.h>
#include <stdbool.h>

typedef struct Map Map;
typedef struct MapNode MapNode;

typedef void *KeyType;
typedef void *ValueType;

typedef int (*CompareFuncMap)(const KeyType, const KeyType);
typedef void (*ValueDeallocFunc)(void *);

struct MapNode {
  void *key;
  void *value;
  struct MapNode *left;
  struct MapNode *right;
  struct MapNode *parent;
  int color;
};

typedef struct MapEntry {
  KeyType key;
  ValueType value;
} MapEntry;

typedef struct MapIterator {
  MapNode *node;
} MapIterator;

typedef struct MapIteratorPair {
  MapIterator first;
  MapIterator second;
} MapIteratorPair;

CompareFuncMap map_key_comp(const Map* map);
MapIteratorPair map_equal_range(const Map* map, KeyType key);
KeyType map_node_get_key(MapNode* node);

Map *map_create(CompareFuncMap comp, ValueDeallocFunc deallocKey, ValueDeallocFunc deallocValue);
Map *map_copy(const Map* src);

void map_deallocate(Map *map);
void map_iterator_increment(MapIterator *it);
void map_iterator_decrement(MapIterator *it);
void map_swap(Map *map1, Map *map2);
void map_clear(Map* map);

MapIterator map_begin(const Map *map);
MapIterator map_end(const Map *map);
MapIterator map_rbegin(const Map *map);
MapIterator map_rend(const Map *map);
MapIterator map_cbegin(const Map *map);
MapIterator map_cend(const Map *map);
MapIterator map_crbegin(const Map *map);
MapIterator map_crend(const Map *map);
MapIterator map_lower_bound(const Map *map, KeyType key);
MapIterator map_upper_bound(const Map *map, KeyType key);
MapIterator map_find(const Map *map, KeyType key);

size_t map_size(const Map *map);
size_t map_max_size(const Map *map);
size_t map_count(const Map *map, KeyType key);

ValueType map_at(const Map* map, KeyType key);
ValueType map_node_get_value(MapNode* node);

bool map_insert(Map *map, KeyType key, ValueType value);
bool map_erase(Map *map, KeyType key);
bool map_emplace(Map *map, KeyType key, ValueType value);
bool map_emplace_hint(Map *map, MapIterator hint, KeyType key, ValueType value);
bool map_empty(const Map *map);
