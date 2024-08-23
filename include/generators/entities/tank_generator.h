#pragma once

#include "generators/elements/tank_body_generator.h"
#include "generators/elements/tank_top_generator.h"

typedef struct {
  tank_body body;
  tank_top top;
} tank;

void generate_tank(tank *t, float x, float y);
void free_tank(tank *t);
