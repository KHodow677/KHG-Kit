#include "ecs/comp_light.h"
#include "camera/camera.h"
#include "ecs/comp_physics.h"
#include "ecs/ecs_manager.h"
#include "graphics/light.h"
#include "khg_ecs/ecs.h"
#include "khg_phy/body.h"
#include "khg_phy/phy_types.h"
#include "khg_phy/vect.h"
#include <stdio.h>

ecs_id LIGHT_COMPONENT_SIGNATURE;
ecs_id LIGHT_SYSTEM_SIGNATURE;

comp_light_constructor_info *LIGHT_CONSTRUCTOR_INFO = NULL;

static ecs_ret sys_light_update(ecs_ecs *ecs, ecs_id *entities, const int entity_count, const ecs_dt dt, void *udata) {
  clear_lights();
  for (int id = 0; id < entity_count; id++) {
    comp_light *info = ecs_get(ECS, entities[id], LIGHT_COMPONENT_SIGNATURE);
    const phy_vect pos = phy_v_add(phy_body_get_position(info->body), info->offset);
    const phy_vect screen_pos_perc = world_to_screen_perc(pos.x, pos.y);
    info->light.pos_perc.x = screen_pos_perc.x;
    info->light.pos_perc.y = screen_pos_perc.y;
    add_light(info->light.pos_perc, info->light.radius);
  }
  return 0;
}

static void comp_light_constructor(ecs_ecs *ecs, const ecs_id entity_id, void *ptr, void *args) {
  comp_light *info = ptr;
  const comp_light_constructor_info *constructor_info = LIGHT_CONSTRUCTOR_INFO;
  if (info && constructor_info) {
    info->body = constructor_info->body;
    info->light = constructor_info->light;
    info->offset = constructor_info->offset;
  }
}

void comp_light_register() {
  LIGHT_COMPONENT_SIGNATURE = ecs_register_component(ECS, sizeof(comp_light), comp_light_constructor, NULL);
}

void sys_light_register() {
  LIGHT_SYSTEM_SIGNATURE = ecs_register_system(ECS, sys_light_update, NULL, NULL, NULL);
  ecs_require_component(ECS, LIGHT_SYSTEM_SIGNATURE, LIGHT_COMPONENT_SIGNATURE);
  ecs_require_component(ECS, LIGHT_SYSTEM_SIGNATURE, PHYSICS_COMPONENT_SIGNATURE);
}

comp_light *sys_light_add(const ecs_id eid, comp_light_constructor_info *clci) {
  LIGHT_CONSTRUCTOR_INFO = clci;
  return ecs_add(ECS, eid, LIGHT_COMPONENT_SIGNATURE, NULL);
}
