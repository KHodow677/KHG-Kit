#pragma once

#include "generators/entities/outline_generator.h"
#include "generators/entities/particle_generator.h"
#include "generators/entities/tank_generator.h"

typedef struct {
  enum {
    ENTITY_TYPE_NONE,
    ENTITY_TYPE_PARTICLE,
    ENTITY_TYPE_TANK,
    ENTITY_TYPE_TANK_OUTLINE,
  } type;
  union {
    particle particle;
    tank tank;
    tank_outline tank_outline;
  };
} generic_entity;

void free_entity(generic_entity *ge);
void generate_entity_lookup(void);
void free_entity_lookup(void);

