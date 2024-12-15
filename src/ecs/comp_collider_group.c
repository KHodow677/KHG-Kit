#include "area/collider.h"
#include "ecs/comp_collider_group.h"
#include "ecs/ecs_manager.h"
#include "khg_ecs/ecs.h"
#include "khg_utl/array.h"
#include "resources/area_loader.h"
#include <stdio.h>

ecs_id COLLIDER_GROUP_COMPONENT_SIGNATURE;
ecs_id COLLIDER_GROUP_SYSTEM_SIGNATURE;

comp_collider_group_constructor_info *COLLIDER_GROUP_CONSTRUCTOR_INFO = NULL;

static ecs_ret sys_collider_group_update(ecs_ecs *ecs, ecs_id *entities, const int entity_count, const ecs_dt dt, void *udata) {
  if (dt == 0.0f) {
    return 0;
  }
  for (int id = 0; id < entity_count; id++) {
  }
  return 0;
}

static void comp_collider_group_constructor(ecs_ecs *ecs, const ecs_id entity_id, void *ptr, void *args) {
  comp_collider_group *info = ptr;
  const comp_collider_group_constructor_info *constructor_info = COLLIDER_GROUP_CONSTRUCTOR_INFO;
  if (info && constructor_info) {
    info->area_id = constructor_info->area_id;
    info->colliders = constructor_info->area_id != NO_AREA_ID ? get_area_colliders(constructor_info->area_id, constructor_info->enabled) : (area_colliders){ false };
  }
}

static void comp_collider_group_destructor(ecs_ecs *ecs, const ecs_id entity_id, void *ptr) {
  const comp_collider_group *info = ptr;
  free_colliders(info->colliders.colliders);
  if (info) {
    utl_array_deallocate(info->colliders.colliders);
  }
}

void comp_collider_group_register() {
  COLLIDER_GROUP_COMPONENT_SIGNATURE = ecs_register_component(ECS, sizeof(comp_collider_group), comp_collider_group_constructor, comp_collider_group_destructor);
}

void sys_collider_group_register() {
  COLLIDER_GROUP_SYSTEM_SIGNATURE = ecs_register_system(ECS, sys_collider_group_update, NULL, NULL, NULL);
  ecs_require_component(ECS, COLLIDER_GROUP_SYSTEM_SIGNATURE, COLLIDER_GROUP_COMPONENT_SIGNATURE);
}

comp_collider_group *sys_collider_group_add(const ecs_id eid, comp_collider_group_constructor_info *crci) {
  COLLIDER_GROUP_CONSTRUCTOR_INFO = crci;
  return ecs_add(ECS, eid, COLLIDER_GROUP_COMPONENT_SIGNATURE, NULL);
}

