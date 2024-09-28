#pragma once

#include "entity/comp_commander.h"
#include "entity/comp_damage.h"
#include "entity/comp_destroyer.h"
#include "entity/comp_mover.h"
#include "entity/comp_physics.h"
#include "entity/comp_renderer.h"
#include "entity/comp_rotator.h"
#include "entity/comp_selector.h"
#include "entity/comp_shooter.h"
#include "entity/comp_targeter.h"
#include "generators/elements/tank_body_generator.h"
#include "khg_ecs/ecs.h"

typedef struct {
  ecs_id entity;
  comp_physics *comp_physics;
  comp_renderer *comp_renderer;
  comp_destroyer *comp_destroyer;
  comp_mover *comp_mover;
  comp_rotator *comp_rotator;
  comp_shooter *comp_shooter;
  comp_selector *comp_selector;
  comp_commander *comp_commander;
  targeter_info targeter_info;
  comp_damage *comp_damage;
} tank_top;

void generate_tank_top(tank_top *tt, tank_body *tb, float x, float y, float angle);
void free_tank_top(tank_top *tt);

