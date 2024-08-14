#include "entity/comp_animator.h"
#include "entity/comp_destroyer.h"
#include "entity/comp_renderer.h"
#include "data_utl/map_utl.h"
#include "generators/entities/particle_generator.h"
#include "khg_ecs/ecs.h"
#include "khg_utl/map.h"
#include <stdio.h>

ecs_id ANIMATOR_COMPONENT_SIGNATURE;
utl_map *ANIMATOR_INFO_MAP = NULL;

void comp_animator_register(comp_animator *ca, ecs_ecs *ecs) {
  ca->id = ecs_register_component(ecs, sizeof(comp_animator), NULL, NULL);
  ANIMATOR_COMPONENT_SIGNATURE = ca->id; 
}

void sys_animator_register(sys_animator *sa, ecs_ecs *ecs) {
  sa->id = ecs_register_system(ecs, sys_animator_update, NULL, NULL, NULL);
  ecs_require_component(ecs, sa->id, ANIMATOR_COMPONENT_SIGNATURE);
  ecs_require_component(ecs, sa->id, RENDERER_COMPONENT_SIGNATURE);
  ecs_require_component(ecs, sa->id, DESTROYER_COMPONENT_SIGNATURE);
  sa->ecs = *ecs;
  ANIMATOR_INFO_MAP = utl_map_create(compare_ints, no_deallocator, no_deallocator);
}

void sys_animator_add(ecs_ecs *ecs, ecs_id *eid, animator_info *info) {
  ecs_add(ecs, *eid, ANIMATOR_COMPONENT_SIGNATURE, NULL);
  utl_map_insert(ANIMATOR_INFO_MAP, eid, info);
}

void sys_animator_free(bool need_free) {
  if (need_free) {
    utl_map_deallocate(ANIMATOR_INFO_MAP);
  }
}

ecs_ret sys_animator_update(ecs_ecs *ecs, ecs_id *entities, int entity_count, ecs_dt dt, void *udata) {
  (void)ecs;
  (void)dt;
  (void)udata;
  if (entity_count == 0) {
    return 0;
  }
  animator_info *info;
  renderer_info *r_info;
  destroyer_info *d_info;
  for (int id = 0; id < entity_count; id++) {
    info = utl_map_at(ANIMATOR_INFO_MAP, &entities[id]);
    r_info = utl_map_at(RENDERER_INFO_MAP, &entities[id]);
    d_info = utl_map_at(DESTROYER_INFO_MAP, &entities[id]);
    if (--info->frame_timer == 0) {
      r_info->tex_id = r_info->tex_id < info->max_tex_id ? r_info->tex_id + 1 : info->min_tex_id;
      info->frame_timer = info->frame_duration;
    }
    if (info->destroy_on_max && r_info->tex_id == info->max_tex_id) {
      d_info->destroy_now = true;
    }
  }
  return 0;
}

