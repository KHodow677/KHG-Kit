#include "entity/comp_physics.h"
#include "data_utl/kinematic_utl.h"
#include "entity/ecs_manager.h"
#include "khg_ecs/ecs.h"
#include "khg_phy/body.h"
#include "khg_phy/vect.h"
#include "khg_utl/vector.h"
#include <math.h>
#include <stdio.h>

ecs_id PHYSICS_COMPONENT_SIGNATURE;
physics_info NO_PHYSICS = { 0 };
utl_vector *PHYSICS_INFO = NULL;

void comp_physics_register(comp_physics *cp, ecs_ecs *ecs) {
  cp->id = ecs_register_component(ecs, sizeof(comp_physics), NULL, NULL);
  PHYSICS_COMPONENT_SIGNATURE = cp->id; 
}

void sys_physics_register(sys_physics *sp, ecs_ecs *ecs) {
  sp->id = ecs_register_system(ecs, sys_physics_update, NULL, NULL, NULL);
  ecs_require_component(ecs, sp->id, PHYSICS_COMPONENT_SIGNATURE);
  sp->ecs = *ecs;
  PHYSICS_INFO = utl_vector_create(sizeof(physics_info));
  for (int i = 0; i < ECS->entity_count; i++) {
    utl_vector_push_back(PHYSICS_INFO, &NO_PHYSICS);
  }
}

void sys_physics_add(ecs_ecs *ecs, ecs_id *eid, physics_info *info) {
  ecs_add(ecs, *eid, PHYSICS_COMPONENT_SIGNATURE, NULL);
  utl_vector_assign(PHYSICS_INFO, *eid, info);
}

void sys_physics_free(bool need_free) {
  if (need_free) {
    utl_vector_deallocate(PHYSICS_INFO);
  }
}

ecs_ret sys_physics_update(ecs_ecs *ecs, ecs_id *entities, int entity_count, ecs_dt dt, void *udata) {
  physics_info *info;
  for (int id = 0; id < entity_count; id++) {
    info = utl_vector_at(PHYSICS_INFO, entities[id]);
    float current_ang = normalize_angle(phy_body_get_angle(info->body));
    phy_body_set_velocity(info->body, cpv(sinf(current_ang)*info->target_vel, -cosf(current_ang)*info->target_vel));
    phy_body_set_angular_velocity(info->body, info->target_ang_vel);
  }
  return 0;
}
