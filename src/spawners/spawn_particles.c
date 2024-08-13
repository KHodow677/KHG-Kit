#include "spawners/spawn_particles.h"
#include "entity/entity.h"
#include "entity/ecs_manager.h"
#include "generators/elements/particle_generator.h"
#include "khg_utl/map.h"
#include <stdlib.h>

void spawn_particle() {
  generic_entity *ge = malloc(sizeof(generic_entity));
  int *i = malloc(sizeof(int));
  particle *p = malloc(sizeof(particle));
  *i = utl_map_size(ENTITY_LOOKUP);
  generate_particle(p, ECS);
  ge->type = ENTITY_TYPE_PARTICLE;
  ge->particle = p;
  utl_map_insert(ENTITY_LOOKUP, i, ge);
}

