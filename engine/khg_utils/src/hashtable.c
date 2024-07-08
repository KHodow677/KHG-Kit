#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "khg_utils/hashtable.h"

int ht_setup(hash_table* table, size_t key_size, size_t value_size, size_t capacity) {
	assert(table != NULL);
	if (table == NULL) return HT_ERROR;
	if (capacity < HT_MINIMUM_CAPACITY) {
		capacity = HT_MINIMUM_CAPACITY;
	}
	if (_ht_allocate(table, capacity) == HT_ERROR) {
		return HT_ERROR;
	}
	table->key_size = key_size;
	table->value_size = value_size;
	table->hash = _ht_default_hash;
	table->compare = _ht_default_compare;
	table->size = 0;
	return HT_SUCCESS;
}

int ht_copy(hash_table* first, hash_table* second) {
	size_t chain;
	ht_node* node;
	assert(first != NULL);
	assert(ht_is_initialized(second));
	if (first == NULL) return HT_ERROR;
	if (!ht_is_initialized(second)) return HT_ERROR;
	if (_ht_allocate(first, second->capacity) == HT_ERROR) {
		return HT_ERROR;
	}
	first->key_size = second->key_size;
	first->value_size = second->value_size;
	first->hash = second->hash;
	first->compare = second->compare;
	first->size = second->size;
	for (chain = 0; chain < second->capacity; ++chain) {
		for (node = second->nodes[chain]; node; node = node->next) {
			if (_ht_push_front(first, chain, node->key, node->value) == HT_ERROR) {
				return HT_ERROR;
			}
		}
	}
	return HT_SUCCESS;
}

int ht_move(hash_table* first, hash_table* second) {
	assert(first != NULL);
	assert(ht_is_initialized(second));
	if (first == NULL) return HT_ERROR;
	if (!ht_is_initialized(second)) return HT_ERROR;
	*first = *second;
	second->nodes = NULL;
	return HT_SUCCESS;
}

int ht_swap(hash_table* first, hash_table* second) {
	assert(ht_is_initialized(first));
	assert(ht_is_initialized(second));
	if (!ht_is_initialized(first)) return HT_ERROR;
	if (!ht_is_initialized(second)) return HT_ERROR;
	_ht_int_swap(&first->key_size, &second->key_size);
	_ht_int_swap(&first->value_size, &second->value_size);
	_ht_int_swap(&first->size, &second->size);
	_ht_pointer_swap((void**)&first->hash, (void**)&second->hash);
	_ht_pointer_swap((void**)&first->compare, (void**)&second->compare);
	_ht_pointer_swap((void**)&first->nodes, (void**)&second->nodes);
	return HT_SUCCESS;
}

int ht_destroy(hash_table* table) {
	ht_node* node;
	ht_node* next;
	size_t chain;
	assert(ht_is_initialized(table));
	if (!ht_is_initialized(table)) return HT_ERROR;
	for (chain = 0; chain < table->capacity; ++chain) {
		node = table->nodes[chain];
		while (node) {
			next = node->next;
			_ht_destroy_node(node);
			node = next;
		}
	}
	free(table->nodes);
	return HT_SUCCESS;
}

int ht_insert(hash_table* table, void* key, void* value) {
	size_t index;
	ht_node* node;
	assert(ht_is_initialized(table));
	assert(key != NULL);
	if (!ht_is_initialized(table)) return HT_ERROR;
	if (key == NULL) return HT_ERROR;
	if (_ht_should_grow(table)) {
		_ht_adjust_capacity(table);
	}
	index = _ht_hash(table, key);
	for (node = table->nodes[index]; node; node = node->next) {
		if (_ht_equal(table, key, node->key)) {
			memcpy(node->value, value, table->value_size);
			return HT_UPDATED;
		}
	}
	if (_ht_push_front(table, index, key, value) == HT_ERROR) {
		return HT_ERROR;
	}
	++table->size;
	return HT_INSERTED;
}

int ht_contains(hash_table* table, void* key) {
	size_t index;
	ht_node* node;
	assert(ht_is_initialized(table));
	assert(key != NULL);
	if (!ht_is_initialized(table)) return HT_ERROR;
	if (key == NULL) return HT_ERROR;
	index = _ht_hash(table, key);
	for (node = table->nodes[index]; node; node = node->next) {
		if (_ht_equal(table, key, node->key)) {
			return HT_FOUND;
		}
	}
	return HT_NOT_FOUND;
}

void *ht_lookup(hash_table* table, void* key) {
	ht_node* node;
	size_t index;
	assert(table != NULL);
	assert(key != NULL);
	if (table == NULL) return NULL;
	if (key == NULL) return NULL;
	index = _ht_hash(table, key);
	for (node = table->nodes[index]; node; node = node->next) {
		if (_ht_equal(table, key, node->key)) {
			return node->value;
		}
	}
	return NULL;
}

const void *ht_const_lookup(const hash_table* table, void* key) {
	const ht_node* node;
	size_t index;
	assert(table != NULL);
	assert(key != NULL);
	if (table == NULL) return NULL;
	if (key == NULL) return NULL;
	index = _ht_hash(table, key);
	for (node = table->nodes[index]; node; node = node->next) {
		if (_ht_equal(table, key, node->key)) {
			return node->value;
		}
	}
	return NULL;
}

