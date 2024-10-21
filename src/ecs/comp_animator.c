#include "ecs/comp_animator.h"
#include "ecs/comp_physics.h"
#include "ecs/comp_renderer.h"
#include "ecs/ecs_manager.h"
#include "khg_ecs/ecs.h"

ecs_id ANIMATOR_COMPONENT_SIGNATURE;
ecs_id ANIMATOR_SYSTEM_SIGNATURE;

comp_animator_constructor_info *ANIMATOR_CONSTRUCTOR_INFO = NULL;

static ecs_ret sys_animator_update(ecs_ecs *ecs, ecs_id *entities, const int entity_count, const ecs_dt dt, void *udata) {
  if (dt == 0.0f) {
    return 0;
  }
  for (int id = 0; id < entity_count; id++) {
    comp_animator *info = ecs_get(ECS, entities[id], ANIMATOR_COMPONENT_SIGNATURE);
    comp_renderer *r_info = ecs_get(ECS, entities[id], RENDERER_COMPONENT_SIGNATURE);
    if (info->destroy_on_max && r_info->tex_id == info->max_tex_id) {
    }
    else if (info->frame_timer <= 0) {
      r_info->tex_id = r_info->tex_id < info->max_tex_id ? r_info->tex_id + 1 : info->min_tex_id;
      info->frame_timer = info->frame_duration;
    }
    info->frame_timer -= dt;
  }
  return 0;
}

static void comp_animator_constructor(ecs_ecs *ecs, const ecs_id entity_id, void *ptr, void *args) {
  comp_animator *info = ptr;
  const comp_animator_constructor_info *constructor_info = ANIMATOR_CONSTRUCTOR_INFO;
  if (info && constructor_info) {
    info->min_tex_id = constructor_info->min_tex_id;
    info->max_tex_id = constructor_info->max_tex_id;
    info->frame_duration = constructor_info->frame_duration;
    info->frame_timer = constructor_info->frame_duration;
    info->destroy_on_max = constructor_info->destroy_on_max;
  }
}

void comp_animator_register() {
  ANIMATOR_COMPONENT_SIGNATURE = ecs_register_component(ECS, sizeof(comp_animator), comp_animator_constructor, NULL);
}

void sys_animator_register() {
  ANIMATOR_SYSTEM_SIGNATURE = ecs_register_system(ECS, sys_animator_update, NULL, NULL, NULL);
  ecs_require_component(ECS, ANIMATOR_SYSTEM_SIGNATURE, ANIMATOR_COMPONENT_SIGNATURE);
  ecs_require_component(ECS, ANIMATOR_SYSTEM_SIGNATURE, RENDERER_COMPONENT_SIGNATURE);
  ecs_require_component(ECS, ANIMATOR_SYSTEM_SIGNATURE, PHYSICS_COMPONENT_SIGNATURE);
}

comp_animator *sys_animator_add(ecs_id eid, comp_animator_constructor_info *crci) {
  ANIMATOR_CONSTRUCTOR_INFO = crci;
  return ecs_add(ECS, eid, ANIMATOR_COMPONENT_SIGNATURE, NULL);
}

