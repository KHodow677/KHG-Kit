#include "entity/comp_health.h"
#include "entity/comp_destroyer.h"
#include "game_manager.h"
#include "khg_ecs/ecs.h"
#include <stdio.h>
#include <stdlib.h>

ecs_id HEALTH_COMPONENT_SIGNATURE;

static ecs_ret sys_health_update(ecs_ecs *ecs, ecs_id *entities, int entity_count, ecs_dt dt, void *udata) {
  for (int id = 0; id < entity_count; id++) {
    comp_health *info = ecs_get(ECS, entities[id], HEALTH_COMPONENT_SIGNATURE);
    comp_destroyer *d_info = ecs_get(ECS, entities[id], DESTROYER_COMPONENT_SIGNATURE);
    if (info->current_health <= 0.0f) {
      d_info->destroy_now = true;
    }
  }
  return 0;
}

void comp_health_register(comp_health *ch) {
  ch->id = ecs_register_component(ECS, sizeof(comp_health), NULL, NULL);
  HEALTH_COMPONENT_SIGNATURE = ch->id; 
}

void sys_health_register(sys_health *sh) {
  sh->id = ecs_register_system(ECS, sys_health_update, NULL, NULL, NULL);
  ecs_require_component(ECS, sh->id, HEALTH_COMPONENT_SIGNATURE);
  ecs_require_component(ECS, sh->id, DESTROYER_COMPONENT_SIGNATURE);
  sh->ecs = *ECS;
}

comp_health *sys_health_add(ecs_id eid) {
  return ecs_add(ECS, eid, HEALTH_COMPONENT_SIGNATURE, NULL);
}

