#pragma once

#include "entity/comp_destroyer.h"
#include "entity/comp_physics.h"
#include "entity/comp_renderer.h"
#include "entity/comp_rotator.h"
#include "entity/comp_selector.h"
#include "entity/comp_shooter.h"
#include "generators/elements/turret_base_generator.h"
#include "khg_ecs/ecs.h"

typedef struct {
  ecs_id entity;
  physics_info physics_info;
  renderer_info renderer_info;
  comp_destroyer *comp_destroyer;
  rotator_info rotator_info;
  shooter_info shooter_info;
  selector_info selector_info;
} turret_top;

void generate_turret_top(turret_top *tt, turret_base *tb, float x, float y, float angle);
void free_turret_top(turret_top *tt);

