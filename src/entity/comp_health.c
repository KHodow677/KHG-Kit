#include "entity/comp_health.h"
#include "entity/comp_destroyer.h"
#include "game_manager.h"
#include "khg_ecs/ecs.h"
#include <stdio.h>
#include <stdlib.h>

ecs_id HEALTH_COMPONENT_SIGNATURE;
health_info NO_HEALTH = { 0 };
health_info *HEALTH_INFO = (health_info[ECS_ENTITY_COUNT]){};

static ecs_ret sys_health_update(ecs_ecs *ecs, ecs_id *entities, int entity_count, ecs_dt dt, void *udata) {
  for (int id = 0; id < entity_count; id++) {
    health_info *info = &HEALTH_INFO[entities[id]];
    destroyer_info *d_info = &DESTROYER_INFO[entities[id]];
    /*printf("Current Health for Entity %i: %f\n", entities[id], info->current_health);*/
    if (info->current_health <= 0.0f) {
      d_info->destroy_now = true;
    }
  }
  return 0;
}

void comp_health_register(comp_health *ch) {
  ch->id = ecs_register_component(ECS, sizeof(comp_destroyer), NULL, NULL);
  HEALTH_COMPONENT_SIGNATURE = ch->id; 
}

void sys_health_register(sys_health *sh) {
  sh->id = ecs_register_system(ECS, sys_health_update, NULL, NULL, NULL);
  ecs_require_component(ECS, sh->id, HEALTH_COMPONENT_SIGNATURE);
  ecs_require_component(ECS, sh->id, DESTROYER_COMPONENT_SIGNATURE);
  sh->ecs = *ECS;
  for (int i = 0; i < ECS_ENTITY_COUNT; i++) {
    HEALTH_INFO[i] = NO_HEALTH;
  }
}

void sys_health_add(ecs_id *eid, health_info *info) {
  ecs_add(ECS, *eid, HEALTH_COMPONENT_SIGNATURE, NULL);
  HEALTH_INFO[*eid] = *info;
}

