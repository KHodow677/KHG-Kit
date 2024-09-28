#include "entity/comp_destroyer.h"
#include "controllers/elements/element_destruction_controller.h"
#include "game_manager.h"
#include "khg_ecs/ecs.h"
#include <stdio.h>
#include <stdlib.h>

ecs_id DESTROYER_COMPONENT_SIGNATURE;

static ecs_ret sys_destroyer_update(ecs_ecs *ecs, ecs_id *entities, int entity_count, ecs_dt dt, void *udata) {
  for (int id = 0; id < entity_count; id++) {
    comp_destroyer *info = ecs_get(ECS, entities[id], DESTROYER_COMPONENT_SIGNATURE);
    if (info->destroy_now) {
      element_destroy(entities[id]);
    }
  }
  return 0;
}

void comp_destroyer_register() {
  DESTROYER_COMPONENT_SIGNATURE = ecs_register_component(ECS, sizeof(comp_destroyer), NULL, NULL);
}

void sys_destroyer_register(sys_destroyer *sd) {
  sd->id = ecs_register_system(ECS, sys_destroyer_update, NULL, NULL, NULL);
  ecs_require_component(ECS, sd->id, DESTROYER_COMPONENT_SIGNATURE);
  sd->ecs = *ECS;
}

comp_destroyer *sys_destroyer_add(ecs_id eid) {
  return ecs_add(ECS, eid, DESTROYER_COMPONENT_SIGNATURE, NULL);
}

