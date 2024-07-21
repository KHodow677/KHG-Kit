#include "khg_ecs/internal.h"
#include "khg_ecs/ecs.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

void *ecs_realloc_zero(ecs_t *ecs, void *ptr, size_t old_size, size_t new_size) {
  (void)ecs;
  ptr = realloc(ptr, new_size);
  if (new_size > old_size && ptr) {
    size_t diff = new_size - old_size;
    void *start = ((char *)ptr)+ old_size;
    memset(start, 0, diff);
  }
  return ptr;
}

void ecs_flush_destroyed(ecs_t *ecs) {
  ecs_stack_t *destroy_queue = &ecs->destroy_queue;
  for (size_t i = 0; i < destroy_queue->size; i++) {
    ecs_id_t entity_id = destroy_queue->array[i];
    if (ecs_is_ready(ecs, entity_id)) {
        ecs_destroy(ecs, entity_id);
    }
  }
  destroy_queue->size = 0;
}

void ecs_flush_removed(ecs_t *ecs) {
  ecs_stack_t *remove_queue = &ecs->remove_queue;
  for (size_t i = 0; i < remove_queue->size; i += 2) {
    ecs_id_t entity_id = remove_queue->array[i];
    if (ecs_is_ready(ecs, entity_id)) {
      ecs_id_t comp_id = remove_queue->array[i + 1];
      ecs_remove(ecs, entity_id, comp_id);
    }
  }
  remove_queue->size = 0;
}

bool ecs_bitset_is_zero(ecs_bitset_t *set) {
  return *set == 0;
}

void ecs_bitset_flip(ecs_bitset_t *set, int bit, bool on) {
  if (on) {
    *set |=  ((uint64_t)1 << bit);
  }
  else {
    *set &= ~((uint64_t)1 << bit);
  }
}

bool ecs_bitset_test(ecs_bitset_t *set, int bit) {
  return *set & ((uint64_t)1 << bit);
}

ecs_bitset_t ecs_bitset_and(ecs_bitset_t *set1, ecs_bitset_t *set2) {
  return *set1 & *set2;
}

ecs_bitset_t ecs_bitset_or(ecs_bitset_t *set1, ecs_bitset_t *set2) {
  return *set1 | *set2;
}

ecs_bitset_t ecs_bitset_not(ecs_bitset_t *set) {
  return ~(*set);
}

bool ecs_bitset_equal(ecs_bitset_t *set1, ecs_bitset_t *set2) {
  return *set1 == *set2;
}

bool ecs_bitset_true(ecs_bitset_t *set) {
  return *set;
}

void ecs_sparse_set_init(ecs_t *ecs, ecs_sparse_set_t *set, size_t capacity) {
  assert(ecs_is_not_null(ecs));
  assert(ecs_is_not_null(set));
  assert(capacity > 0);
  (void)ecs;
  set->capacity = capacity;
  set->size = 0;
  set->dense  = (ecs_id_t *)malloc(capacity * sizeof(ecs_id_t));
  set->sparse = (size_t *)malloc(capacity * sizeof(size_t));
  memset(set->sparse, 0, capacity * sizeof(size_t));
}

void ecs_sparse_set_free(ecs_t *ecs, ecs_sparse_set_t *set) {
  assert(ecs_is_not_null(ecs));
  assert(ecs_is_not_null(set));
  (void)ecs;
  free(set->dense);
  free(set->sparse);
}

bool ecs_sparse_set_add(ecs_t *ecs, ecs_sparse_set_t *set, ecs_id_t id) {
  assert(ecs_is_not_null(ecs));
  assert(ecs_is_not_null(set));
  (void)ecs;
  if (id >= set->capacity) {
    size_t old_capacity = set->capacity;
    size_t new_capacity = old_capacity;
    while (new_capacity <= id) {
      new_capacity += (new_capacity / 2) + 2;
    }
    set->dense = (ecs_id_t *)realloc(set->dense, new_capacity * sizeof(ecs_id_t));
    set->sparse = (size_t *)ecs_realloc_zero(ecs, set->sparse, old_capacity * sizeof(size_t), new_capacity * sizeof(size_t));
    set->capacity = new_capacity;
  }
  if (ECS_NULL != ecs_sparse_set_find(set, id)) {
    return false;
  }
  set->dense[set->size] = id;
  set->sparse[id] = set->size;
  set->size++;
  return true;
}

