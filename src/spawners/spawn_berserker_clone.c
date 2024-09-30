#include "spawners/spawn_berserker_clone.h"
#include "game_manager.h"
#include "entity/entity.h"
#include "generators/entities/berserker_clone_generator.h"

generic_entity *spawn_berserker_clone(comp_physics *ref, float x, float y, float angle) {
  generic_entity ge;
  utl_vector_push_back(ENTITY_LOOKUP, &ge);
  generic_entity *stored_ge = utl_vector_back(ENTITY_LOOKUP);
  stored_ge->type = ENTITY_TYPE_BERSERKER_CLONE;
  stored_ge->berserker_clone = (berserker_clone){ 0 };
  generate_berserker_clone(&stored_ge->berserker_clone, ref, x, y, angle);
  return stored_ge;
}

