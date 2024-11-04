#pragma once

#include "khg_ecs/ecs.h"
#include "khg_phy/phy_types.h"

typedef struct comp_mover {
  ecs_id id;
  phy_vect target;
  float target_vel;
  float max_vel;
  int idle_min_tex_id;
  int idle_max_tex_id;
  int walk_min_tex_id;
  int walk_max_tex_id;
} comp_mover;

typedef struct comp_mover_constructor_info {
  const phy_body *body; 
  const float target_vel;
  const float max_vel;
  const int idle_min_tex_id;
  const int idle_max_tex_id;
  const int walk_min_tex_id;
  const int walk_max_tex_id;
} comp_mover_constructor_info;

extern ecs_id MOVER_COMPONENT_SIGNATURE;
extern ecs_id MOVER_SYSTEM_SIGNATURE;

extern comp_mover_constructor_info *MOVER_CONSTRUCTOR_INFO;

void comp_mover_register(void);
void sys_mover_register(void);

comp_mover *sys_mover_add(const ecs_id eid, comp_mover_constructor_info *clci);

