#include "ecs/comp_animator.h"
#include "ecs/comp_physics.h"
#include "ecs/comp_renderer.h"
#include "ecs/ecs_manager.h"
#include "khg_ecs/ecs.h"
#include "rig/anim.h"
#include "rig/rig.h"
#include <stdio.h>

ecs_id ANIMATOR_COMPONENT_SIGNATURE;
ecs_id ANIMATOR_SYSTEM_SIGNATURE;

comp_animator_constructor_info *ANIMATOR_CONSTRUCTOR_INFO = NULL;

static ecs_ret sys_animator_update(ecs_ecs *ecs, ecs_id *entities, const unsigned int entity_count, const ecs_dt dt, void *udata) {
  if (dt == 0.0f) {
    return 0;
  }
  for (unsigned int id = 0; id < entity_count; id++) {
    comp_animator *info = ecs_get(ECS, entities[id], ANIMATOR_COMPONENT_SIGNATURE);
    comp_renderer *r_info = ecs_get(ECS, entities[id], RENDERER_COMPONENT_SIGNATURE);
    if (!r_info->rig.enabled) {
      continue;
    }
    info->frame_timer -= dt;
    if (r_info->rig.current_state_id != info->target_state_id) {
      r_info->rig.current_state_id = info->target_state_id;
      r_info->rig.current_frame_id = info->target_frame_id;
    }
    if (info->frame_timer <= 0 && info->target_frame_id == last_frame_num(&r_info->rig, r_info->rig.current_state_id)) {
      r_info->rig.current_frame_id = last_frame_num(&r_info->rig, r_info->rig.current_state_id);
      set_state_and_frame(&r_info->rig, r_info->rig.current_state_id, r_info->rig.current_frame_id);
      info->target_frame_id = 0;
      info->frame_timer = info->frame_duration;
    }
    else if (info->frame_timer <= 0 && r_info->rig.current_frame_id == last_frame_num(&r_info->rig, r_info->rig.current_state_id)) {
      r_info->rig.current_frame_id = 0;
      set_state_and_frame(&r_info->rig, r_info->rig.current_state_id, r_info->rig.current_frame_id);
      info->target_frame_id++;
      info->frame_timer = info->frame_duration;
    }
    else if (info->frame_timer <= 0) {
      r_info->rig.current_frame_id++;
      set_state_and_frame(&r_info->rig, r_info->rig.current_state_id, r_info->rig.current_frame_id);
      info->target_frame_id++;
      info->frame_timer = info->frame_duration;
    }
    update_rig(&r_info->rig, r_info->body, 1.0f - info->frame_timer / info->frame_duration, get_frame(&r_info->rig, info->target_state_id, info->target_frame_id), r_info->flipped);
  }
  return 0;
}

static void comp_animator_constructor(ecs_ecs *ecs, const ecs_id entity_id, void *ptr, void *args) {
  comp_animator *info = ptr;
  const comp_animator_constructor_info *constructor_info = ANIMATOR_CONSTRUCTOR_INFO;
  if (info && constructor_info) {
    info->target_state_id = constructor_info->initial_target_state_id;
    info->target_frame_id = constructor_info->initial_target_frame_id;
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

comp_animator *sys_animator_add(const ecs_id eid, comp_animator_constructor_info *crci) {
  ANIMATOR_CONSTRUCTOR_INFO = crci;
  return ecs_add(ECS, eid, ANIMATOR_COMPONENT_SIGNATURE, NULL);
}

