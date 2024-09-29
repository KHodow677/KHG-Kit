#pragma once

#include "generators/elements/berserker_clone_body_generator.h"
#include "generators/elements/berserker_clone_top_generator.h"
#include "khg_ecs/ecs.h"

typedef struct {
  ecs_id entity;
  berserker_clone_body body;
  berserker_clone_top top;
} berserker_clone;

void generate_berserker_clone(berserker_clone *bc, float x, float y, float angle);
void free_berserker_clone(berserker_clone *bc);