int ht_erase(hash_table* table, void* key) {
	ht_node* node;
	ht_node* previous;
	size_t index;
	assert(table != NULL);
	assert(key != NULL);
	if (table == NULL) return HT_ERROR;
	if (key == NULL) return HT_ERROR;
	index = _ht_hash(table, key);
	node = table->nodes[index];
	for (previous = NULL; node; previous = node, node = node->next) {
		if (_ht_equal(table, key, node->key)) {
			if (previous) {
				previous->next = node->next;
			} else {
				table->nodes[index] = node->next;
			}
			_ht_destroy_node(node);
			--table->size;
			if (_ht_should_shrink(table)) {
				if (_ht_adjust_capacity(table) == HT_ERROR) {
					return HT_ERROR;
				}
			}
			return HT_SUCCESS;
		}
	}
	return HT_NOT_FOUND;
}

int ht_clear(hash_table* table) {
	assert(table != NULL);
	assert(table->nodes != NULL);
	if (table == NULL) return HT_ERROR;
	if (table->nodes == NULL) return HT_ERROR;
	ht_destroy(table);
	_ht_allocate(table, HT_MINIMUM_CAPACITY);
	table->size = 0;
	return HT_SUCCESS;
}

int ht_is_empty(hash_table* table) {
	assert(table != NULL);
	if (table == NULL) return HT_ERROR;
	return table->size == 0;
}

bool ht_is_initialized(hash_table* table) {
	return table != NULL && table->nodes != NULL;
}

int ht_reserve(hash_table* table, size_t minimum_capacity) {
	assert(ht_is_initialized(table));
	if (!ht_is_initialized(table)) return HT_ERROR;
	if (minimum_capacity > table->threshold) {
		return _ht_resize(table, minimum_capacity / HT_LOAD_FACTOR);
	}
	return HT_SUCCESS;
}

void _ht_int_swap(size_t* first, size_t* second) {
	size_t temp = *first;
	*first = *second;
	*second = temp;
}

void _ht_pointer_swap(void** first, void** second) {
	void* temp = *first;
	*first = *second;
	*second = temp;
}

int _ht_default_compare(void* first_key, void* second_key, size_t key_size) {
	return memcmp(first_key, second_key, key_size);
}

size_t _ht_default_hash(void* raw_key, size_t key_size) {
	size_t byte;
	size_t hash = 5381;
	char* key = raw_key;
	for (byte = 0; byte < key_size; ++byte) {
		hash = ((hash << 5) + hash) ^ key[byte];
	}
	return hash;
}

size_t _ht_hash(const hash_table* table, void* key) {
#ifdef HT_USING_POWER_OF_TWO
	return table->hash(key, table->key_size) & table->capacity;
#else
	return table->hash(key, table->key_size) % table->capacity;
#endif
}

bool _ht_equal(const hash_table* table, void* first_key, void* second_key) {
	return table->compare(first_key, second_key, table->key_size) == 0;
}

bool _ht_should_grow(hash_table* table) {
	assert(table->size <= table->capacity);
	return table->size == table->capacity;
}

bool _ht_should_shrink(hash_table* table) {
	assert(table->size <= table->capacity);
	return table->size == table->capacity * HT_SHRINK_THRESHOLD;
}

ht_node *_ht_create_node(hash_table* table, void* key, void* value, ht_node* next) {
	ht_node* node;
	assert(table != NULL);
	assert(key != NULL);
	assert(value != NULL);
	if ((node = malloc(sizeof *node)) == NULL) {
		return NULL;
	}
	if ((node->key = malloc(table->key_size)) == NULL) {
		return NULL;
	}
	if ((node->value = malloc(table->value_size)) == NULL) {
		return NULL;
	}
	memcpy(node->key, key, table->key_size);
	memcpy(node->value, value, table->value_size);
	node->next = next;
	return node;
}

int _ht_push_front(hash_table* table, size_t index, void* key, void* value) {
	table->nodes[index] = _ht_create_node(table, key, value, table->nodes[index]);
	return table->nodes[index] == NULL ? HT_ERROR : HT_SUCCESS;
}

void _ht_destroy_node(ht_node* node) {
	assert(node != NULL);
	free(node->key);
	free(node->value);
	free(node);
}

int _ht_adjust_capacity(hash_table* table) {
	return _ht_resize(table, table->size * HT_GROWTH_FACTOR);
}

int _ht_allocate(hash_table* table, size_t capacity) {
	if ((table->nodes = malloc(capacity * sizeof(ht_node*))) == NULL) {
		return HT_ERROR;
	}
	memset(table->nodes, 0, capacity * sizeof(ht_node*));
	table->capacity = capacity;
	table->threshold = capacity * HT_LOAD_FACTOR;
	return HT_SUCCESS;
}

int _ht_resize(hash_table* table, size_t new_capacity) {
	ht_node** old;
	size_t old_capacity;
	if (new_capacity < HT_MINIMUM_CAPACITY) {
		if (table->capacity > HT_MINIMUM_CAPACITY) {
			new_capacity = HT_MINIMUM_CAPACITY;
		} else {
			return HT_SUCCESS;
		}
	}
	old = table->nodes;
	old_capacity = table->capacity;
	if (_ht_allocate(table, new_capacity) == HT_ERROR) {
		return HT_ERROR;
	}
	_ht_rehash(table, old, old_capacity);
	free(old);
	return HT_SUCCESS;
}

void _ht_rehash(hash_table* table, ht_node** old, size_t old_capacity) {
	ht_node* node;
	ht_node* next;
	size_t new_index;
	size_t chain;
	for (chain = 0; chain < old_capacity; ++chain) {
		for (node = old[chain]; node;) {
			next = node->next;
			new_index = _ht_hash(table, node->key);
			node->next = table->nodes[new_index];
			table->nodes[new_index] = node;
			node = next;
		}
	}
}
