#include "spawners/spawn_tank.h"
#include "data_utl/id_utl.h"
#include "entity/ecs_manager.h"
#include "entity/entity.h"
#include "generators/entities/tank_generator.h"
#include <stdlib.h>

void spawn_tank(){
  generic_entity *ge = malloc(sizeof(generic_entity));
  tank *t = malloc(sizeof(tank));
  generate_tank(t, ECS);
  ge->type = ENTITY_TYPE_TANK;
  ge->tank = t;
  int i = generate_unique_id();
  ge->tank->body.destroyer_info.unique_id = i;
  ge->tank->top.destroyer_info.unique_id = i;
  utl_map_insert(ENTITY_LOOKUP, &ge->tank->body.destroyer_info.unique_id, ge);
}

