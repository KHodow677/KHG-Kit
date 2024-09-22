#include "spawners/spawn_tank.h"
#include "game_manager.h"
#include "entity/entity.h"
#include "generators/entities/tank_generator.h"

generic_entity *spawn_tank(float x, float y, float angle) {
  generic_entity ge;
  utl_vector_push_back(ENTITY_LOOKUP, &ge);
  generic_entity *stored_ge = utl_vector_back(ENTITY_LOOKUP);
  stored_ge->type = ENTITY_TYPE_TANK;
  stored_ge->tank = (tank){ 0 };
  generate_tank(&stored_ge->tank, x, y, angle);
  return stored_ge;
}

