#pragma once

#include <stdbool.h>
#include <stddef.h>

#define HT_MINIMUM_CAPACITY 8
#define HT_LOAD_FACTOR 5
#define HT_MINIMUM_THRESHOLD (HT_MINIMUM_CAPACITY) * (HT_LOAD_FACTOR)

#define HT_GROWTH_FACTOR 2
#define HT_SHRINK_THRESHOLD (1 / 4)

#define HT_ERROR -1
#define HT_SUCCESS 0

#define HT_UPDATED 1
#define HT_INSERTED 0

#define HT_NOT_FOUND 0
#define HT_FOUND 01

#define HT_INITIALIZER {0, 0, 0, 0, 0, NULL, NULL, NULL};

typedef int (*comparison_t)(void*, void*, size_t);
typedef size_t (*hash_t)(void*, size_t);

typedef struct ht_node ht_node;

struct ht_node {
	ht_node *next;
	void *key;
	void *value;

};

typedef struct {
	size_t size;
	size_t threshold;
	size_t capacity;

	size_t key_size;
	size_t value_size;

	comparison_t compare;
	hash_t hash;

	ht_node **nodes;

} hash_table;

int ht_setup(hash_table* table, size_t key_size, size_t value_size, size_t capacity);

int ht_copy(hash_table *first, hash_table *second);
int ht_move(hash_table *first, hash_table *second);
int ht_swap(hash_table *first, hash_table *second);

int ht_destroy(hash_table *table);

int ht_insert(hash_table *table, void *key, void *value);

int ht_contains(hash_table *table, void *key);
void *ht_lookup(hash_table *table, void *key);
const void *ht_const_lookup(const hash_table *table, void *key);

#define HT_LOOKUP_AS(type, table_pointer, key_pointer) (*(type*)ht_lookup((table_pointer), (key_pointer)))

int ht_erase(hash_table *table, void *key);
int ht_clear(hash_table *table);

int ht_is_empty(hash_table *table);
bool ht_is_initialized(hash_table *table);

int ht_reserve(hash_table *table, size_t minimum_capacity);

void _ht_int_swap(size_t *first, size_t *second);
void _ht_pointer_swap(void **first, void **second);

size_t _ht_default_hash(void *key, size_t key_size);
int _ht_default_compare(void *first_key, void *second_key, size_t key_size);

size_t _ht_hash(const hash_table *table, void *key);
bool _ht_equal(const hash_table *table, void *first_key, void *second_key);

bool _ht_should_grow(hash_table *table);
bool _ht_should_shrink(hash_table *table);

ht_node* _ht_create_node(hash_table *table, void *key, void *value, ht_node *next);
int _ht_push_front(hash_table *table, size_t index, void *key, void *value);
void _ht_destroy_node(ht_node *node);

int _ht_adjust_capacity(hash_table *table);
int _ht_allocate(hash_table *table, size_t capacity);
int _ht_resize(hash_table *table, size_t new_capacity);
void _ht_rehash(hash_table *table, ht_node **old, size_t old_capacity);