size_t ecs_sparse_set_find(ecs_sparse_set_t *set, ecs_id_t id) {
  assert(ecs_is_not_null(set));
  if (set->sparse[id] < set->size && set->dense[set->sparse[id]] == id) {
    return set->sparse[id];
  }
  else {
    return ECS_NULL;
  }
}

bool ecs_sparse_set_remove(ecs_sparse_set_t *set, ecs_id_t id) {
  assert(ecs_is_not_null(set));
  if (ECS_NULL == ecs_sparse_set_find(set, id)) {
    return false;
  }
  ecs_id_t tmp = set->dense[set->size - 1];
  set->dense[set->sparse[id]] = tmp;
  set->sparse[tmp] = set->sparse[id];
  set->size--;
  return true;
}

bool ecs_entity_system_test(ecs_bitset_t *require_bits, ecs_bitset_t *exclude_bits, ecs_bitset_t *entity_bits) {
  if (!ecs_bitset_is_zero(exclude_bits)) {
    ecs_bitset_t overlap = ecs_bitset_and(entity_bits, exclude_bits);
    if (ecs_bitset_true(&overlap)) {
      return false;
    }
  }
  ecs_bitset_t entity_and_require = ecs_bitset_and(entity_bits, require_bits);
  return ecs_bitset_equal(&entity_and_require, require_bits);
}

void ecs_stack_init(ecs_t *ecs, ecs_stack_t *stack, int capacity) {
  assert(ecs_is_not_null(ecs));
  assert(ecs_is_not_null(stack));
  assert(capacity > 0);
  (void)ecs;
  stack->size = 0;
  stack->capacity = capacity;
  stack->array = (ecs_id_t *)malloc(capacity * sizeof(ecs_id_t));
}

void ecs_stack_free(ecs_t *ecs, ecs_stack_t *stack) {
  assert(ecs_is_not_null(ecs));
  assert(ecs_is_not_null(stack));
  (void)ecs;
  free(stack->array);
}

void ecs_stack_push(ecs_t *ecs, ecs_stack_t *stack, ecs_id_t id) {
  assert(ecs_is_not_null(ecs));
  assert(ecs_is_not_null(stack));
  assert(stack->capacity > 0);
  (void)ecs;
  if (stack->size == stack->capacity) {
    stack->capacity += (stack->capacity / 2) + 2;
    stack->array = (ecs_id_t *)realloc(stack->array, stack->capacity * sizeof(ecs_id_t));
  }
  stack->array[stack->size++] = id;
}

ecs_id_t ecs_stack_pop(ecs_stack_t *stack) {
  assert(ecs_is_not_null(stack));
  return stack->array[--stack->size];
}

int ecs_stack_size(ecs_stack_t *stack) {
  return stack->size;
}

void ecs_array_init(ecs_t *ecs, ecs_array_t *array, size_t size, size_t capacity) {
  assert(ecs_is_not_null(ecs));
  assert(ecs_is_not_null(array));
  (void)ecs;
  memset(array, 0, sizeof(ecs_array_t));
  array->capacity = capacity;
  array->count = 0;
  array->size = size;
  array->data = malloc(size * capacity);
}

void ecs_array_free(ecs_t *ecs, ecs_array_t *array) {
  assert(ecs_is_not_null(ecs));
  assert(ecs_is_not_null(array));
  (void)ecs;
  free(array->data);
}

void ecs_array_resize(ecs_t *ecs, ecs_array_t *array, size_t capacity) {
  assert(ecs_is_not_null(ecs));
  assert(ecs_is_not_null(array));
  (void)ecs;
  if (capacity >= array->capacity) {
    while (array->capacity <= capacity) {
      array->capacity += (array->capacity / 2) + 2;
    }
    array->data = realloc(array->data, array->capacity * array->size);
  }
}

bool ecs_is_not_null(void *ptr) {
  return NULL != ptr;
}

bool ecs_is_valid_component_id(ecs_id_t id) {
  return id < ECS_MAX_COMPONENTS;
}

bool ecs_is_valid_system_id(ecs_id_t id) {
  return id < ECS_MAX_SYSTEMS;
}

bool ecs_is_entity_ready(ecs_t *ecs, ecs_id_t entity_id) {
  return ecs->entities[entity_id].ready;
}

bool ecs_is_component_ready(ecs_t *ecs, ecs_id_t comp_id) {
  return comp_id < ecs->comp_count;
}

bool ecs_is_system_ready(ecs_t *ecs, ecs_id_t sys_id) {
  return sys_id < ecs->system_count;
}

