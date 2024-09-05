#pragma once

#include "entity/comp_destroyer.h"
#include "entity/comp_mover.h"
#include "entity/comp_physics.h"
#include "entity/comp_renderer.h"
#include "entity/comp_selector.h"
#include "khg_ecs/ecs.h"

typedef struct {
  ecs_id entity;
  physics_info physics_info;
  renderer_info renderer_info;
  destroyer_info destroyer_info;
  mover_info mover_info;
  selector_info selector_info;
} tank_body;

void generate_tank_body(tank_body *tb, float x, float y);
void free_tank_body(tank_body *tb);

