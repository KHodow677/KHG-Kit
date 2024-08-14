#include "spawners/spawn_tank.h"
#include "entity/ecs_manager.h"
#include "entity/entity.h"
#include "generators/entities/tank_generator.h"
#include <stdlib.h>

void spawn_tank() {
  generic_entity *ge = malloc(sizeof(generic_entity));
  tank *t = malloc(sizeof(tank));
  generate_tank(t, ECS);
  ge->type = ENTITY_TYPE_TANK;
  ge->tank = t;
  utl_vector_push_back(ENTITY_LOOKUP, &ge);
}

