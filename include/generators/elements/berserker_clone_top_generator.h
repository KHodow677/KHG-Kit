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
#include "generators/elements/berserker_clone_body_generator.h"
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
  comp_targeter *comp_targeter;
  comp_damage *comp_damage;
} berserker_clone_top;

void generate_berserker_clone_top(berserker_clone_top *bct, berserker_clone_body *bcb, float x, float y, float angle);
void free_berserker_clone_top(berserker_clone_top *bct);

