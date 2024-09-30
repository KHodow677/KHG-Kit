#include "spawners/spawn_berserker.h"
#include "game_manager.h"
#include "entity/entity.h"
#include "generators/entities/berserker_generator.h"

generic_entity *spawn_berserker(float x, float y, float angle) {
  generic_entity ge;
  utl_vector_push_back(ENTITY_LOOKUP, &ge);
  generic_entity *stored_ge = utl_vector_back(ENTITY_LOOKUP);
  stored_ge->type = ENTITY_TYPE_BERSERKER;
  stored_ge->berserker = (berserker){ 0 };
  generate_berserker(&stored_ge->berserker, x, y, angle);
  return stored_ge;
}

