#pragma once

#include "entity/comp_destroyer.h"
#include "entity/comp_physics.h"
#include "entity/comp_renderer.h"
#include "khg_ecs/ecs.h"

typedef struct {
  ecs_id entity;
  comp_physics *comp_physics;
  comp_renderer *comp_renderer;
  comp_destroyer *comp_destroyer;
} turret_base;

void generate_turret_base(turret_base *tb, float x, float y, float angle);
void free_turret_base(turret_base *tb);

