#pragma once

#include "entity/comp_destroyer.h"
#include "entity/comp_physics.h"
#include "entity/comp_renderer.h"
#include "entity/comp_selector.h"
#include "entity/comp_spawn.h"
#include "khg_ecs/ecs.h"

typedef struct {
  ecs_id entity;
  comp_physics *comp_physics;
  comp_renderer *comp_renderer;
  comp_destroyer *comp_destroyer;
  comp_selector *comp_selector;
  comp_spawn *comp_spawn;
} hangar;

void generate_hangar(hangar *h, float x, float y, float angle);
void free_hangar(hangar *h);

