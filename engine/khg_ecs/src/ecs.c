#include "khg_ecs/ecs.h"
#include "khg_ecs/internal.h"
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

void *ecs_realloc_zero(ecs_ecs *ecs, void *ptr, size_t old_size, size_t new_size);

ecs_ecs *ecs_new(size_t entity_count, void *mem_ctx) {
  assert(entity_count > 0);
  ecs_ecs *ecs = (ecs_ecs *)malloc(sizeof(ecs_ecs));
  if (NULL == ecs) {
    return NULL;
  }
  memset(ecs, 0, sizeof(ecs_ecs));
  ecs->entity_count = entity_count;
  ecs->mem_ctx      = mem_ctx;
  ecs_stack_init(ecs, &ecs->entity_pool,   entity_count);
  ecs_stack_init(ecs, &ecs->destroy_queue, entity_count);
  ecs_stack_init(ecs, &ecs->remove_queue,  entity_count * 2);
  ecs->entities = (ecs_entity *)malloc(ecs->entity_count * sizeof(ecs_entity));
  memset(ecs->entities, 0, ecs->entity_count * sizeof(ecs_entity));
  for (ecs_id id = 0; id < entity_count; id++) {
    ecs_stack_push(ecs, &ecs->entity_pool, id);
  }
  return ecs;
}

void ecs_free(ecs_ecs *ecs) {
  assert(ecs_is_not_null(ecs));
  for (ecs_id entity_id = 0; entity_id < ecs->entity_count; entity_id++) {
    if (ecs->entities[entity_id].ready) {
        ecs_destroy(ecs, entity_id);
    }
  }
  ecs_stack_free(ecs, &ecs->entity_pool);
  ecs_stack_free(ecs, &ecs->destroy_queue);
  ecs_stack_free(ecs, &ecs->remove_queue);
  for (ecs_id comp_id = 0; comp_id < ecs->comp_count; comp_id++) {
    ecs_array *comp_array = &ecs->comp_arrays[comp_id];
    ecs_array_free(ecs, comp_array);
  }
  for (ecs_id sys_id = 0; sys_id < ecs->system_count; sys_id++) {
    ecs_sys *sys = &ecs->systems[sys_id];
    ecs_sparse_set_free(ecs, &sys->entity_ids);
  }
  free(ecs->entities);
  free(ecs);
}

void ecs_reset(ecs_ecs *ecs) {
  assert(ecs_is_not_null(ecs));
  for (ecs_id entity_id = 0; entity_id < ecs->entity_count; entity_id++) {
    if (ecs->entities[entity_id].ready) {
        ecs_destroy(ecs, entity_id);
    }
  }
  ecs->entity_pool.size   = 0;
  ecs->destroy_queue.size = 0;
  ecs->remove_queue.size  = 0;
  memset(ecs->entities, 0, ecs->entity_count * sizeof(ecs_entity));
  for (ecs_id entity_id = 0; entity_id < ecs->entity_count; entity_id++) {
      ecs_stack_push(ecs, &ecs->entity_pool, entity_id);
  }
  for (ecs_id sys_id = 0; sys_id < ecs->system_count; sys_id++) {
      ecs->systems[sys_id].entity_ids.size = 0;
  }
}

ecs_id ecs_register_component(ecs_ecs *ecs, size_t size, ecs_constructor_fn constructor, ecs_destructor_fn destructor) {
  assert(ecs_is_not_null(ecs));
  assert(ecs->comp_count < ECS_MAX_COMPONENTS);
  assert(size > 0);
  ecs_id comp_id = ecs->comp_count;
  ecs_array *comp_array = &ecs->comp_arrays[comp_id];
  ecs_array_init(ecs, comp_array, size, ecs->entity_count);
  ecs->comps[comp_id].constructor = constructor;
  ecs->comps[comp_id].destructor = destructor;
  ecs->comp_count++;
  return comp_id;
}

ecs_id ecs_register_system(ecs_ecs *ecs, ecs_system_fn system_cb, ecs_added_fn add_cb, ecs_removed_fn remove_cb, void *udata) {
  assert(ecs_is_not_null(ecs));
  assert(ecs->system_count < ECS_MAX_SYSTEMS);
  assert(NULL != system_cb);
  ecs_id sys_id = ecs->system_count;
  ecs_sys *sys = &ecs->systems[sys_id];
  ecs_sparse_set_init(ecs, &sys->entity_ids, ecs->entity_count);
  sys->active = true;
  sys->system_cb = system_cb;
  sys->add_cb = add_cb;
  sys->remove_cb = remove_cb;
  sys->udata = udata;
  ecs->system_count++;
  return sys_id;
}

