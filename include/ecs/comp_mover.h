#pragma once

#include "khg_ecs/ecs.h"
#include "khg_phy/body.h"
#include "khg_phy/core/phy_vector.h"

typedef struct comp_mover {
  ecs_id id;
  phy_vector2 target;
  float target_vel;
  float max_vel;
  int idle_min_tex_id;
  int idle_max_tex_id;
  int walk_min_tex_id;
  int walk_max_tex_id;
} comp_mover;

typedef struct comp_mover_constructor_info {
  const phy_rigid_body *body; 
  const float target_vel;
  const float max_vel;
} comp_mover_constructor_info;

extern ecs_id MOVER_COMPONENT_SIGNATURE;
extern ecs_id MOVER_SYSTEM_SIGNATURE;

extern comp_mover_constructor_info *MOVER_CONSTRUCTOR_INFO;

void comp_mover_register(void);
void sys_mover_register(void);

comp_mover *sys_mover_add(const ecs_id eid, comp_mover_constructor_info *clci);

