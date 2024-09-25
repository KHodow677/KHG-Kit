#include "entity/comp_animator.h"
#include "entity/comp_destroyer.h"
#include "entity/comp_renderer.h"
#include "game_manager.h"
#include "khg_ecs/ecs.h"
#include "khg_utl/vector.h"
#include <stdio.h>

ecs_id ANIMATOR_COMPONENT_SIGNATURE;
animator_info NO_ANIMATOR = { 0 };
utl_vector *ANIMATOR_INFO = NULL;

static ecs_ret sys_animator_update(ecs_ecs *ecs, ecs_id *entities, int entity_count, ecs_dt dt, void *udata) {
  animator_info *info;
  renderer_info *r_info;
  destroyer_info *d_info;
  for (int id = 0; id < entity_count; id++) {
    info = utl_vector_at(ANIMATOR_INFO, entities[id]);
    r_info = utl_vector_at(RENDERER_INFO, entities[id]);
    d_info = utl_vector_at(DESTROYER_INFO, entities[id]);
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
  ANIMATOR_INFO = utl_vector_create(sizeof(animator_info));
  for (int i = 0; i < ECS->entity_count; i++) {
    utl_vector_push_back(ANIMATOR_INFO, &NO_ANIMATOR);
  }
}

void sys_animator_add(ecs_id *eid, animator_info *info) {
  ecs_add(ECS, *eid, ANIMATOR_COMPONENT_SIGNATURE, NULL);
  utl_vector_assign(ANIMATOR_INFO, *eid, info);
}

