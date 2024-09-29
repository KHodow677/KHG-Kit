#include "spawners/spawn_barrier.h"
#include "generators/entities/barrier_generator.h"
#include "entity/entity.h"
#include "game_manager.h"

generic_entity *spawn_barrier(float x, float y, float angle) {
  generic_entity ge;
  utl_vector_push_back(ENTITY_LOOKUP, &ge);
  generic_entity *stored_ge = utl_vector_back(ENTITY_LOOKUP);
  stored_ge->type = ENTITY_TYPE_BARRIER;
  stored_ge->barrier = (barrier){ 0 };
  generate_barrier(&stored_ge->barrier, x, y, angle);
  utl_vector_push_back(ENTITY_LOOKUP, &ge);
  return stored_ge;
}

