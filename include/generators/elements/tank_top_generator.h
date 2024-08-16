#pragma once

#include "entity/comp_destroyer.h"
#include "entity/comp_follower.h"
#include "entity/comp_physics.h"
#include "entity/comp_renderer.h"
#include "entity/comp_rotator.h"
#include "entity/comp_shooter.h"
#include "generators/elements/tank_body_generator.h"
#include "khg_ecs/ecs.h"

typedef struct {
  ecs_id entity;
  physics_info physics_info;
  renderer_info renderer_info;
  follower_info follower_info;
  destroyer_info destroyer_info;
  rotator_info rotator_info;
  shooter_info shooter_info;
} tank_top;

void generate_tank_top(tank_top *tt, ecs_ecs *ecs, tank_body *tb, float x, float y);
void free_tank_top(tank_top *tt);

