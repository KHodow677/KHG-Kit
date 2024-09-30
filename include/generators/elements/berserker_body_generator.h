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
} berserker_body;

void generate_berserker_body(berserker_body *bb, float x, float y, float angle);
void free_berserker_body(berserker_body *bb);

