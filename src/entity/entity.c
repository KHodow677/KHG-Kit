#include "entity/entity.h"

bool generic_entity_is_particle(generic_entity *entity) {
  return entity->type == ENTITY_TYPE_PARTICLE;
}

bool generic_entity_is_tank(generic_entity *entity) {
  return entity->type == ENTITY_TYPE_TANK;
}

