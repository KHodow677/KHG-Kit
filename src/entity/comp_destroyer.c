#include "entity/comp_destroyer.h"
#include "controllers/elements/element_destruction_controller.h"
#include "game_manager.h"
#include "khg_ecs/ecs.h"
#include "khg_utl/vector.h"
#include <stdio.h>
#include <stdlib.h>

ecs_id DESTROYER_COMPONENT_SIGNATURE;
destroyer_info NO_DESTROYER = { 0 };
utl_vector *DESTROYER_INFO = NULL;

void comp_destroyer_register(comp_destroyer *cd) {
  cd->id = ecs_register_component(ECS, sizeof(comp_destroyer), NULL, NULL);
  DESTROYER_COMPONENT_SIGNATURE = cd->id; 
}

void sys_destroyer_register(sys_destroyer *sd) {
  sd->id = ecs_register_system(ECS, sys_destroyer_update, NULL, NULL, NULL);
  ecs_require_component(ECS, sd->id, DESTROYER_COMPONENT_SIGNATURE);
  sd->ecs = *ECS;
  DESTROYER_INFO = utl_vector_create(sizeof(destroyer_info));
  for (int i = 0; i < ECS->entity_count; i++) {
    utl_vector_push_back(DESTROYER_INFO, &NO_DESTROYER);
  }
}

void sys_destroyer_add(ecs_id *eid, destroyer_info *info) {
  ecs_add(ECS, *eid, DESTROYER_COMPONENT_SIGNATURE, NULL);
  utl_vector_assign(DESTROYER_INFO, *eid, info);
}

ecs_ret sys_destroyer_update(ecs_ecs *ecs, ecs_id *entities, int entity_count, ecs_dt dt, void *udata) {
  destroyer_info *info;
  for (int id = 0; id < entity_count; id++) {
    info = utl_vector_at(DESTROYER_INFO, entities[id]);
    if (info->destroy_now) {
      element_destroy(entities[id]);
    }
  }
  return 0;
}

