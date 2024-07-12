#include "khg_utils/error_func.h"
#include "khg_utils/fmt.h"
#include "khg_utils/vector.h"
#include <stdlib.h>
#include <string.h>

static MemoryPoolVector *memory_pool_create(size_t size);
static void *memory_pool_allocate(MemoryPoolVector *pool, size_t size);
static void memory_pool_destroy(MemoryPoolVector *pool);

static MemoryPoolVector *memory_pool_create(size_t size) {
  if (size == 0) {
    error_func("Memory pool size cannot be zero", user_defined_data);
    return NULL;
  }
  MemoryPoolVector *pool = malloc(sizeof(MemoryPoolVector));
  if (!pool) {
    error_func("Failed to allocate memory for MemoryPoolVector structure", user_defined_data);
    return NULL;
  }
  pool->pool = malloc(size);
  if (!pool->pool) {
    error_func("Failed to allocate memory for memory pool", user_defined_data);
    free(pool);
    return NULL;
  }
  pool->poolSize = size;
  pool->used = 0;
  return pool;
}

static void *memory_pool_allocate(MemoryPoolVector *pool, size_t size) {
  if (!pool) {
    error_func("Memory pool is not initialized", user_defined_data);
    return NULL;
  }
  if (size == 0) {
    error_func("Cannot allocate zero size", user_defined_data);
    return NULL;
  }
  if (pool->used + size > pool->poolSize) {
    error_func("Memory pool out of space, cannot allocate", user_defined_data);
    return NULL;
  }
  void *mem = (char *)pool->pool + pool->used;
  pool->used += size;
  return mem;
}

static void memory_pool_deallocate(MemoryPoolVector *pool, size_t size) {
  if (!pool) {
    error_func("Memory pool is not initialized", user_defined_data);
    return;
  }
  if (size == 0) {
    error_func("Cannot deallocate zero size", user_defined_data);
    return;
  }
  pool->used -= size;
}

static void memory_pool_destroy(MemoryPoolVector *pool) {
  if (!pool) {
    error_func("Attempted to destroy a non-initialized memory pool", user_defined_data);
    return;
  }
  free(pool->pool);
  free(pool);
}

vector *vector_create(size_t itemSize, size_t capacity) {
  vector *vec = (vector*)malloc(sizeof(vector));
  if (!vec){
    error_func("Can not allocate memory for vector structure", user_defined_data);
    exit(-1);
  } 
  vec->size = 0;
  vec->capacitySize = 32;
  vec->itemSize = itemSize;
  size_t initialPoolSize = capacity;
  vec->pool = memory_pool_create(initialPoolSize);
  if (!vec->pool) {
    free(vec);
    error_func("Can not allocate memory for vector pool", user_defined_data);
    exit(-1);
  }
  vec->items = memory_pool_allocate(vec->pool, vec->capacitySize * itemSize);
  if (!vec->items) {
    memory_pool_destroy(vec->pool);
    free(vec);
    error_func("Can not allocate memory for vector items", user_defined_data);
    exit(-1);
  }
  return vec;
}

bool vector_is_equal(const vector *vec1, const vector *vec2) {
  if (vec1 == NULL || vec2 == NULL) {
    error_func("One or both vector pointers are NULL in vector_is_equal", user_defined_data);
    return false;
  }
  if (vec1->size != vec2->size) {
    error_func("Vectors have different sizes in vector_is_equal", user_defined_data);
    return false;
  }
  return memcmp(vec1->items, vec2->items, vec1->size * vec1->itemSize) == 0;
}

bool vector_is_less(const vector *vec1, const vector *vec2) {
  if (!vec1 || !vec2) {
    error_func("One or both vector pointers are NULL in vector_is_less", user_defined_data);
    return false;
  }
  size_t minSize = vec1->size < vec2->size ? vec1->size : vec2->size;
  int cmp = memcmp(vec1->items, vec2->items, minSize * vec1->itemSize);
  return cmp < 0 || (cmp == 0 && vec1->size < vec2->size);
}

bool vector_is_greater(const vector *vec1, const vector *vec2) {
  if (!vec1 || !vec2) {
    error_func("One or both vector pointers are NULL in vector_is_greater", user_defined_data);
    return false;
  }

  size_t minSize = vec1->size < vec2->size ? vec1->size : vec2->size;
  int cmp = memcmp(vec1->items, vec2->items, minSize * vec1->itemSize);

  return cmp > 0 || (cmp == 0 && vec1->size > vec2->size);
}

