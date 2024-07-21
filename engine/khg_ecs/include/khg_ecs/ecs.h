#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define ECS_MAX_COMPONENTS 32
#define ECS_MAX_SYSTEMS 16

typedef uint32_t ecs_bitset_t;
typedef struct ecs_s ecs_t;
typedef uint32_t ecs_id_t;
#define ECS_NULL ((ecs_id_t)-1)
typedef int8_t ecs_ret_t;
#define ECS_DT_TYPE double
typedef ECS_DT_TYPE ecs_dt_t;

typedef void (*ecs_constructor_fn)(ecs_t *ecs, ecs_id_t entity_id, void *ptr, void *args);
typedef void (*ecs_destructor_fn)(ecs_t *ecs, ecs_id_t entity_id, void *ptr);

typedef ecs_ret_t (*ecs_system_fn)(ecs_t *ecs, ecs_id_t *entities, int entity_count, ecs_dt_t dt, void *udata);
typedef void (*ecs_added_fn)(ecs_t *ecs, ecs_id_t entity_id, void *udata);
typedef void (*ecs_removed_fn)(ecs_t *ecs, ecs_id_t entity_id, void *udata);

typedef struct {
  size_t capacity;
  size_t size;
  size_t *sparse;
  ecs_id_t *dense;
} ecs_sparse_set_t;

typedef struct {
  size_t capacity;
  size_t size;
  ecs_id_t *array;
} ecs_stack_t;

typedef struct {
  size_t capacity;
  size_t count;
  size_t size;
  void *data;
} ecs_array_t;

typedef struct {
  ecs_bitset_t comp_bits;
  bool ready;
} ecs_entity_t;

typedef struct {
  ecs_constructor_fn constructor;
  ecs_destructor_fn  destructor;
} ecs_comp_t;

typedef struct {
  bool active;
  ecs_sparse_set_t entity_ids;
  ecs_system_fn system_cb;
  ecs_added_fn add_cb;
  ecs_removed_fn remove_cb;
  ecs_bitset_t require_bits;
  ecs_bitset_t exclude_bits;
  void *udata;
} ecs_sys_t;

struct ecs_s {
  ecs_stack_t entity_pool;
  ecs_stack_t destroy_queue;
  ecs_stack_t remove_queue;
  ecs_entity_t *entities;
  size_t entity_count;
  ecs_comp_t comps[ECS_MAX_COMPONENTS];
  ecs_array_t comp_arrays[ECS_MAX_COMPONENTS];
  size_t comp_count;
  ecs_sys_t systems[ECS_MAX_SYSTEMS];
  size_t system_count;
  void *mem_ctx;
};

ecs_t *ecs_new(size_t entity_count, void *mem_ctx);
void ecs_free(ecs_t *ecs);
void ecs_reset(ecs_t *ecs);

ecs_id_t ecs_register_component(ecs_t *ecs, size_t size, ecs_constructor_fn constructor, ecs_destructor_fn destructor);

ecs_id_t ecs_register_system(ecs_t *ecs, ecs_system_fn system_cb, ecs_added_fn add_cb, ecs_removed_fn remove_cb, void *udata);
void ecs_require_component(ecs_t *ecs, ecs_id_t sys_id, ecs_id_t comp_id);
void ecs_exclude_component(ecs_t *ecs, ecs_id_t sys_id, ecs_id_t comp_id);
void ecs_enable_system(ecs_t *ecs, ecs_id_t sys_id);
void ecs_disable_system(ecs_t *ecs, ecs_id_t sys_id);

ecs_id_t ecs_create(ecs_t *ecs);
bool ecs_is_ready(ecs_t *ecs, ecs_id_t entity_id);
void ecs_destroy(ecs_t *ecs, ecs_id_t entity_id);
bool ecs_has(ecs_t *ecs, ecs_id_t entity_id, ecs_id_t comp_id);
void *ecs_add(ecs_t *ecs, ecs_id_t entity_id, ecs_id_t comp_id, void *args);
void *ecs_get(ecs_t *ecs, ecs_id_t entity_id, ecs_id_t comp_id);
void ecs_remove(ecs_t *ecs, ecs_id_t entity_id, ecs_id_t comp_id);
void ecs_queue_destroy(ecs_t *ecs, ecs_id_t entity_id);
void ecs_queue_remove(ecs_t *ecs, ecs_id_t entity_id, ecs_id_t comp_id);
ecs_ret_t ecs_update_system(ecs_t *ecs, ecs_id_t sys_id, ecs_dt_t dt);
ecs_ret_t ecs_update_systems(ecs_t *ecs, ecs_dt_t dt);

