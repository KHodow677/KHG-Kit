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
} turret_base;

void generate_turret_base(turret_base *tb, float x, float y, float angle);
void free_turret_base(turret_base *tb);

