#include "entity/entity.h"
#include "game_manager.h"
#include "generators/entities/berserker_clone_generator.h"
#include "generators/entities/spawner_generator.h"
#include "khg_utl/vector.h"

bool free_entity(generic_entity *ge, bool check_id, ecs_id id) {
  if (ge->type == ENTITY_TYPE_PARTICLE) {
    if (!check_id || ge->particle.entity == id) {
      free_particle(&ge->particle);
      return true;
    }
  }
  else if (ge->type == ENTITY_TYPE_BERSERKER) {
    if (!check_id || ge->berserker.entity == id) {
      free_berserker(&ge->berserker);
      return true;
    }
  }
  else if (ge->type == ENTITY_TYPE_BERSERKER_CLONE) {
    if (!check_id || ge->berserker_clone.entity == id) {
      free_berserker_clone(&ge->berserker_clone);
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
  else if (ge->type == ENTITY_TYPE_DEPOT) {
    if (!check_id || ge->hangar.entity == id) {
      free_depot(&ge->depot);
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

