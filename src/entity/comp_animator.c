#include "entity/comp_animator.h"
#include "entity/comp_destroyer.h"
#include "entity/comp_renderer.h"
#include "game_manager.h"
#include "khg_ecs/ecs.h"
#include "khg_utl/vector.h"
#include <stdio.h>

ecs_id ANIMATOR_COMPONENT_SIGNATURE;

static ecs_ret sys_animator_update(ecs_ecs *ecs, ecs_id *entities, int entity_count, ecs_dt dt, void *udata) {
  for (int id = 0; id < entity_count; id++) {
    comp_animator *info = ecs_get(ECS, entities[id], ANIMATOR_COMPONENT_SIGNATURE);
    renderer_info *r_info = utl_vector_at(RENDERER_INFO, entities[id]);
    comp_destroyer *d_info = ecs_get(ECS, entities[id], DESTROYER_COMPONENT_SIGNATURE);
    if (info->destroy_on_max && r_info->tex_id == info->max_tex_id) {
      d_info->destroy_now = true;
    }
    else if (info->frame_timer <= 0) {
      r_info->tex_id = r_info->tex_id < info->max_tex_id ? r_info->tex_id + 1 : info->min_tex_id;
      info->frame_timer = info->frame_duration;
    }
    info->frame_timer -= dt;
  }
  return 0;
}

void comp_animator_register(comp_animator *ca) {
  ca->id = ecs_register_component(ECS, sizeof(comp_animator), NULL, NULL);
  ANIMATOR_COMPONENT_SIGNATURE = ca->id; 
}

void sys_animator_register(sys_animator *sa) {
  sa->id = ecs_register_system(ECS, sys_animator_update, NULL, NULL, NULL);
  ecs_require_component(ECS, sa->id, ANIMATOR_COMPONENT_SIGNATURE);
  ecs_require_component(ECS, sa->id, RENDERER_COMPONENT_SIGNATURE);
  ecs_require_component(ECS, sa->id, DESTROYER_COMPONENT_SIGNATURE);
  sa->ecs = *ECS;
}

comp_animator *sys_animator_add(ecs_id eid) {
  return ecs_add(ECS, eid, ANIMATOR_COMPONENT_SIGNATURE, NULL);
}