bool vector_is_not_equal(const vector *vec1, const vector *vec2) {
  if (!vec1 || !vec2) {
    error_func("One or both vector pointers are NULL in vector_is_not_equal", user_defined_data);
    return true;
  }
  if (vec1->size != vec2->size) { 
    error_func("Vectors have different sizes in vector_is_not_equal", user_defined_data);
    return true;
  }
  return memcmp(vec1->items, vec2->items, vec1->size * vec1->itemSize) != 0;
}

bool vector_is_greater_or_equal(const vector *vec1, const vector *vec2) {
  if (!vec1 || !vec2) {
    error_func("One or both vector pointers are NULL in vector_is_greater_or_equal", user_defined_data);
    return false;
  }
  return !vector_is_less(vec1, vec2);
}

bool vector_is_less_or_equal(const vector *vec1, const vector *vec2) {
    if (!vec1 || !vec2) {
      error_func("One or both vector pointers are NULL in vector_is_less_or_equal", user_defined_data);
      return false;
    }
    return !vector_is_greater(vec1, vec2);
}

bool vector_is_empty(vector *vec) {
  if (vec == NULL) {
    error_func("Vector is NULL in vector_is_empty_impl", user_defined_data);
    return true;
  }
  return vec->size == 0;
}

void vector_erase(vector *vec, size_t pos, size_t len) {
  if (vec == NULL) {
    error_func("Vector is NULL in vector_erase", user_defined_data);
    return;
  }
  if (pos >= vec->size) {
    error_func("Position is out of bounds in vector_erase", user_defined_data);
    return;
  }
  if (pos + len > vec->size) {
    error_func("Length is too large in vector_erase", user_defined_data);
    return;
  }
  char *base = (char *)vec->items;
  memmove(base + pos * vec->itemSize, base + (pos + len) * vec->itemSize, (vec->size - pos - len) * vec->itemSize);
  vec->size -= len;
}

void vector_insert(vector *vec, size_t pos, void *item) {
  if (!vec) {
    error_func("Vector is NULL in vector_insert", user_defined_data);
    return;
  }
  if (pos > vec->size) {
    error_func("Position is out of bounds in vector_insert", user_defined_data);
    return;
  }
  if (vec->size == vec->capacitySize) {
    size_t newCapacity = vec->capacitySize * 2;
    void *newItems = memory_pool_allocate(vec->pool, newCapacity * vec->itemSize);
    if (!newItems) {
      error_func("Failed to allocate memory for vector_insert", user_defined_data);
      return;
    }
    memcpy(newItems, vec->items, pos * vec->itemSize);
    memcpy((char *)newItems + (pos + 1) * vec->itemSize, (char *)vec->items + pos * vec->itemSize, (vec->size - pos) * vec->itemSize);
    vec->items = newItems;
    vec->capacitySize = newCapacity;
  } 
  else {
    char *base = (char *)vec->items;
    memmove(base + (pos + 1) * vec->itemSize, base + pos * vec->itemSize, (vec->size - pos) * vec->itemSize);
  }
  memcpy((char *)vec->items + pos * vec->itemSize, item, vec->itemSize);
  vec->size++;
}

bool vector_reserve(vector *vec, size_t size) {
    if (!vec) {
      error_func("Vector is NULL in vector_reserve", user_defined_data);
      return false; 
    }
    if (vec->capacitySize >= size) {
      return true; 
    }
    void *newItems = memory_pool_allocate(vec->pool, size * vec->itemSize);
    if (!newItems) {
      error_func("Failed to allocate memory for vector_reserve", user_defined_data);
      return false;
    }
    if (vec->size > 0) {
      memcpy(newItems, vec->items, vec->size * vec->itemSize);
    }
    vec->items = newItems;
    vec->capacitySize = size;
    return true;
}

void vector_resize(vector *vec, size_t size) {
  if (!vec) {
    error_func("Vector is NULL in vector_resize", user_defined_data);
    return;
  }
  if (size > vec->capacitySize) { 
    vector_reserve(vec, size);
  }
  if (vec->size < size) { 
    memset((char *)vec->items + vec->size * vec->itemSize, 0, (size - vec->size) * vec->itemSize);  // Initialize new elements to 0 if size is increased
  }
  vec->size = size;
}

