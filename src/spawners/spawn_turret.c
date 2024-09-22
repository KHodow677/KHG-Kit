#include "spawners/spawn_turret.h"
#include "game_manager.h"
#include "entity/entity.h"
#include "generators/entities/turret_generator.h"
#include <stdlib.h>

generic_entity *spawn_turret(float x, float y) {
  generic_entity *ge = malloc(sizeof(generic_entity));
  ge->type = ENTITY_TYPE_TURRET;
  ge->turret = (turret){ 0 };
  generate_turret(&ge->turret, x, y);
  utl_vector_push_back(ENTITY_LOOKUP, &ge);
  return ge;
}

