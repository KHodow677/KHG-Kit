#pragma once

#include "khg_ecs/ecs.h"
#include "khg_phy/body.h"
#include "khg_phy/shape.h"
#include "khg_phy/core/phy_vector.h"

typedef struct {
  ecs_id id;
  phy_rigid_body *body;
  phy_shape *shape;
  float target_vel;
  bool move_enabled;
  bool is_moving;
} comp_physics;

typedef struct {
  enum {
    PHYSICS_BOX,
  } mode;
  float width;
  float height;
  float mass;
  phy_vector2 pos;
  float ang;
  bool move_enabled;
  bool gravity_enabled;
  bool collision_enabled;
} comp_physics_constructor_info;

extern ecs_id PHYSICS_COMPONENT_SIGNATURE;
extern ecs_id PHYSICS_SYSTEM_SIGNATURE;

extern comp_physics_constructor_info *PHYSICS_CONSTRUCTOR_INFO;

void comp_physics_register(void);
void sys_physics_register(void);

comp_physics *sys_physics_add(const ecs_id eid, comp_physics_constructor_info *cpci);

