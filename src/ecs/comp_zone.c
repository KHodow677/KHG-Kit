#include "ecs/comp_zone.h"
#include "ecs/comp_physics.h"
#include "ecs/ecs_manager.h"
#include "khg_ecs/ecs.h"
#include "khg_phy/body.h"
#include <stdio.h>

ecs_id ZONE_COMPONENT_SIGNATURE;
ecs_id ZONE_SYSTEM_SIGNATURE;

comp_zone_constructor_info *ZONE_CONSTRUCTOR_INFO = NULL;

static ecs_ret sys_zone_update(ecs_ecs *ecs, ecs_id *entities, const int entity_count, const ecs_dt dt, void *udata) {
  for (int id = 0; id < entity_count; id++) {
    comp_zone *info = ecs_get(ECS, entities[id], ZONE_COMPONENT_SIGNATURE);
    const float body_x = phy_body_get_position(info->body).x;
    if (body_x > info->min_x && body_x < info->max_x) {
      printf("Hi\n");
    }
  }
  return 0;
}

static void comp_zone_constructor(ecs_ecs *ecs, const ecs_id entity_id, void *ptr, void *args) {
  comp_zone *info = ptr;
  const comp_zone_constructor_info *constructor_info = ZONE_CONSTRUCTOR_INFO;
  if (info && constructor_info) {
    info->body = constructor_info->body;
    info->min_x = constructor_info->min_x;
    info->max_x = constructor_info->max_x;
  }
}

void comp_zone_register() {
  ZONE_COMPONENT_SIGNATURE = ecs_register_component(ECS, sizeof(comp_zone), comp_zone_constructor, NULL);
}

void sys_zone_register() {
  ZONE_SYSTEM_SIGNATURE = ecs_register_system(ECS, sys_zone_update, NULL, NULL, NULL);
  ecs_require_component(ECS, ZONE_SYSTEM_SIGNATURE, ZONE_COMPONENT_SIGNATURE);
  ecs_require_component(ECS, ZONE_SYSTEM_SIGNATURE, PHYSICS_COMPONENT_SIGNATURE);
}

comp_zone *sys_zone_add(const ecs_id eid, comp_zone_constructor_info *czci) {
  ZONE_CONSTRUCTOR_INFO = czci;
  return ecs_add(ECS, eid, ZONE_COMPONENT_SIGNATURE, NULL);
}

