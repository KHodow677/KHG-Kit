#include "spawners/spawn_tank_outline.h"
#include "game_manager.h"
#include "entity/entity.h"
#include "generators/entities/outline_generator.h"
#include <stdlib.h>

void spawn_tank_outline(phy_body *body_body, phy_body *top_body, float x, float y) {
  generic_entity *ge = malloc(sizeof(generic_entity));
  ge->type = ENTITY_TYPE_TANK_OUTLINE;
  ge->tank_outline = (tank_outline){ 0 };
  generate_tank_outline(&ge->tank_outline, body_body, top_body, x, y);
  utl_vector_push_back(ENTITY_LOOKUP, &ge);
}