void vector_shrink_to_fit(vector *vec) {
  if (!vec) {
    error_func("Vector is NULL in vector_shrink_to_fit", user_defined_data);
    return;
  }
  if (vec->size == vec->capacitySize) {
    return;
  }
  if (vec->size == 0) {
      free(vec->items);
      vec->items = NULL;
      vec->capacitySize = 0;
      return;
  }
  void *newItems = memory_pool_allocate(vec->pool, vec->size * vec->itemSize);
  if (!newItems) {
    error_func("Failed to allocate memory for vector_shrink_to_fit", user_defined_data);
    return;
  }
  memcpy(newItems, vec->items, vec->size * vec->itemSize);
  vec->items = newItems;
  vec->capacitySize = vec->size;
}

void vector_swap(vector *vec1, vector *vec2) {
  if (!vec1 || !vec2) {
    error_func("One or both vectors are NULL in vector_swap", user_defined_data);
    return;
  }
  void *tempItems = vec1->items;
  vec1->items = vec2->items;
  vec2->items = tempItems;
  size_t tempSize = vec1->size;
  vec1->size = vec2->size;
  vec2->size = tempSize;
  size_t tempCapacity = vec1->capacitySize;
  vec1->capacitySize = vec2->capacitySize;
  vec2->capacitySize = tempCapacity;
  size_t tempItemSize = vec1->itemSize;
  vec1->itemSize = vec2->itemSize;
  vec2->itemSize = tempItemSize;
}

void vector_assign(vector *vec, size_t pos, void *item) {
  if (!vec) {
    error_func("Vector is NULL in vector_assign", user_defined_data);
    return;
  }
  if (pos >= vec->size) {
    error_func("Position is out of bounds in vector_assign", user_defined_data);
    return;
  }
  memcpy((char *)vec->items + pos * vec->itemSize, item, vec->itemSize);
}

void vector_emplace(vector *vec, size_t pos, void *item, size_t itemSize) {
  if (!vec) {
    error_func("Vector is NULL in vector_emplace", user_defined_data);
    return;
  }
  if (pos > vec->size || itemSize != vec->itemSize) {
    error_func("Invalid position or item size in vector_emplace", user_defined_data);
    return;
  }
  if (vec->size == vec->capacitySize) {
    vector_reserve(vec, vec->capacitySize * 2);
  }
  char *base = (char *)vec->items;
  memmove(base + (pos + 1) * vec->itemSize, base + pos * vec->itemSize, (vec->size - pos) * vec->itemSize);
  memcpy(base + pos * vec->itemSize, item, vec->itemSize);
  vec->size++;
}

bool vector_emplace_back(vector *vec, void *item, size_t itemSize) {
  if (!vec) {
    error_func("Vector is NULL in vector_emplace_back", user_defined_data);
    return false;
  }
  if (itemSize != vec->itemSize) {
    error_func("Invalid item size in vector_emplace_back", user_defined_data);
    return false;
  }
  if (vec->size >= vec->capacitySize) {
    if (!vector_reserve(vec, vec->capacitySize * 2)) {
      return false;
    }
  }
  memcpy((char *)vec->items + vec->size * vec->itemSize, item, vec->itemSize);
  vec->size++;
  return true;
}

bool vector_push_back(vector *vec, const void *item) {
  if (!vec) {
    error_func("Vector is NULL in vector_push_back", user_defined_data);
    return false;
  }
  if (vec->size >= vec->capacitySize) {
    size_t newCapacity = vec->capacitySize * 2;
    void *newItems = memory_pool_allocate(vec->pool, newCapacity * vec->itemSize);
    if (!newItems) {
      error_func("Failed to allocate memory in vector_push_back", user_defined_data);
      return false;
    }
    memcpy(newItems, vec->items, vec->size * vec->itemSize);
    vec->items = newItems;
    vec->capacitySize = newCapacity;
  }
  memcpy((char *)vec->items + (vec->size * vec->itemSize), item, vec->itemSize);
  vec->size++;
  return true;
}

void vector_deallocate(vector *vec) {
  if (!vec) {
    error_func("Vector is NULL in vector_deallocate", user_defined_data);
    return;
  }
  if (vec->pool != NULL) {
    memory_pool_destroy(vec->pool);
    vec->pool = NULL;
  }
  if (vec->items != NULL) {
    vec->items = NULL;
  }
  free(vec);
}

void *vector_at(const vector *vec, size_t pos) {
  if (!vec) {
    error_func("Vector is NULL in vector_at", user_defined_data);
    return NULL;
  }
  if (pos < vec->size) {
    return (char *)vec->items + (pos * vec->itemSize);
  }
  else {
    error_func("Position is out of bounds in vector_at", user_defined_data);
    return NULL;
  }
}

