#pragma once

#include "generators/elements/turret_base_generator.h"
#include "generators/elements/turret_top_generator.h"
#include "khg_ecs/ecs.h"

typedef struct {
  ecs_id entity;
  turret_base base;
  turret_top top;
} turret;

void generate_turret(turret *t, float x, float y);
void free_turret(turret *t);

