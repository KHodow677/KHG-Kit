#include "entity/comp_physics.h"
#include "data_utl/kinematic_utl.h"
#include "data_utl/map_utl.h"
#include "khg_ecs/ecs.h"
#include "khg_phy/body.h"
#include "khg_phy/phy_types.h"
#include "khg_phy/space.h"
#include "khg_phy/vect.h"
#include "khg_utl/map.h"
#include <math.h>
#include <stdio.h>

ecs_id PHYSICS_COMPONENT_SIGNATURE;
utl_map *PHYSICS_INFO_MAP = NULL;

void comp_physics_register(comp_physics *cp, ecs_ecs *ecs) {
  cp->id = ecs_register_component(ecs, sizeof(comp_physics), NULL, NULL);
  PHYSICS_COMPONENT_SIGNATURE = cp->id; 
}

void sys_physics_register(sys_physics *sp, ecs_ecs *ecs) {
  sp->id = ecs_register_system(ecs, sys_physics_update, NULL, NULL, NULL);
  ecs_require_component(ecs, sp->id, PHYSICS_COMPONENT_SIGNATURE);
  sp->ecs = *ecs;
  PHYSICS_INFO_MAP = utl_map_create(compare_ints, no_deallocator, no_deallocator);
}

void sys_physics_add(ecs_ecs *ecs, ecs_id *eid, physics_info *info) {
  ecs_add(ecs, *eid, PHYSICS_COMPONENT_SIGNATURE, NULL);
  utl_map_insert(PHYSICS_INFO_MAP, eid, info);
}

void sys_physics_free(bool need_free) {
  if (need_free) {
    utl_map_deallocate(PHYSICS_INFO_MAP);
  }
}

ecs_ret sys_physics_update(ecs_ecs *ecs, ecs_id *entities, int entity_count, ecs_dt dt, void *udata) {
  (void)ecs;
  (void)dt;
  (void)udata;
  if (entity_count == 0) {
    return 0;
  }
  physics_info *info = utl_map_at(PHYSICS_INFO_MAP, &entities[0]);
  for (int id = 0; id < entity_count; id++) {
    info = utl_map_at(PHYSICS_INFO_MAP, &entities[id]);
    cpFloat current_ang = normalize_angle(cpBodyGetAngle(info->body));
    cpBodySetVelocity(info->body, cpv(sinf(current_ang)*info->target_vel, -cosf(current_ang)*info->target_vel));
    cpBodySetAngularVelocity(info->body, info->target_ang_vel);
  }
  cpSpaceStep(info->space, dt);
  return 0;
}
