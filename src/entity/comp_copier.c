#include "entity/comp_copier.h"
#include "data_utl/kinematic_utl.h"
#include "game_manager.h"
#include "khg_ecs/ecs.h"
#include "khg_phy/body.h"
#include "khg_phy/phy_types.h"
#include "khg_phy/vect.h"
#include <stdio.h>
#include <stdlib.h>

ecs_id COPIER_COMPONENT_SIGNATURE;

static ecs_ret sys_copier_update(ecs_ecs *ecs, ecs_id *entities, int entity_count, ecs_dt dt, void *udata) {
  if (dt == 0.0f) {
    return 0;
  }
  for (int id = 0; id < entity_count; id++) {
    comp_copier *info = ecs_get(ECS, entities[id], COPIER_COMPONENT_SIGNATURE);
    comp_physics *p_info = ecs_get(ECS, entities[id], PHYSICS_COMPONENT_SIGNATURE);
    phy_vect body_vel = phy_body_get_velocity(info->ext_physics_ref->target_body);
    float body_ang = phy_body_get_angle(info->ext_physics_ref->target_body);
    phy_body_set_velocity(p_info->target_body, phy_v(-body_vel.x, -body_vel.y));
    phy_body_set_angle(p_info->target_body, normalize_angle(body_ang + M_PI));
  }
  return 0;
}

void comp_copier_register() {
  COPIER_COMPONENT_SIGNATURE = ecs_register_component(ECS, sizeof(comp_copier), NULL, NULL);
}

void sys_copier_register(sys_copier *sm) {
  sm->id = ecs_register_system(ECS, sys_copier_update, NULL, NULL, NULL);
  ecs_require_component(ECS, sm->id, COPIER_COMPONENT_SIGNATURE);
  sm->ecs = *ECS;
}

comp_copier *sys_copier_add(ecs_id eid) {
  return ecs_add(ECS, eid, COPIER_COMPONENT_SIGNATURE, NULL);
}

