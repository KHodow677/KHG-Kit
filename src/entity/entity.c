#include "entity/entity.h"

bool generic_entity_is_particle(generic_entity *entity) {
  return entity->type == ENTITY_TYPE_PARTICLE;
}

bool generic_entity_is_tank_body(generic_entity *entity) {
  return entity->type == ENTITY_TYPE_TANK_BODY;
}

bool generic_entity_is_tank_top(generic_entity *entity) {
  return entity->type == ENTITY_TYPE_TANK_TOP;
}

