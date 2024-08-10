#pragma once

#include <stdbool.h>
#include <stddef.h>

typedef struct {
  void *pool;
  size_t poolSize;
  size_t used;
} MemoryPoolVector;

typedef struct {
  void *items;
  size_t size;
  size_t capacitySize;
  size_t itemSize;
  MemoryPoolVector *pool;
} utl_vector;

utl_vector *vector_create(size_t itemSize);

bool vector_is_equal(const utl_vector *vec1, const utl_vector *vec2);
bool vector_is_less(const utl_vector *vec1, const utl_vector *vec2);
bool vector_is_greater(const utl_vector *vec1, const utl_vector *vec2);
bool vector_is_not_equal(const utl_vector *vec1, const utl_vector *vec2);
bool vector_is_greater_or_equal(const utl_vector *vec1, const utl_vector *vec2);
bool vector_is_less_or_equal(const utl_vector *vec1, const utl_vector *vec2);
bool vector_is_empty(utl_vector *vec);
bool vector_reserve(utl_vector *vec, size_t size);
bool vector_push_back(utl_vector *vec, const void *item);
bool vector_emplace_back(utl_vector *vec, void *item, size_t itemSize);

void vector_erase(utl_vector *vec, size_t pos, size_t len);
void vector_insert(utl_vector *vec, size_t pos, void *item);
void vector_resize(utl_vector *vec, size_t size);
void vector_shrink_to_fit(utl_vector *vec);
void vector_clear(utl_vector *vec);
void vector_swap(utl_vector *vec1, utl_vector *vec2);
void vector_assign(utl_vector *vec, size_t pos, void *item);
void vector_emplace(utl_vector *vec, size_t pos, void *item, size_t itemSize);
void vector_deallocate(utl_vector *vec);

const void *vector_cbegin(utl_vector *vec);
const void *vector_cend(utl_vector *vec);
const void *vector_crbegin(utl_vector *vec);
const void *vector_crend(utl_vector *vec);

void *vector_begin(utl_vector *vec);
void *vector_end(utl_vector *vec);
void *vector_pop_back(utl_vector *vec);
void *vector_front(utl_vector *vec);
void *vector_back(utl_vector *vec);
void *vector_data(utl_vector *vec);
void *vector_at(const utl_vector *vec, size_t pos);
void *vector_rbegin(utl_vector *vec);
void *vector_rend(utl_vector *vec);

size_t vector_size(const utl_vector *vec);
size_t vector_capacity(utl_vector *vec);
size_t vector_max_size(utl_vector *vec);

