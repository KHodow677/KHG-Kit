#include "entity/comp_targeter.h"
#include "data_utl/kinematic_utl.h"
#include "game_manager.h"
#include "khg_ecs/ecs.h"
#include "khg_phy/arbiter.h"
#include "khg_phy/phy_types.h"
#include "khg_phy/shape.h"
#include <stdio.h>
#include <stdlib.h>

ecs_id TARGETER_COMPONENT_SIGNATURE;
targeter_info NO_TARGETER = { 0 };
targeter_info *TARGETER_INFO = (targeter_info[ECS_ENTITY_COUNT]){};

static ecs_ret sys_targeter_update(ecs_ecs *ecs, ecs_id *entities, int entity_count, ecs_dt dt, void *udata) {
  for (int id = 0; id < entity_count; id++) {
    targeter_info *info = &TARGETER_INFO[entities[id]];
  }
  return 0;
}

bool targeter_sensor_enter(phy_arbiter *arb, phy_space *space, phy_data_pointer udata) {
  phy_shape *sensor, *entity;
  phy_arbiter_get_shapes(arb, &sensor, &entity);
  physics_info *sensor_p_info = (physics_info *)phy_shape_get_user_data(sensor);
  physics_info *entity_p_info = (physics_info *)phy_shape_get_user_data(entity);
  printf("Entity %i entered the tank entity %i area!\n", entity_p_info->eid, sensor_p_info->eid);
  return true;
}

void targeter_sensor_exit(phy_arbiter *arb, phy_space *space, phy_data_pointer udata) {
  printf("Entity exited the sensor area!\n");
}

void comp_targeter_register(comp_targeter *ct) {
  ct->id = ecs_register_component(ECS, sizeof(comp_targeter), NULL, NULL);
  TARGETER_COMPONENT_SIGNATURE = ct->id; 
}

void sys_targeter_register(sys_targeter *st) {
  st->id = ecs_register_system(ECS, sys_targeter_update, NULL, NULL, NULL);
  ecs_require_component(ECS, st->id, TARGETER_COMPONENT_SIGNATURE);
  st->ecs = *ECS;
  for (int i = 0; i < ECS_ENTITY_COUNT; i++) {
    TARGETER_INFO[i] = NO_TARGETER;
  }
}

void sys_targeter_add(ecs_id *eid, targeter_info *info) {
  ecs_add(ECS, *eid, SHOOTER_COMPONENT_SIGNATURE, NULL);
  TARGETER_INFO[*eid] = *info;
}

