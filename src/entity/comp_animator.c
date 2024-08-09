#include "entity/comp_animator.h"
#include "data_utl/map_utl.h"
#include "entity/comp_renderer.h"
#include "khg_ecs/ecs.h"
#include "khg_utl/map.h"
#include <stdio.h>

ecs_id ANIMATOR_COMPONENT_SIGNATURE;
map *ANIMATOR_INFO_MAP = NULL;

void comp_animator_register(comp_animator *ca, ecs_ecs *ecs) {
  ca->id = ecs_register_component(ecs, sizeof(comp_animator), NULL, NULL);
  ANIMATOR_COMPONENT_SIGNATURE = ca->id; 
}

void sys_animator_register(sys_animator *sa, ecs_ecs *ecs) {
  sa->id = ecs_register_system(ecs, sys_animator_update, NULL, NULL, NULL);
  ecs_require_component(ecs, sa->id, ANIMATOR_COMPONENT_SIGNATURE);
  ecs_require_component(ecs, sa->id, RENDERER_COMPONENT_SIGNATURE);
  sa->ecs = *ecs;
  ANIMATOR_INFO_MAP = map_create(compare_ints, no_deallocator, no_deallocator);
}

void sys_animator_add(ecs_ecs *ecs, ecs_id *eid, animator_info *info) {
  ecs_add(ecs, *eid, ANIMATOR_COMPONENT_SIGNATURE, NULL);
  map_insert(ANIMATOR_INFO_MAP, eid, info);
}

void sys_animator_free(bool need_free) {
  if (need_free) {
    map_deallocate(ANIMATOR_INFO_MAP);
  }
}

ecs_ret sys_animator_update(ecs_ecs *ecs, ecs_id *entities, int entity_count, ecs_dt dt, void *udata) {
  (void)ecs;
  (void)dt;
  (void)udata;
  if (entity_count == 0) {
    return 0;
  }
  animator_info *info = map_at(ANIMATOR_INFO_MAP, &entities[0]);
  for (int id = 0; id < entity_count; id++) {
    info = map_at(ANIMATOR_INFO_MAP, &entities[id]);
    printf("Hi");
  }
  return 0;
}

