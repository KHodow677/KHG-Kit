#pragma once

#include "entity/comp_destroyer.h"
#include "entity/comp_physics.h"
#include "entity/comp_renderer.h"
#include "khg_ecs/ecs.h"
#include "khg_phy/phy_types.h"

typedef struct {
  ecs_id entity;
  physics_info physics_info;
  renderer_info renderer_info;
  destroyer_info destroyer_info;
} tank_body;

void generate_tank_body(tank_body *tb, ecs_ecs *ecs, cpSpace *sp);
void free_tank_body(tank_body *tb, cpSpace *sp);

