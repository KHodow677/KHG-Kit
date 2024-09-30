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
} berserker_clone_body;

void generate_berserker_clone_body(berserker_clone_body *bcb, float x, float y, float angle);
void free_berserker_clone_body(berserker_clone_body *bcb);

