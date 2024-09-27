#include "entity/comp_rotator.h"
#include "controllers/elements/element_controller.h"
#include "entity/comp_physics.h"
#include "game_manager.h"
#include "khg_ecs/ecs.h"
#include "khg_phy/body.h"

ecs_id ROTATOR_COMPONENT_SIGNATURE;
rotator_info NO_ROTATOR = { 0 };
rotator_info *ROTATOR_INFO = (rotator_info[ECS_ENTITY_COUNT]){};

static ecs_ret sys_rotator_update(ecs_ecs *ecs, ecs_id *entities, int entity_count, ecs_dt dt, void *udata) {
  for (int id = 0; id < entity_count; id++) {
    rotator_info *info = &ROTATOR_INFO[entities[id]]; 
    physics_info *p_info = &PHYSICS_INFO[entities[id]];
    if (info->target_aim_body == NULL) {
      element_set_rotation_speed(p_info, 0.0f);
      continue;
    }
    element_lock_on_position(p_info, phy_body_get_position(info->target_aim_body), 16.0f);
  }
  return 0;
}

void comp_rotator_register(comp_rotator *cr) {
  cr->id = ecs_register_component(ECS, sizeof(comp_rotator), NULL, NULL);
  ROTATOR_COMPONENT_SIGNATURE = cr->id; 
}

void sys_rotator_register(sys_rotator *sr) {
  sr->id = ecs_register_system(ECS, sys_rotator_update, NULL, NULL, NULL);
  ecs_require_component(ECS, sr->id, ROTATOR_COMPONENT_SIGNATURE);
  ecs_require_component(ECS, sr->id, PHYSICS_COMPONENT_SIGNATURE);
  sr->ecs = *ECS;
  for (int i = 0; i < ECS_ENTITY_COUNT; i++) {
    ROTATOR_INFO[i] = NO_ROTATOR;
  }
}

void sys_rotator_add(ecs_id *eid, rotator_info *info) {
  ecs_add(ECS, *eid, ROTATOR_COMPONENT_SIGNATURE, NULL);
  ROTATOR_INFO[*eid] = *info;
}

