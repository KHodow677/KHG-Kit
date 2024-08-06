#include "comp_physics.h"
#include "khg_ecs/ecs.h"
#include "khg_phy/body.h"
#include "khg_phy/phy.h"
#include "khg_phy/phy_types.h"
#include "khg_utl/map.h"
#include <stdio.h>

ecs_id PHYSICS_COMPONENT_SIGNATURE;
map *PHYSICS_INFO_MAP = NULL;

int compare_ints(const key_type a, const key_type b) {
  const int* ia = (const int*)a;
  const int* ib = (const int*)b;
  return (*ia - *ib);
}

void no_deallocator(void *data) {}

void comp_physics_setup(comp_physics *cp, cpSpace *sp, bool collides) {
  cp->time = 0.0f;
  float width = 145.0f, height = 184.0f, mass = 1;
  float moment = cpMomentForBox(mass, width, height);
  cp->info.body = cpSpaceAddBody(sp, cpBodyNew(mass, moment));
  cpBodySetPosition(cp->info.body, cpv(200.0f, 150.0f));
  if (collides) {
    cp->info.shape = cpSpaceAddShape(sp, cpBoxShapeNew(cp->info.body, width, height, 0.0f));
  }
  else {
    cp->info.shape = cpSpaceAddShape(sp, cpBoxShapeNew(cp->info.body, 0.0f, 0.0f, 0.0f));
  }
  cpShapeSetFriction(cp->info.shape, 0.0f);
  cp->info.space = sp;
}

void comp_physics_free(comp_physics *cp) {
  cpShapeFree(cp->info.shape);
  cpBodyFree(cp->info.body);
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

void sys_physics_add(ecs_ecs *ecs, ecs_id *eid, ecs_id cid, physics_info *info) {
  ecs_add(ecs, *eid, cid, NULL);
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
  for (int id = 0; id < entity_count; id++) {
    comp_physics *cp = ecs_get(ecs, entities[id], PHYSICS_COMPONENT_SIGNATURE);
    physics_info *info = map_at(PHYSICS_INFO_MAP, &entities[id]);
    cp->time += dt; 
    cpSpaceStep(info->space, dt);
  }
  return 0;
}
