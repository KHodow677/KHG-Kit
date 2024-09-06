#pragma once

#include "entity/comp_destroyer.h"
#include "entity/comp_physics.h"
#include "entity/comp_renderer.h"
#include "khg_ecs/ecs.h"

typedef struct {
  ecs_id entity;
  physics_info physics_info;
  renderer_info renderer_info;
  destroyer_info destroyer_info;
} tank_outline;

void generate_tank_outline(tank_outline *to, phy_body *body_body, phy_body *top_body, float x, float y);
void free_tank_outline(tank_outline *to);

