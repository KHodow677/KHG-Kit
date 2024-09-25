#include "entity/entity.h"
#include "game_manager.h"
#include "generators/entities/spawner_generator.h"
#include "khg_utl/vector.h"
#include <stdio.h>

bool free_entity(generic_entity *ge, bool check_id, ecs_id id) {
  if (ge->type == ENTITY_TYPE_PARTICLE) {
    if (!check_id || ge->particle.entity == id) {
      free_particle(&ge->particle);
      return true;
    }
  }
  else if (ge->type == ENTITY_TYPE_TANK) {
    if (!check_id || ge->tank.entity == id) {
      free_tank(&ge->tank);
      return true;
    }
  }
  else if (ge->type == ENTITY_TYPE_TURRET) {
    if (!check_id || ge->turret.entity == id) {
      free_turret(&ge->turret);
      return true;
    }
  }
  else if (ge->type == ENTITY_TYPE_HANGAR) {
    if (!check_id || ge->hangar.entity == id) {
      free_hangar(&ge->hangar);
      return true;
    }
  }
  else if (ge->type == ENTITY_TYPE_SPAWNER) {
    if (!check_id || ge->spawner.entity == id) {
      free_spawner(&ge->spawner);
      return true;
    }
  }
  else if (ge->type == ENTITY_TYPE_SLUG) {
    if (!check_id || ge->hangar.entity == id) {
      free_slug(&ge->slug);
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

