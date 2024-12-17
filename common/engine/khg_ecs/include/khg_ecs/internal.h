#pragma once

#include "khg_ecs/ecs.h"
#include <stdint.h>

extern void ecs_flush_destroyed(ecs_ecs *ecs);
extern void ecs_flush_removed(ecs_ecs *ecs);

extern void ecs_bitset_flip(ecs_bitset *set, int bit, bool on);
extern bool ecs_bitset_is_zero(ecs_bitset *set);
extern bool ecs_bitset_test(ecs_bitset *set, int bit);
extern ecs_bitset ecs_bitset_and(ecs_bitset *set1, ecs_bitset *set2);
extern ecs_bitset ecs_bitset_or(ecs_bitset *set1, ecs_bitset *set2);
extern ecs_bitset ecs_bitset_not(ecs_bitset *set);
extern bool ecs_bitset_equal(ecs_bitset *set1, ecs_bitset *set2);
extern bool ecs_bitset_true(ecs_bitset *set);

extern void ecs_sparse_set_init(ecs_ecs *ecs, ecs_sparse_set *set, size_t capacity);
extern void ecs_sparse_set_free(ecs_ecs *ecs, ecs_sparse_set *set);
extern bool ecs_sparse_set_add(ecs_ecs *ecs, ecs_sparse_set *set, ecs_id id);
extern size_t ecs_sparse_set_find(ecs_sparse_set *set, ecs_id id);
extern bool ecs_sparse_set_remove(ecs_sparse_set *set, ecs_id id);

extern bool ecs_entity_system_test(ecs_bitset *require_bits, ecs_bitset *exclude_bits, ecs_bitset *entity_bits);

extern void ecs_stack_init(ecs_ecs *ecs, ecs_stack *pool, int capacity);
extern void ecs_stack_free(ecs_ecs *ecs, ecs_stack *pool);
extern void ecs_stack_push(ecs_ecs *ecs, ecs_stack *pool, ecs_id id);
extern ecs_id ecs_stack_pop(ecs_stack *pool);
extern int ecs_stack_size(ecs_stack *pool);

extern void ecs_array_init(ecs_ecs* ecs, ecs_array* array, size_t size, size_t capacity);
extern void ecs_array_free(ecs_ecs* ecs, ecs_array* array);
extern void ecs_array_resize(ecs_ecs* ecs, ecs_array* array, size_t capacity);

extern bool ecs_is_not_null(void* ptr);
extern bool ecs_is_valid_component_id(ecs_id id);
extern bool ecs_is_valid_system_id(ecs_id id);
extern bool ecs_is_entity_ready(ecs_ecs* ecs, ecs_id entity_id);
extern bool ecs_is_component_ready(ecs_ecs* ecs, ecs_id comp_id);
extern bool ecs_is_system_ready(ecs_ecs* ecs, ecs_id sys_id);

