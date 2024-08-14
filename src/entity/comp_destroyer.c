#include "entity/comp_destroyer.h"
#include "controllers/elements/element_destruction_controller.h"
#include "data_utl/map_utl.h"
#include "entity/ecs_manager.h"
#include "entity/entity.h"
#include "generators/entities/particle_generator.h"
#include "khg_ecs/ecs.h"
#include "khg_utl/map.h"
#include "khg_utl/vector.h"
#include <stdio.h>
#include <stdlib.h>

ecs_id DESTROYER_COMPONENT_SIGNATURE;
utl_map *DESTROYER_INFO_MAP = NULL;

void comp_destroyer_register(comp_destroyer *cd, ecs_ecs *ecs) {
  cd->id = ecs_register_component(ecs, sizeof(comp_destroyer), NULL, NULL);
  DESTROYER_COMPONENT_SIGNATURE = cd->id; 
}

void sys_destroyer_register(sys_destroyer *sd, ecs_ecs *ecs) {
  sd->id = ecs_register_system(ecs, sys_destroyer_update, NULL, NULL, NULL);
  ecs_require_component(ecs, sd->id, DESTROYER_COMPONENT_SIGNATURE);
  sd->ecs = *ecs;
  DESTROYER_INFO_MAP = utl_map_create(compare_ints, no_deallocator, no_deallocator);
}

void sys_destroyer_add(ecs_ecs *ecs, ecs_id *eid, destroyer_info *info) {
  ecs_add(ecs, *eid, DESTROYER_COMPONENT_SIGNATURE, NULL);
  utl_map_insert(DESTROYER_INFO_MAP, eid, info);
}

void sys_destroyer_free(bool need_free) {
  if (need_free) {
    utl_map_deallocate(DESTROYER_INFO_MAP);
  }
}

ecs_ret sys_destroyer_update(ecs_ecs *ecs, ecs_id *entities, int entity_count, ecs_dt dt, void *udata) {
  (void)ecs;
  (void)dt;
  (void)udata;
  if (entity_count == 0) {
    return 0;
  }
  destroyer_info *info = utl_map_at(DESTROYER_INFO_MAP, &entities[0]);
  generic_entity *ge = NULL;
  for (int id = 0; id < entity_count; id++) {
    info = utl_map_at(DESTROYER_INFO_MAP, &entities[id]);
    if (info->destroy_now) {
      element_destroy(ecs, entities[id]);
    }
  }
  return 0;
}
