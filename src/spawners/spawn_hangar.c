#include "spawners/spawn_hangar.h"
#include "entity/entity.h"
#include "game_manager.h"
#include "generators/entities/hangar_generator.h"

generic_entity *spawn_hangar(float x, float y, float angle) {
  generic_entity ge;
  utl_vector_push_back(ENTITY_LOOKUP, &ge);
  generic_entity *stored_ge = utl_vector_back(ENTITY_LOOKUP);
  stored_ge->type = ENTITY_TYPE_HANGAR;
  stored_ge->hangar = (hangar){ 0 };
  generate_hangar(&stored_ge->hangar, x, y, angle);
  utl_vector_push_back(ENTITY_LOOKUP, &ge);
  return stored_ge;
}

