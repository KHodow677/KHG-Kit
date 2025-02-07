#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define ECS_MAX_COMPONENTS 32
#define ECS_MAX_SYSTEMS 32

typedef unsigned int ecs_bitset;
typedef struct ecs_ecs ecs_ecs;
typedef unsigned int ecs_id;
#define ECS_NULL ((ecs_id)-1)
typedef char ecs_ret;
typedef float ecs_dt;

typedef void (*ecs_constructor_fn)(ecs_ecs *ecs, ecs_id entity_id, void *ptr, void *args);
typedef void (*ecs_destructor_fn)(ecs_ecs *ecs, ecs_id entity_id, void *ptr);

typedef ecs_ret (*ecs_system_fn)(ecs_ecs *ecs, ecs_id *entities, unsigned int entity_count, ecs_dt dt, void *udata);
typedef void (*ecs_added_fn)(ecs_ecs *ecs, ecs_id entity_id, void *udata);
typedef void (*ecs_removed_fn)(ecs_ecs *ecs, ecs_id entity_id, void *udata);

typedef struct {
  unsigned int capacity;
  unsigned int size;
  unsigned int *sparse;
  ecs_id *dense;
} ecs_sparse_set;

typedef struct {
  unsigned int capacity;
  unsigned int size;
  ecs_id *array;
} ecs_stack;

typedef struct {
  unsigned int capacity;
  unsigned int count;
  unsigned int size;
  void *data;
} ecs_array;

typedef struct {
  ecs_bitset comp_bits;
  bool ready;
} ecs_entity;

typedef struct {
  ecs_constructor_fn constructor;
  ecs_destructor_fn  destructor;
} ecs_comp;

typedef struct {
  bool active;
  ecs_sparse_set entity_ids;
  ecs_system_fn system_cb;
  ecs_added_fn add_cb;
  ecs_removed_fn remove_cb;
  ecs_bitset require_bits;
  ecs_bitset exclude_bits;
  void *udata;
} ecs_sys;

struct ecs_ecs {
  ecs_stack entity_pool;
  ecs_stack destroy_queue;
  ecs_stack remove_queue;
  ecs_entity *entities;
  unsigned int entity_count;
  ecs_comp comps[ECS_MAX_COMPONENTS];
  ecs_array comp_arrays[ECS_MAX_COMPONENTS];
  unsigned int comp_count;
  ecs_sys systems[ECS_MAX_SYSTEMS];
  unsigned int system_count;
};

ecs_ecs *ecs_new(unsigned int entity_count);
void ecs_free(ecs_ecs *ecs);
void ecs_reset(ecs_ecs *ecs);

ecs_id ecs_register_component(ecs_ecs *ecs, unsigned int size, ecs_constructor_fn constructor, ecs_destructor_fn destructor);

ecs_id ecs_register_system(ecs_ecs *ecs, ecs_system_fn system_cb, ecs_added_fn add_cb, ecs_removed_fn remove_cb, void *udata);
void ecs_require_component(ecs_ecs *ecs, ecs_id sys_id, ecs_id comp_id);
void ecs_exclude_component(ecs_ecs *ecs, ecs_id sys_id, ecs_id comp_id);
void ecs_enable_system(ecs_ecs *ecs, ecs_id sys_id);
void ecs_disable_system(ecs_ecs *ecs, ecs_id sys_id);

ecs_id ecs_create(ecs_ecs *ecs);
bool ecs_is_ready(ecs_ecs *ecs, ecs_id entity_id);
void ecs_destroy(ecs_ecs *ecs, ecs_id entity_id);
bool ecs_has(ecs_ecs *ecs, ecs_id entity_id, ecs_id comp_id);
void *ecs_add(ecs_ecs *ecs, ecs_id entity_id, ecs_id comp_id, void *args);
void *ecs_get(ecs_ecs *ecs, ecs_id entity_id, ecs_id comp_id);
void ecs_remove(ecs_ecs *ecs, ecs_id entity_id, ecs_id comp_id);
void ecs_queue_destroy(ecs_ecs *ecs, ecs_id entity_id);
void ecs_queue_remove(ecs_ecs *ecs, ecs_id entity_id, ecs_id comp_id);
ecs_ret ecs_update_system(ecs_ecs *ecs, ecs_id sys_id, ecs_dt dt);
ecs_ret ecs_update_systems(ecs_ecs *ecs, ecs_dt dt);

