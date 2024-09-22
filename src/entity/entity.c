#include "entity/entity.h"
#include "game_manager.h"
#include "khg_utl/vector.h"

static bool generic_entity_is_particle(generic_entity *entity) {
  return entity->type == ENTITY_TYPE_PARTICLE;
}

static bool generic_entity_is_tank(generic_entity *entity) {
  return entity->type == ENTITY_TYPE_TANK;
}

static bool generic_entity_is_turret(generic_entity *entity) {
  return entity->type == ENTITY_TYPE_TURRET;
}

static bool generic_entity_is_hangar(generic_entity *entity) {
  return entity->type == ENTITY_TYPE_HANGAR;
}

void free_entity(generic_entity *ge) {
  if (generic_entity_is_particle(ge)) {
    free_particle(&ge->particle);
  }
  else if (generic_entity_is_tank(ge)) {
    free_tank(&ge->tank);
  }
  else if (generic_entity_is_turret(ge)) {
    free_turret(&ge->turret);
  }
  else if (generic_entity_is_hangar(ge)) {
    free_hangar(&ge->hangar);
  }
}

void generate_entity_lookup() {
  ENTITY_LOOKUP = utl_vector_create(sizeof(generic_entity));
}

void free_entity_lookup() {
  for (int i = 0; i < utl_vector_size(ENTITY_LOOKUP); i++) {
    free_entity((generic_entity *)(utl_vector_at(ENTITY_LOOKUP, i)));
  }
  utl_vector_deallocate(ENTITY_LOOKUP);
}

