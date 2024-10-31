#pragma once

#include "khg_ecs/ecs.h"
#include "khg_phy/phy_types.h"

typedef struct {
  ecs_id id;
  phy_body *body;
  phy_body *target_body;
  phy_constraint *pivot;
  bool has_constraint;
  float target_vel;
  float target_ang_vel;
  bool move_enabled;
  bool rotate_enabled;
  bool is_moving;
  bool is_turning;
} comp_physics;


typedef struct {
  enum {
    PHYSICS_BOX,
  } mode;
  float width;
  float height;
  float mass;
  phy_vect pos;
  float ang;
  phy_vect cog;
} comp_physics_constructor_info;

extern ecs_id PHYSICS_COMPONENT_SIGNATURE;
extern ecs_id PHYSICS_SYSTEM_SIGNATURE;

extern comp_physics_constructor_info *PHYSICS_CONSTRUCTOR_INFO;

void comp_physics_register(void);
void sys_physics_register(void);

comp_physics *sys_physics_add(const ecs_id eid, comp_physics_constructor_info *cpci);

