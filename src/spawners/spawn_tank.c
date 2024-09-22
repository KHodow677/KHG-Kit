#include "spawners/spawn_tank.h"
#include "game_manager.h"
#include "entity/entity.h"
#include "generators/entities/tank_generator.h"
#include <stdlib.h>

generic_entity *spawn_tank(float x, float y) {
  generic_entity *ge = malloc(sizeof(generic_entity));
  ge->type = ENTITY_TYPE_TANK;
  ge->tank = (tank){ 0 };
  generate_tank(&ge->tank, x, y);
  utl_vector_push_back(ENTITY_LOOKUP, &ge);
  return ge;
}

