#pragma once

#include "generators/entities/particle_generator.h"
#include "generators/entities/tank_generator.h"

typedef struct {
  enum {
    ENTITY_TYPE_NONE,
    ENTITY_TYPE_PARTICLE,
    ENTITY_TYPE_TANK,
  } type;
  union {
    particle particle;
    tank tank;
  };
} generic_entity;

bool generic_entity_is_particle(generic_entity *entity);
bool generic_entity_is_tank(generic_entity *entity);
void free_entity(generic_entity *ge);
void generate_entity_lookup(void);
void free_entity_lookup(void);

