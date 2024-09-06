#pragma once

#include "generators/elements/tank_body_generator.h"
#include "generators/elements/tank_top_generator.h"
#include "khg_ecs/ecs.h"

typedef struct {
  ecs_id entity;
  tank_body body;
  tank_top top;
} tank;

void generate_tank(tank *t, float x, float y);
void free_tank(tank *t);