void *vector_rbegin(vector *vec) {
  if (!vec) {
    error_func("Vector is NULL in vector_rbegin", user_defined_data);
    return NULL;
  }
  if (vec->size == 0) {
    error_func("Vector is empty in vector_rbegin", user_defined_data);
    return NULL;
  }
  return (void *)((char *)vec->items + (vec->size - 1) * vec->itemSize);
}

void *vector_rend(vector *vec) {
  if (!vec) {
    error_func(stderr, "Error: Vector is NULL in vector_rend.\n");
    return NULL;
  }
  return (void *)((char *)vec->items - vec->itemSize);
}

const void *vector_cbegin(vector *vec) {
  if (!vec) {
    error_func("Vector is NULL in vector_cbegin", user_defined_data);
    return NULL;
  }
  if (vec->size == 0) {
    error_func("Vector is empty in vector_cbegin", user_defined_data);
    return NULL;
  }
  return (const void *)vec->items;
}

const void *vector_cend(vector *vec) {
  if (!vec) {
    error_func(stderr, "Error: Vector is NULL in vector_cend.\n");
    return NULL;
  }
  if (vec->size == 0) {
    error_func(stderr, "Error: Vector is empty in vector_end.\n");
    return NULL;
  }
  return (const void *)((char *)vec->items + (vec->size * vec->itemSize));
}

const void *vector_crbegin(vector *vec) {
  if (!vec) {
    error_func("Vector is NULL in vector_crbegin", user_defined_data);
    return NULL;
  }
  if (vec->size == 0) {
    error_func(stderr, "Error: Vector is empty in vector_crbegin.\n");
    return NULL;
  }
  return (const void *)((char *)vec->items + (vec->size - 1) * vec->itemSize);
}

const void *vector_crend(vector *vec) {
  if (vec == NULL) {
    error_func(stderr, "Error: Vector is NULL in vector_crend.\n");
    return NULL;
  } 
  return (const void *)((char *)vec->items - vec->itemSize);
}

void *vector_begin(vector *vec) {
  if (!vec) {
    error_func("Vector is NULL in vector_crend", user_defined_data);
    return NULL;
  }
  return vec->items;
}

void *vector_end(vector *vec) {
  if (!vec) {
    error_func("Vector is NULL in vector_end", user_defined_data);
    return NULL;
  }
  if (vec->size == 0) {
    error_func("Vector is empty in vector_end", user_defined_data);
    return NULL;
  }
  return (char *)vec->items + (vec->size * vec->itemSize);
}

void *vector_pop_back(vector *vec) {
  if (!vec) {
    error_func("Vector is NULL in vector_pop_back", user_defined_data);
    return NULL;
  }
  if (vec->size == 0) {
    error_func("Vector is empty in vector_pop_back", user_defined_data);
      return NULL;
  }
  vec->size--;
  return (char *)vec->items + (vec->size * vec->itemSize);
}

void vector_clear(vector *vec) {
  if (!vec) {
    error_func("Vector is NULL in vector_clear", user_defined_data);
    return;
  }
  if (vec->size != 0) {
    memory_pool_deallocate(vec->pool, vec->itemSize * vec->size);
    vec->size = 0;
  }
}

void *vector_front(vector *vec) {
    if (!vec) {
      error_func("Vector is NULL in vector_front", user_defined_data);
      return NULL;
    }
    if (vec->size == 0) {
      error_func("Vector is empty in vector_front", user_defined_data);
        return NULL;
    }
    return vec->items;
}

void *vector_back(vector *vec) {
  if (!vec) {
    error_func(stderr, "Error: Vector is NULL in vector_back.\n");
    return NULL;
  }
  if (vec->size == 0) {
    error_func(stderr, "Error: Vector is empty in vector_back.\n");
    return NULL;
  }
  return (char *)vec->items + (vec->size - 1) * vec->itemSize;
}

void *vector_data(vector *vec) {
  if (!vec) {
    error_func(stderr, "Error: Vector is NULL in vector_data.\n");
    return NULL;
  }
  return vec->items;
}

size_t vector_size(const vector *vec) {
  if (!vec) {
    error_func("Vector is NULL in vector_size", user_defined_data);
    return 0; 
  }
  return vec->size;
}

size_t vector_capacity(vector *vec) {
  if (!vec) {
    error_func("Vector is NULL in vector_capacity", user_defined_data);
    return 0;
  }
  return vec->capacitySize;
}

size_t vector_max_size(vector *vec) {
  if (!vec) {
    error_func("Vector is NULL in vector_max_size", user_defined_data);
    return 0;
  }
  return vec->itemSize;
}
