#pragma once

#include "entity/comp_physics.h"
#include "entity/comp_renderer.h"
#include "khg_ecs/ecs.h"
#include "khg_phy/phy_types.h"

typedef struct {
  ecs_id entity;
  physics_info physics_info;
  renderer_info renderer_info;
} tank_top;

void generate_tank_top(tank_top *tt, ecs_ecs *ecs, cpSpace *sp);
void free_tank_top(tank_top *tt);

