#include "entity/entity.h"
#include "entity/ecs_manager.h"
#include "khg_utl/vector.h"
#include <stdlib.h>

bool generic_entity_is_particle(generic_entity *entity) {
  return entity->type == ENTITY_TYPE_PARTICLE;
}

bool generic_entity_is_tank(generic_entity *entity) {
  return entity->type == ENTITY_TYPE_TANK;
}

void free_entity(generic_entity *ge) {
  if (generic_entity_is_particle(ge)) {
    free_particle(&ge->particle);
  }
  else if (generic_entity_is_tank(ge)) {
    free_tank(&ge->tank);
  }
  free(ge);
}

void generate_entity_lookup() {
  ENTITY_LOOKUP = utl_vector_create(sizeof(generic_entity *));
}

void free_entity_lookup() {
  for (int i = 0; i < utl_vector_size(ENTITY_LOOKUP); i++) {
    free_entity(*(generic_entity **)(utl_vector_at(ENTITY_LOOKUP, i)));
  }
  utl_vector_deallocate(ENTITY_LOOKUP);
}

