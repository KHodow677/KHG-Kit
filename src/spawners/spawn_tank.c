#include "spawners/spawn_tank.h"
#include "entity/ecs_manager.h"
#include "entity/entity.h"
#include "generators/entities/tank_generator.h"
#include <stdlib.h>

void spawn_tank(){
  generic_entity *ge = malloc(sizeof(generic_entity));
  int *i = malloc(sizeof(int));
  tank *t = malloc(sizeof(tank));
  *i = utl_map_size(ENTITY_LOOKUP);
  generate_tank(t, ECS);
  ge->type = ENTITY_TYPE_TANK;
  ge->tank = t;
  utl_map_insert(ENTITY_LOOKUP, i, ge);
}

