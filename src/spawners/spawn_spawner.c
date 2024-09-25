#include "spawners/spawn_spawner.h"
#include "entity/entity.h"
#include "game_manager.h"
#include "generators/entities/spawner_generator.h"

generic_entity *spawn_spawner(float x, float y, float angle) {
  generic_entity ge;
  utl_vector_push_back(ENTITY_LOOKUP, &ge);
  generic_entity *stored_ge = utl_vector_back(ENTITY_LOOKUP);
  stored_ge->type = ENTITY_TYPE_SPAWNER;
  stored_ge->spawner = (spawner){ 0 };
  generate_spawner(&stored_ge->spawner, x, y, angle);
  utl_vector_push_back(ENTITY_LOOKUP, &ge);
  return stored_ge;
}

