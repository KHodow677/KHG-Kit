#pragma once

#include "entity/comp_destroyer.h"
#include "entity/comp_physics.h"
#include "entity/comp_renderer.h"
#include "khg_ecs/ecs.h"

typedef struct {
  ecs_id entity;
  physics_info physics_info;
  renderer_info renderer_info;
  destroyer_info destroyer_info;
} hangar;

void generate_hangar(hangar *h, float x, float y);
void free_hangar(hangar *h);

