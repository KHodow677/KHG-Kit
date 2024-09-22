#pragma once

#include "generators/entities/hangar_generator.h"
#include "generators/entities/particle_generator.h"
#include "generators/entities/tank_generator.h"
#include "generators/entities/turret_generator.h"

typedef struct {
  enum {
    ENTITY_TYPE_NONE,
    ENTITY_TYPE_PARTICLE,
    ENTITY_TYPE_TANK,
    ENTITY_TYPE_TURRET,
    ENTITY_TYPE_HANGAR,
  } type;
  union {
    particle particle;
    tank tank;
    turret turret;
    hangar hangar;
  };
} generic_entity;

bool free_entity(generic_entity *ge, bool check_id, ecs_id id);
void generate_entity_lookup(void);
void free_entity_lookup(void);

