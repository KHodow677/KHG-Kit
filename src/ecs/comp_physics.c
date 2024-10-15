#include "ecs/comp_physics.h"
#include "ecs/ecs_manager.h"
#include "utility/math_utl.h"
#include "khg_phy/body.h"
#include "khg_phy/vect.h"

ecs_id PHYSICS_COMPONENT_SIGNATURE;
ecs_id PHYSICS_SYSTEM_SIGNATURE;

static ecs_ret sys_physics_update(ecs_ecs *ecs, ecs_id *entities, int entity_count, ecs_dt dt, void *udata) {
  if (dt == 0.0f) {
    return 0;
  }
  for (int id = 0; id < entity_count; id++) {
    comp_physics *info = ecs_get(ECS, entities[id], PHYSICS_COMPONENT_SIGNATURE);
    float current_ang = normalize_angle(phy_body_get_angle(info->body));
    if (info->move_enabled) {
      phy_body_set_velocity(info->body, phy_v(sinf(current_ang)*info->target_vel, -cosf(current_ang)*info->target_vel));
    }
    if (info->rotate_enabled) {
      phy_body_set_angular_velocity(info->body, info->target_ang_vel);
    }
  }
  return 0;
}

void comp_physics_register() {
  PHYSICS_COMPONENT_SIGNATURE = ecs_register_component(ECS, sizeof(comp_physics), NULL, NULL);
}

void sys_physics_register() {
  PHYSICS_SYSTEM_SIGNATURE = ecs_register_system(ECS, sys_physics_update, NULL, NULL, NULL);
  ecs_require_component(ECS, PHYSICS_SYSTEM_SIGNATURE, PHYSICS_COMPONENT_SIGNATURE);
}

comp_physics *sys_physics_add(ecs_id eid) {
  return ecs_add(ECS, eid, PHYSICS_COMPONENT_SIGNATURE, NULL);
}
