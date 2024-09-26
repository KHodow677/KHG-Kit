#pragma once

#include "khg_ecs/ecs.h"
#include "khg_phy/phy_types.h"
#include "khg_utl/vector.h"

typedef struct {
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
  bool is_locked_on;
} physics_info;

typedef struct {
  ecs_id id;
} comp_physics;

typedef struct {
  ecs_id id;
  ecs_ecs ecs;
} sys_physics;

extern ecs_id PHYSICS_COMPONENT_SIGNATURE;
extern physics_info NO_PHYSICS;
extern physics_info *PHYSICS_INFO;

void comp_physics_register(comp_physics *cp);

void sys_physics_register(sys_physics *sp);
void sys_physics_add(ecs_id *eid, physics_info *info);

