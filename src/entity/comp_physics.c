#include "entity/comp_physics.h"
#include "game_manager.h"
#include "data_utl/kinematic_utl.h"
#include "khg_ecs/ecs.h"
#include "khg_phy/body.h"
#include "khg_phy/vect.h"
#include <math.h>
#include <stdio.h>

ecs_id PHYSICS_COMPONENT_SIGNATURE;
physics_info NO_PHYSICS = { 0 };
physics_info *PHYSICS_INFO = (physics_info[ECS_ENTITY_COUNT]){};

static ecs_ret sys_physics_update(ecs_ecs *ecs, ecs_id *entities, int entity_count, ecs_dt dt, void *udata) {
  for (int id = 0; id < entity_count; id++) {
    physics_info *info = &PHYSICS_INFO[entities[id]];
    float current_ang = normalize_angle(phy_body_get_angle(info->body));
    if (info->move_enabled) {
      phy_body_set_velocity(info->body, phy_v(sinf(current_ang)*info->target_vel, -cosf(current_ang)*info->target_vel));
    }
    if (info->rotate_enabled) {
      phy_body_set_angular_velocity(info->body, info->target_ang_vel);
    }
  }
  return 0;
}

void comp_physics_register(comp_physics *cp) {
  cp->id = ecs_register_component(ECS, sizeof(comp_physics), NULL, NULL);
  PHYSICS_COMPONENT_SIGNATURE = cp->id; 
}

void sys_physics_register(sys_physics *sp) {
  sp->id = ecs_register_system(ECS, sys_physics_update, NULL, NULL, NULL);
  ecs_require_component(ECS, sp->id, PHYSICS_COMPONENT_SIGNATURE);
  sp->ecs = *ECS;
  for (int i = 0; i < ECS_ENTITY_COUNT; i++) {
    PHYSICS_INFO[i] = NO_PHYSICS;
  }
}

void sys_physics_add(ecs_id *eid, physics_info *info) {
  ecs_add(ECS, *eid, PHYSICS_COMPONENT_SIGNATURE, NULL);
  PHYSICS_INFO[*eid] = *info;
}