void ecs_require_component(ecs_ecs *ecs, ecs_id sys_id, ecs_id comp_id) {
  assert(ecs_is_not_null(ecs));
  assert(ecs_is_valid_system_id(sys_id));
  assert(ecs_is_valid_component_id(comp_id));
  assert(ecs_is_system_ready(ecs, sys_id));
  assert(ecs_is_component_ready(ecs, comp_id));
  ecs_sys *sys = &ecs->systems[sys_id];
  ecs_bitset_flip(&sys->require_bits, comp_id, true);
}

void ecs_exclude_component(ecs_ecs *ecs, ecs_id sys_id, ecs_id comp_id) {
  assert(ecs_is_not_null(ecs));
  assert(ecs_is_valid_system_id(sys_id));
  assert(ecs_is_valid_component_id(comp_id));
  assert(ecs_is_system_ready(ecs, sys_id));
  assert(ecs_is_component_ready(ecs, comp_id));
  ecs_sys *sys = &ecs->systems[sys_id];
  ecs_bitset_flip(&sys->exclude_bits, comp_id, true);
}

void ecs_enable_system(ecs_ecs* ecs, ecs_id sys_id) {
  assert(ecs_is_not_null(ecs));
  assert(ecs_is_valid_system_id(sys_id));
  assert(ecs_is_system_ready(ecs, sys_id));
  ecs_sys *sys = &ecs->systems[sys_id];
  sys->active = true;
}

void ecs_disable_system(ecs_ecs *ecs, ecs_id sys_id) {
  assert(ecs_is_not_null(ecs));
  assert(ecs_is_valid_system_id(sys_id));
  assert(ecs_is_system_ready(ecs, sys_id));
  ecs_sys *sys = &ecs->systems[sys_id];
  sys->active = false;
}

ecs_id ecs_create(ecs_ecs *ecs) {
  assert(ecs_is_not_null(ecs));
  ecs_stack *pool = &ecs->entity_pool;
  if (0 == ecs_stack_size(pool)) {
    size_t old_count = ecs->entity_count;
    size_t new_count = old_count + (old_count / 2) + 2;
    ecs->entities = (ecs_entity *)ecs_realloc_zero(ecs, ecs->entities, old_count * sizeof(ecs_entity), new_count * sizeof(ecs_entity));
    for (ecs_id id = old_count; id < new_count; id++) {
      ecs_stack_push(ecs, pool, id);
    }
    ecs->entity_count = new_count;
  }
  ecs_id entity_id = ecs_stack_pop(pool);
  ecs->entities[entity_id].ready = true;
  return entity_id;
}

bool ecs_is_ready(ecs_ecs *ecs, ecs_id entity_id) {
  assert(ecs_is_not_null(ecs));
  return ecs->entities[entity_id].ready;
}

void ecs_destroy(ecs_ecs *ecs, ecs_id entity_id) {
  assert(ecs_is_not_null(ecs));
  assert(ecs_is_entity_ready(ecs, entity_id));
  ecs_entity *entity = &ecs->entities[entity_id];
  for (ecs_id sys_id = 0; sys_id < ecs->system_count; sys_id++) {
    ecs_sys *sys = &ecs->systems[sys_id];
    if (ecs_sparse_set_remove(&sys->entity_ids, entity_id)) {
      if (sys->remove_cb) {
        sys->remove_cb(ecs, entity_id, sys->udata);
      }
    }
  }
  ecs_stack *pool = &ecs->entity_pool;
  ecs_stack_push(ecs, pool, entity_id);
  for (ecs_id comp_id = 0; comp_id < ecs->comp_count; comp_id++) {
    if (ecs_bitset_test(&entity->comp_bits, comp_id)) {
      ecs_comp *comp = &ecs->comps[comp_id];
      if (comp->destructor) {
        void *ptr = ecs_get(ecs, entity_id, comp_id);
        comp->destructor(ecs, entity_id, ptr);
      }
    }
  }
  memset(entity, 0, sizeof(ecs_entity));
}

bool ecs_has(ecs_ecs *ecs, ecs_id entity_id, ecs_id comp_id) {
  assert(ecs_is_not_null(ecs));
  assert(ecs_is_valid_component_id(comp_id));
  assert(ecs_is_entity_ready(ecs, entity_id));
  ecs_entity *entity = &ecs->entities[entity_id];
  return ecs_bitset_test(&entity->comp_bits, comp_id);
}

void *ecs_get(ecs_ecs *ecs, ecs_id entity_id, ecs_id comp_id) {
  assert(ecs_is_not_null(ecs));
  assert(ecs_is_valid_component_id(comp_id));
  assert(ecs_is_component_ready(ecs, comp_id));
  assert(ecs_is_entity_ready(ecs, entity_id));
  ecs_array *comp_array = &ecs->comp_arrays[comp_id];
  return (char *)comp_array->data + (comp_array->size * entity_id);
}

