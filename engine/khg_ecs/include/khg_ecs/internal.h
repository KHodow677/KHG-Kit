#pragma once

#include "khg_ecs/ecs.h"
#include <stdint.h>

extern void ecs_flush_destroyed(ecs_t *ecs);
extern void ecs_flush_removed(ecs_t *ecs);

extern void ecs_bitset_flip(ecs_bitset_t *set, int bit, bool on);
extern bool ecs_bitset_is_zero(ecs_bitset_t *set);
extern bool ecs_bitset_test(ecs_bitset_t *set, int bit);
extern ecs_bitset_t ecs_bitset_and(ecs_bitset_t *set1, ecs_bitset_t *set2);
extern ecs_bitset_t ecs_bitset_or(ecs_bitset_t *set1, ecs_bitset_t *set2);
extern ecs_bitset_t ecs_bitset_not(ecs_bitset_t *set);
extern bool ecs_bitset_equal(ecs_bitset_t *set1, ecs_bitset_t *set2);
extern bool ecs_bitset_true(ecs_bitset_t *set);

extern void ecs_sparse_set_init(ecs_t *ecs, ecs_sparse_set_t *set, size_t capacity);
extern void ecs_sparse_set_free(ecs_t *ecs, ecs_sparse_set_t *set);
extern bool ecs_sparse_set_add(ecs_t *ecs, ecs_sparse_set_t *set, ecs_id_t id);
extern size_t ecs_sparse_set_find(ecs_sparse_set_t *set, ecs_id_t id);
extern bool ecs_sparse_set_remove(ecs_sparse_set_t *set, ecs_id_t id);

extern bool ecs_entity_system_test(ecs_bitset_t *require_bits, ecs_bitset_t *exclude_bits, ecs_bitset_t *entity_bits);

extern void ecs_stack_init(ecs_t *ecs, ecs_stack_t *pool, int capacity);
extern void ecs_stack_free(ecs_t *ecs, ecs_stack_t *pool);
extern void ecs_stack_push(ecs_t *ecs, ecs_stack_t *pool, ecs_id_t id);
extern ecs_id_t ecs_stack_pop(ecs_stack_t *pool);
extern int ecs_stack_size(ecs_stack_t *pool);

extern void ecs_array_init(ecs_t* ecs, ecs_array_t* array, size_t size, size_t capacity);
extern void ecs_array_free(ecs_t* ecs, ecs_array_t* array);
extern void ecs_array_resize(ecs_t* ecs, ecs_array_t* array, size_t capacity);

extern bool ecs_is_not_null(void* ptr);
extern bool ecs_is_valid_component_id(ecs_id_t id);
extern bool ecs_is_valid_system_id(ecs_id_t id);
extern bool ecs_is_entity_ready(ecs_t* ecs, ecs_id_t entity_id);
extern bool ecs_is_component_ready(ecs_t* ecs, ecs_id_t comp_id);
extern bool ecs_is_system_ready(ecs_t* ecs, ecs_id_t sys_id);

