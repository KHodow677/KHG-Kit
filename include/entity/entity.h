#pragma once

#include "generators/elements/particle_generator.h"
#include "generators/elements/tank_body_generator.h"
#include "generators/elements/tank_top_generator.h"

typedef struct {
  enum {
    ENTITY_TYPE_NONE,
    ENTITY_TYPE_PARTICLE,
    ENTITY_TYPE_TANK_BODY,
    ENTITY_TYPE_TANK_TOP
  } type;

  union {
    particle *particle;
    tank_body *tank_body;
    tank_top *tank_top;
  };
} generic_entity;

bool generic_entity_is_particle(generic_entity *entity);
bool generic_entity_is_tank_body(generic_entity *entity);
bool generic_entity_is_tank_top(generic_entity *entity);

