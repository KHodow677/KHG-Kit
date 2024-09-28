#pragma once

#include "entity/comp_health.h"
#include "entity/comp_targeter.h"
#include "khg_ecs/ecs.h"
#include "khg_phy/phy_types.h"

typedef struct {
  ecs_id id;
  phy_body *body;
  phy_shape *shape;
  phy_body *target_body;
  phy_shape *target_shape;
  phy_constraint *pivot;
  targeter_info *targeter_ref;
  comp_health *health_ref;
  float target_vel;
  float target_ang_vel;
  bool move_enabled;
  bool rotate_enabled;
  bool is_moving;
  bool is_turning;
  bool is_locked_on;
} comp_physics;

typedef struct {
  ecs_id id;
  ecs_ecs ecs;
} sys_physics;

extern ecs_id PHYSICS_COMPONENT_SIGNATURE;

void comp_physics_register(comp_physics *cp);

void sys_physics_register(sys_physics *sp);
comp_physics *sys_physics_add(ecs_id eid);

