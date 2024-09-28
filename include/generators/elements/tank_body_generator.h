#pragma once

#include "entity/comp_destroyer.h"
#include "entity/comp_physics.h"
#include "entity/comp_renderer.h"
#include "khg_ecs/ecs.h"

typedef struct {
  ecs_id entity;
  physics_info physics_info;
  renderer_info renderer_info;
  comp_destroyer *comp_destroyer;
} tank_body;

void generate_tank_body(tank_body *tb, float x, float y, float angle);
void free_tank_body(tank_body *tb);

