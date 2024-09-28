#pragma once

#include "entity/comp_destroyer.h"
#include "entity/comp_physics.h"
#include "entity/comp_renderer.h"
#include "entity/comp_selector.h"
#include "khg_ecs/ecs.h"

typedef struct {
  ecs_id entity;
  comp_physics *comp_physics;
  renderer_info renderer_info;
  comp_destroyer *comp_destroyer;
  selector_info selector_info;
} hangar;

void generate_hangar(hangar *h, float x, float y, float angle);
void free_hangar(hangar *h);

