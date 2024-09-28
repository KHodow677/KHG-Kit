#pragma once

#include "entity/comp_destroyer.h"
#include "entity/comp_physics.h"
#include "entity/comp_renderer.h"
#include "entity/comp_rotator.h"
#include "entity/comp_shooter.h"
#include "generators/elements/turret_base_generator.h"
#include "khg_ecs/ecs.h"

typedef struct {
  ecs_id entity;
  comp_physics *comp_physics;
  comp_renderer *comp_renderer;
  comp_destroyer *comp_destroyer;
  comp_rotator *comp_rotator;
  comp_shooter *comp_shooter;
} turret_top;

void generate_turret_top(turret_top *tt, turret_base *tb, float x, float y, float angle);
void free_turret_top(turret_top *tt);

