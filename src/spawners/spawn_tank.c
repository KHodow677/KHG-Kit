#include "spawners/spawn_tank.h"
#include "entity/ecs_manager.h"
#include "entity/entity.h"
#include "generators/entities/tank_generator.h"
#include <stdlib.h>

void spawn_tank() {
  generic_entity *ge = malloc(sizeof(generic_entity));
  ge->type = ENTITY_TYPE_TANK;
  ge->tank = (tank){ 0 };
  generate_tank(&ge->tank, ECS);
  utl_vector_push_back(ENTITY_LOOKUP, &ge);
}

