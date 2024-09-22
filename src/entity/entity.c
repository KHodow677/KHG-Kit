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

bool free_entity(generic_entity *ge, bool check_id, ecs_id id) {
  if (generic_entity_is_particle(ge)) {
    if (!check_id || ge->particle.entity == id) {
      free_particle(&ge->particle);
      return true;
    }
  }
  else if (generic_entity_is_tank(ge)) {
    if (!check_id || ge->tank.entity == id) {
      free_tank(&ge->tank);
      return true;
    }
  }
  else if (generic_entity_is_turret(ge)) {
    if (!check_id || ge->turret.entity == id) {
      free_turret(&ge->turret);
      return true;
    }
  }
  else if (generic_entity_is_hangar(ge)) {
    if (!check_id || ge->hangar.entity == id) {
      free_hangar(&ge->hangar);
      return true;
    }
  }
  return false;
}

void generate_entity_lookup() {
  ENTITY_LOOKUP = utl_vector_create(sizeof(generic_entity));
}

void free_entity_lookup() {
  for (int i = 0; i < utl_vector_size(ENTITY_LOOKUP); i++) {
    free_entity((generic_entity *)(utl_vector_at(ENTITY_LOOKUP, i)), false, 0);
  }
  utl_vector_deallocate(ENTITY_LOOKUP);
}

