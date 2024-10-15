#pragma once

#include "khg_ecs/ecs.h"
#include "khg_phy/phy_types.h"

typedef struct {
  ecs_id id;
  phy_body *body;
  phy_shape *shape;
  phy_body *target_body;
  phy_shape *target_shape;
  phy_constraint *pivot;
  float target_vel;
  float target_ang_vel;
  bool move_enabled;
  bool rotate_enabled;
  bool is_moving;
  bool is_turning;
} comp_physics;

extern ecs_id PHYSICS_COMPONENT_SIGNATURE;
extern ecs_id PHYSICS_SYSTEM_SIGNATURE;

void comp_physics_register(void);
void sys_physics_register(void);

comp_physics *sys_physics_add(ecs_id eid);
