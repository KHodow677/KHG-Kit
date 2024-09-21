#include "entity/comp_spawner.h"
#include "game_manager.h"
#include "khg_ecs/ecs.h"
#include "khg_utl/vector.h"
#include <stdio.h>

static ecs_ret sys_spawner_update(ecs_ecs *ecs, ecs_id *entities, int entity_count, ecs_dt dt, void *udata) {
  for (int id = 0; id < entity_count; id++) {
    selector_info *s_info = utl_vector_at(SELECTOR_INFO, entities[id]);
    if (s_info->selected) {
      printf("hi\n");
    }
  }
  return 0;
}

ecs_id SPAWNER_COMPONENT_SIGNATURE;

void comp_spawner_register(comp_spawner *cs) {
  cs->id = ecs_register_component(ECS, sizeof(comp_spawner), NULL, NULL);
  SPAWNER_COMPONENT_SIGNATURE = cs->id; 
}

void sys_spawner_register(sys_spawner *ss) {
  ss->id = ecs_register_system(ECS, sys_spawner_update, NULL, NULL, NULL);
  ecs_require_component(ECS, ss->id, SPAWNER_COMPONENT_SIGNATURE);
  ecs_require_component(ECS, ss->id, SELECTOR_COMPONENT_SIGNATURE);
  ss->ecs = *ECS;
}

void sys_spawner_add(ecs_id *eid) {
  ecs_add(ECS, *eid, SPAWNER_COMPONENT_SIGNATURE, NULL);
}

