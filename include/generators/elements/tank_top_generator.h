#pragma once

#include "entity/comp_destroyer.h"
#include "entity/comp_follower.h"
#include "entity/comp_mover.h"
#include "entity/comp_physics.h"
#include "entity/comp_renderer.h"
#include "generators/elements/tank_body_generator.h"
#include "khg_ecs/ecs.h"

typedef struct {
  ecs_id entity;
  physics_info physics_info;
  renderer_info renderer_info;
  follower_info follower_info;
  destroyer_info destroyer_info;
  mover_info mover_info;
  bool is_locked_on;
} tank_top;

void generate_tank_top(tank_top *tt, ecs_ecs *ecs, tank_body *tb);
void free_tank_top(tank_top *tt);