void *ecs_add(ecs_ecs *ecs, ecs_id entity_id, ecs_id comp_id, void *args) {
  assert(ecs_is_not_null(ecs));
  assert(ecs_is_valid_component_id(comp_id));
  assert(ecs_is_entity_ready(ecs, entity_id));
  assert(ecs_is_component_ready(ecs, comp_id));
  ecs_entity *entity = &ecs->entities[entity_id];
  ecs_array *comp_array = &ecs->comp_arrays[comp_id];
  ecs_comp *comp = &ecs->comps[comp_id];
  ecs_array_resize(ecs, comp_array, entity_id);
  void *ptr = ecs_get(ecs, entity_id, comp_id);
  memset(ptr, 0, comp_array->size);
  if (comp->constructor) {
    comp->constructor(ecs, entity_id, ptr, args);
  }
  ecs_bitset_flip(&entity->comp_bits, comp_id, true);
  for (ecs_id sys_id = 0; sys_id < ecs->system_count; sys_id++) {
    ecs_sys* sys = &ecs->systems[sys_id];
    if (ecs_entity_system_test(&sys->require_bits, &sys->exclude_bits, &entity->comp_bits)) {
      if (ecs_sparse_set_add(ecs, &sys->entity_ids, entity_id)) {
        if (sys->add_cb) {
          sys->add_cb(ecs, entity_id, sys->udata);
        }
      }
    }
  }
  return ptr;
}

void ecs_remove(ecs_ecs *ecs, ecs_id entity_id, ecs_id comp_id) {
  assert(ecs_is_not_null(ecs));
  assert(ecs_is_valid_component_id(comp_id));
  assert(ecs_is_component_ready(ecs, comp_id));
  assert(ecs_is_entity_ready(ecs, entity_id));
  ecs_entity *entity = &ecs->entities[entity_id];
  for (ecs_id sys_id = 0; sys_id < ecs->system_count; sys_id++) {
    ecs_sys *sys = &ecs->systems[sys_id];
    if (ecs_entity_system_test(&sys->require_bits, &sys->exclude_bits, &entity->comp_bits)) {
      if (ecs_sparse_set_remove(&sys->entity_ids, entity_id)) {
        if (sys->remove_cb) {
          sys->remove_cb(ecs, entity_id, sys->udata);
        }
      }
    }
  }
  ecs_comp *comp = &ecs->comps[comp_id];
  if (comp->destructor) {
    void *ptr = ecs_get(ecs, entity_id, comp_id);
    comp->destructor(ecs, entity_id, ptr);
  }
  ecs_bitset_flip(&entity->comp_bits, comp_id, false);
}

void ecs_queue_destroy(ecs_ecs *ecs, ecs_id entity_id) {
  assert(ecs_is_not_null(ecs));
  assert(ecs_is_entity_ready(ecs, entity_id));
  ecs_stack_push(ecs, &ecs->destroy_queue, entity_id);
}

void ecs_queue_remove(ecs_ecs *ecs, ecs_id entity_id, ecs_id comp_id) {
  assert(ecs_is_not_null(ecs));
  assert(ecs_is_entity_ready(ecs, entity_id));
  assert(ecs_has(ecs, entity_id, comp_id));
  ecs_stack_push(ecs, &ecs->remove_queue, entity_id);
  ecs_stack_push(ecs, &ecs->remove_queue, comp_id);
}

ecs_ret ecs_update_system(ecs_ecs *ecs, ecs_id sys_id, ecs_dt dt) {
  assert(ecs_is_not_null(ecs));
  assert(ecs_is_valid_system_id(sys_id));
  assert(ecs_is_system_ready(ecs, sys_id));
  assert(dt >= 0.0f);
  ecs_sys *sys = &ecs->systems[sys_id];
  if (!sys->active) {
    return 0;
  }
  ecs_ret code = sys->system_cb(ecs, sys->entity_ids.dense, sys->entity_ids.size, dt, sys->udata);
  ecs_flush_destroyed(ecs);
  ecs_flush_removed(ecs);
  return code;
}

ecs_ret ecs_update_systems(ecs_ecs *ecs, ecs_dt dt) {
  assert(ecs_is_not_null(ecs));
  assert(dt >= 0.0f);
  for (ecs_id sys_id = 0; sys_id < ecs->system_count; sys_id++) {
    ecs_ret code = ecs_update_system(ecs, sys_id, dt);
    if (0 != code) {
      return code;
    }
  }
  return 0;
}

