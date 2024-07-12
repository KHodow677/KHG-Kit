#pragma once

#define VECTOR_LOGGING_ENABLE

#include <stdbool.h>
#include <stddef.h>

typedef struct vector vector;

typedef struct MemoryPoolVector {
  void *pool;
  size_t poolSize;
  size_t used;
} MemoryPoolVector;

struct vector {
  void *items;
  size_t size;
  size_t capacitySize;
  size_t itemSize;
  MemoryPoolVector *pool;
};

vector *vector_create(size_t itemSize, size_t capacity);

bool vector_is_equal(const vector *vec1, const vector *vec2);
bool vector_is_less(const vector *vec1, const vector *vec2);
bool vector_is_greater(const vector *vec1, const vector *vec2);
bool vector_is_not_equal(const vector *vec1, const vector *vec2);
bool vector_is_greater_or_equal(const vector *vec1, const vector *vec2);
bool vector_is_less_or_equal(const vector *vec1, const vector *vec2);
bool vector_is_empty(vector *vec);
bool vector_reserve(vector *vec, size_t size);
bool vector_push_back(vector *vec, const void *item);
bool vector_emplace_back(vector *vec, void *item, size_t itemSize);

void vector_erase(vector *vec, size_t pos, size_t len);
void vector_insert(vector *vec, size_t pos, void *item);
void vector_resize(vector *vec, size_t size);
void vector_shrink_to_fit(vector *vec);
void vector_clear(vector *vec);
void vector_swap(vector *vec1, vector *vec2);
void vector_assign(vector *vec, size_t pos, void *item);
void vector_emplace(vector *vec, size_t pos, void *item, size_t itemSize);
void vector_deallocate(vector *vec);

const void* vector_cbegin(vector *vec);
const void* vector_cend(vector *vec);
const void* vector_crbegin(vector *vec);
const void* vector_crend(vector *vec);

void* vector_begin(vector *vec);
void* vector_end(vector *vec);
void* vector_pop_back(vector *vec);
void* vector_front(vector *vec);
void* vector_back(vector *vec);
void* vector_data(vector *vec);
void* vector_at(const vector *vec, size_t pos);
void* vector_rbegin(vector *vec);
void* vector_rend(vector *vec);

size_t vector_size(const vector *vec);
size_t vector_capacity(vector *vec);
size_t vector_max_size(vector *vec);
