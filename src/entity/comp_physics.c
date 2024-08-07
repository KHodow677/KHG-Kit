#include "comp_physics.h"
#include "../data_utl/map_utl.h"
#include "khg_ecs/ecs.h"
#include "khg_phy/phy.h"
#include "khg_phy/phy_types.h"
#include "khg_utl/map.h"
#include <stdio.h>

ecs_id PHYSICS_COMPONENT_SIGNATURE;
map *PHYSICS_INFO_MAP = NULL;

void info_physics_setup(physics_info *info, cpSpace *sp, bool collides) {
  float width = 145.0f, height = 184.0f, mass = 1;
  float moment = cpMomentForBox(mass, width, height);
  info->body = cpSpaceAddBody(sp, cpBodyNew(mass, moment));
  cpBodySetPosition(info->body, cpv(200.0f, 150.0f));
  if (collides) {
    info->shape = cpSpaceAddShape(sp, cpBoxShapeNew(info->body, width, height, 0.0f));
  }
  else {
    info->shape = cpSpaceAddShape(sp, cpBoxShapeNew(info->body, 0.0f, 0.0f, 0.0f));
  }
  cpShapeSetFriction(info->shape, 0.0f);
  info->space = sp;
}

void info_physics_free(physics_info *info) {
  cpShapeFree(info->shape);
  cpBodyFree(info->body);
}

void comp_physics_register(comp_physics *cp, ecs_ecs *ecs) {
  cp->id = ecs_register_component(ecs, sizeof(comp_physics), NULL, NULL);
  PHYSICS_COMPONENT_SIGNATURE = cp->id; 
}

void sys_physics_register(sys_physics *sp, ecs_ecs *ecs) {
  sp->id = ecs_register_system(ecs, sys_physics_update, NULL, NULL, NULL);
  ecs_require_component(ecs, sp->id, PHYSICS_COMPONENT_SIGNATURE);
  sp->ecs = *ecs;
  PHYSICS_INFO_MAP = map_create(compare_ints, no_deallocator, no_deallocator);
}

void sys_physics_add(ecs_ecs *ecs, ecs_id *eid, physics_info *info) {
  ecs_add(ecs, *eid, PHYSICS_COMPONENT_SIGNATURE, NULL);
  map_insert(PHYSICS_INFO_MAP, eid, info);
}

void sys_physics_free(bool need_free) {
  if (need_free) {
    map_deallocate(PHYSICS_INFO_MAP);
  }
}

ecs_ret sys_physics_update(ecs_ecs *ecs, ecs_id *entities, int entity_count, ecs_dt dt, void *udata) {
  (void)ecs;
  (void)dt;
  (void)udata;
  if (entity_count == 0) {
    return 0;
  }
  physics_info *info = map_at(PHYSICS_INFO_MAP, &entities[0]);
  for (int id = 0; id < entity_count; id++) {
    info = map_at(PHYSICS_INFO_MAP, &entities[id]);
  }
  cpSpaceStep(info->space, dt);
  return 0;
}
