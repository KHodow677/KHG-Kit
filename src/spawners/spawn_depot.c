#include "spawners/spawn_depot.h"
#include "entity/entity.h"
#include "game_manager.h"
#include "generators/entities/depot_generator.h"

generic_entity *spawn_depot(float x, float y, float angle) {
  generic_entity ge;
  utl_vector_push_back(ENTITY_LOOKUP, &ge);
  generic_entity *stored_ge = utl_vector_back(ENTITY_LOOKUP);
  stored_ge->type = ENTITY_TYPE_DEPOT;
  stored_ge->depot = (depot){ 0 };
  generate_depot(&stored_ge->depot, x, y, angle);
  utl_vector_push_back(ENTITY_LOOKUP, &ge);
  return stored_ge;
}

