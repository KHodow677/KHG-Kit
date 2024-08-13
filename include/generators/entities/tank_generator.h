#pragma once

#include "generators/elements/tank_body_generator.h"
#include "generators/elements/tank_top_generator.h"
#include "khg_ecs/ecs.h"

typedef struct {
  tank_body body;
  tank_top top;
} tank;

void generate_tank(tank *t, ecs_ecs *ecs);
void free_tank(tank *t);

