#pragma once

#include "khg_ecs/ecs.h"
#include "khg_phy/body.h"
#include "khg_phy/shape.h"
#include "khg_phy/core/phy_vector.h"

typedef struct {
  ecs_id id;
  phy_rigid_body *body;
  phy_shape *shape;
  float horizontal_vel;
  float vertical_vel;
  bool move_enabled;
  bool is_moving;
} comp_physics;

typedef struct {
  enum {
    PHYSICS_BOX,
  } mode;
  const float width;
  const float height;
  const float mass;
  const phy_vector2 pos;
  const float ang;
  const bool move_enabled;
  const bool gravity_enabled;
  const bool collision_enabled;
} comp_physics_constructor_info;

extern ecs_id PHYSICS_COMPONENT_SIGNATURE;
extern ecs_id PHYSICS_SYSTEM_SIGNATURE;

extern comp_physics_constructor_info *PHYSICS_CONSTRUCTOR_INFO;

void comp_physics_register(void);
void sys_physics_register(void);

comp_physics *sys_physics_add(const ecs_id eid, comp_physics_constructor_info *cpci);

