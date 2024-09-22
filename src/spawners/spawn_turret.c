#include "spawners/spawn_turret.h"
#include "game_manager.h"
#include "entity/entity.h"
#include "generators/entities/turret_generator.h"

generic_entity *spawn_turret(float x, float y, float angle) {
  generic_entity ge;
  utl_vector_push_back(ENTITY_LOOKUP, &ge);
  generic_entity *stored_ge = utl_vector_back(ENTITY_LOOKUP);
  stored_ge->type = ENTITY_TYPE_TURRET;
  stored_ge->turret = (turret){ 0 };
  generate_turret(&stored_ge->turret, x, y, angle);
  return stored_ge;
}

