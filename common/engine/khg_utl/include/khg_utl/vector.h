#pragma once

#include <stdbool.h>

typedef struct {
  void *pool;
  unsigned int poolSize;
  unsigned int used;
} MemoryPoolVector;

typedef struct {
  void *items;
  unsigned int size;
  unsigned int capacitySize;
  unsigned int itemSize;
  MemoryPoolVector *pool;
} utl_vector;

utl_vector *utl_vector_create(unsigned int itemSize);

bool utl_vector_is_equal(const utl_vector *vec1, const utl_vector *vec2);
bool utl_vector_is_less(const utl_vector *vec1, const utl_vector *vec2);
bool utl_vector_is_greater(const utl_vector *vec1, const utl_vector *vec2);
bool utl_vector_is_not_equal(const utl_vector *vec1, const utl_vector *vec2);
bool utl_vector_is_greater_or_equal(const utl_vector *vec1, const utl_vector *vec2);
bool utl_vector_is_less_or_equal(const utl_vector *vec1, const utl_vector *vec2);
bool utl_vector_is_empty(utl_vector *vec);
bool utl_vector_reserve(utl_vector *vec, unsigned int size);
bool utl_vector_push_back(utl_vector *vec, const void *item);
bool utl_vector_emplace_back(utl_vector *vec, void *item, unsigned int itemSize);

void utl_vector_erase(utl_vector *vec, unsigned int pos, unsigned int len);
void utl_vector_insert(utl_vector *vec, unsigned int pos, void *item);
void utl_vector_resize(utl_vector *vec, unsigned int size);
void utl_vector_shrink_to_fit(utl_vector *vec);
void utl_vector_clear(utl_vector *vec);
void utl_vector_swap(utl_vector *vec1, utl_vector *vec2);
void utl_vector_assign(utl_vector *vec, unsigned int pos, void *item);
void utl_vector_emplace(utl_vector *vec, unsigned int pos, void *item, unsigned int itemSize);
void utl_vector_deallocate(utl_vector *vec);

const void *utl_vector_cbegin(utl_vector *vec);
const void *utl_vector_cend(utl_vector *vec);
const void *utl_vector_crbegin(utl_vector *vec);
const void *utl_vector_crend(utl_vector *vec);

void *utl_vector_begin(utl_vector *vec);
void *utl_vector_end(utl_vector *vec);
void *utl_vector_pop_back(utl_vector *vec);
void *utl_vector_front(utl_vector *vec);
void *utl_vector_back(utl_vector *vec);
void *utl_vector_data(utl_vector *vec);
void *utl_vector_at(const utl_vector *vec, unsigned int pos);
void *utl_vector_rbegin(utl_vector *vec);
void *utl_vector_rend(utl_vector *vec);

unsigned int utl_vector_size(const utl_vector *vec);
unsigned int utl_vector_capacity(utl_vector *vec);
unsigned int utl_vector_max_size(utl_vector *vec);

