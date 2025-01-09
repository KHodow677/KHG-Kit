#pragma once

#include "khg_ecs/ecs.h"
#include <stdint.h>

void ecs_flush_destroyed(ecs_ecs *ecs);
void ecs_flush_removed(ecs_ecs *ecs);

void ecs_bitset_flip(ecs_bitset *set, unsigned int bit, bool on);
bool ecs_bitset_is_zero(ecs_bitset *set);
bool ecs_bitset_test(ecs_bitset *set, unsigned int bit);
ecs_bitset ecs_bitset_and(ecs_bitset *set1, ecs_bitset *set2);
ecs_bitset ecs_bitset_or(ecs_bitset *set1, ecs_bitset *set2);
ecs_bitset ecs_bitset_not(ecs_bitset *set);
bool ecs_bitset_equal(ecs_bitset *set1, ecs_bitset *set2);
bool ecs_bitset_true(ecs_bitset *set);

void ecs_sparse_set_init(ecs_ecs *ecs, ecs_sparse_set *set, unsigned int capacity);
void ecs_sparse_set_free(ecs_ecs *ecs, ecs_sparse_set *set);
bool ecs_sparse_set_add(ecs_ecs *ecs, ecs_sparse_set *set, ecs_id id);
unsigned int ecs_sparse_set_find(ecs_sparse_set *set, ecs_id id);
bool ecs_sparse_set_remove(ecs_sparse_set *set, ecs_id id);

bool ecs_entity_system_test(ecs_bitset *require_bits, ecs_bitset *exclude_bits, ecs_bitset *entity_bits);

void ecs_stack_init(ecs_ecs *ecs, ecs_stack *stack, unsigned int capacity);
void ecs_stack_free(ecs_ecs *ecs, ecs_stack *pool);
void ecs_stack_push(ecs_ecs *ecs, ecs_stack *pool, ecs_id id);
ecs_id ecs_stack_pop(ecs_stack *pool);
unsigned int ecs_stack_size(ecs_stack *pool);

void ecs_array_init(ecs_ecs* ecs, ecs_array* array, unsigned int size, unsigned int capacity);
void ecs_array_free(ecs_ecs* ecs, ecs_array* array);
void ecs_array_resize(ecs_ecs* ecs, ecs_array* array, unsigned int capacity);

bool ecs_is_not_null(void* ptr);
bool ecs_is_valid_component_id(ecs_id id);
bool ecs_is_valid_system_id(ecs_id id);
bool ecs_is_entity_ready(ecs_ecs* ecs, ecs_id entity_id);
bool ecs_is_component_ready(ecs_ecs* ecs, ecs_id comp_id);
bool ecs_is_system_ready(ecs_ecs* ecs, ecs_id sys_id);

