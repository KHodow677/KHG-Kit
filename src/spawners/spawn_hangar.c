#include "spawners/spawn_hangar.h"
#include "entity/entity.h"
#include "game_manager.h"
#include "generators/entities/hangar_generator.h"
#include <stdlib.h>

void spawn_hangar(float x, float y, float angle) {
  generic_entity *ge = malloc(sizeof(generic_entity));
  ge->type = ENTITY_TYPE_HANGAR;
  ge->hangar = (hangar){ 0 };
  generate_hangar(&ge->hangar, x, y, angle);
  utl_vector_push_back(ENTITY_LOOKUP, &ge);
}

