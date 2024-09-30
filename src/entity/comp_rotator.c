#include "entity/comp_rotator.h"
#include "controllers/elements/element_controller.h"
#include "entity/comp_physics.h"
#include "game_manager.h"
#include "khg_ecs/ecs.h"
#include "khg_phy/body.h"

ecs_id ROTATOR_COMPONENT_SIGNATURE;

static ecs_ret sys_rotator_update(ecs_ecs *ecs, ecs_id *entities, int entity_count, ecs_dt dt, void *udata) {
  if (dt == 0.0f) {
    return 0;
  }
  for (int id = 0; id < entity_count; id++) {
    comp_rotator *info = ecs_get(ECS, entities[id], ROTATOR_COMPONENT_SIGNATURE);
    comp_physics *p_info = ecs_get(ECS, entities[id], PHYSICS_COMPONENT_SIGNATURE);
    if (info->target_health == NULL) {
      element_set_rotation_speed(p_info, 0.0f);
      continue;
    }
    element_lock_on_position(p_info, phy_body_get_position(info->target_health->body), 16.0f);
  }
  return 0;
}

void comp_rotator_register() {
  ROTATOR_COMPONENT_SIGNATURE = ecs_register_component(ECS, sizeof(comp_rotator), NULL, NULL);
}

void sys_rotator_register(sys_rotator *sr) {
  sr->id = ecs_register_system(ECS, sys_rotator_update, NULL, NULL, NULL);
  ecs_require_component(ECS, sr->id, ROTATOR_COMPONENT_SIGNATURE);
  ecs_require_component(ECS, sr->id, PHYSICS_COMPONENT_SIGNATURE);
  sr->ecs = *ECS;
}

comp_rotator *sys_rotator_add(ecs_id eid) {
  return ecs_add(ECS, eid, ROTATOR_COMPONENT_SIGNATURE, NULL);
}

