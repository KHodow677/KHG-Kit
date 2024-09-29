#pragma once

#include "generators/elements/berserker_body_generator.h"
#include "generators/elements/berserker_top_generator.h"
#include "khg_ecs/ecs.h"

typedef struct {
  ecs_id entity;
  berserker_body body;
  berserker_top top;
} berserker;

void generate_berserker(berserker *t, float x, float y, float angle);
void free_berserker(berserker *t);
