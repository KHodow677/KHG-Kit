#pragma once

#include "generators/entities/depot_generator.h"
#include "generators/entities/hangar_generator.h"
#include "generators/entities/particle_generator.h"
#include "generators/entities/slug_generator.h"
#include "generators/entities/spawner_generator.h"
#include "generators/entities/berserker_generator.h"
#include "generators/entities/turret_generator.h"

typedef struct {
  enum {
    ENTITY_TYPE_NONE,
    ENTITY_TYPE_PARTICLE,
    ENTITY_TYPE_TANK,
    ENTITY_TYPE_TURRET,
    ENTITY_TYPE_HANGAR,
    ENTITY_TYPE_SPAWNER,
    ENTITY_TYPE_SLUG,
    ENTITY_TYPE_DEPOT,
  } type;
  union {
    particle particle;
    berserker berserker;
    turret turret;
    hangar hangar;
    spawner spawner;
    slug slug;
    depot depot;
  };
} generic_entity;

bool free_entity(generic_entity *ge, bool check_id, ecs_id id);
void generate_entity_lookup(void);
void free_entity_lookup(void);

