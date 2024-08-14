#include "controllers/elements//element_destruction_controller.h"
#include "entity/ecs_manager.h"
#include "entity/entity.h"
#include "khg_utl/vector.h"
#include <stdlib.h>

void element_destroy(ecs_ecs *ecs, ecs_id eid) {
  generic_entity *ge = NULL;
  for (int i = 0; i < utl_vector_size(ENTITY_LOOKUP); i++) {
    ge = *(generic_entity **)utl_vector_at(ENTITY_LOOKUP, i);
    if (ge->particle.entity == eid) {
      utl_vector_erase(ENTITY_LOOKUP, i, 1);
      free_particle(&ge->particle);
      free(ge);
      break; 
    }
  }
  ecs_queue_destroy(ecs, eid);
}

