#include "controllers/elements/element_destruction_controller.h"
#include "game_manager.h"
#include "entity/entity.h"
#include "khg_utl/vector.h"
#include <stdio.h>
#include <stdlib.h>

void element_destroy(ecs_id eid) {
  generic_entity *ge = NULL;
  for (int i = 0; i < utl_vector_size(ENTITY_LOOKUP); i++) {
    ge = (generic_entity *)utl_vector_at(ENTITY_LOOKUP, i);
    if (free_entity(ge, true, eid)) {
      utl_vector_erase(ENTITY_LOOKUP, i, 1);
      break;
    }
  }
  ecs_destroy(ECS, eid);
}

