#include "controllers/elements/element_destruction_controller.h"
#include "game_manager.h"
#include "entity/entity.h"
#include "khg_utl/vector.h"
#include <stdlib.h>

void element_destroy(ecs_id eid) {
  generic_entity *ge = NULL;
  for (int i = 0; i < utl_vector_size(ENTITY_LOOKUP); i++) {
    ge = *(generic_entity **)utl_vector_at(ENTITY_LOOKUP, i);
    if (ge->type == ENTITY_TYPE_PARTICLE) {
      if (ge->particle.entity == eid) {
        utl_vector_erase(ENTITY_LOOKUP, i, 1);
        free_particle(&ge->particle);
        free(ge);
        break; 
      }
      else if (ge->tank.entity == eid) {
        utl_vector_erase(ENTITY_LOOKUP, i, 1);
        free_tank(&ge->tank);
        free(ge);
        break; 
      }
      else if (ge->turret.entity == eid) {
        utl_vector_erase(ENTITY_LOOKUP, i, 1);
        free_turret(&ge->turret);
        free(ge);
        break; 
      }
      else if (ge->hangar.entity == eid) {
        utl_vector_erase(ENTITY_LOOKUP, i, 1);
        free_hangar(&ge->hangar);
        free(ge);
        break; 
      }
    }
  }
  ecs_queue_destroy(ECS, eid);
}

