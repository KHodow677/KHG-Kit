#define NAMESPACE_ELEMENT_IMPL
#define NAMESPACE_KIN_USE

#include "khg_kin/namespace.h"
#include "element/namespace.h"
#include "khg_ecs/ecs.h"
#include "util/camera/camera.h"
#include "util/light.h"

static ecs_ret sys_light_update(ecs_ecs *ecs, ecs_id *entities, const unsigned int entity_count, const ecs_dt dt, void *udata) {
  if (dt == 0.0f) {
    return 0;
  }
  clear_lights();
  for (unsigned int id = 0; id < entity_count; id++) {
    comp_light *info = ecs_get(NAMESPACE_ELEMENT_INTERNAL.ECS, entities[id], NAMESPACE_ELEMENT_INTERNAL.LIGHT_INFO.component_signature);
    comp_physics *p_info = ecs_get(NAMESPACE_ELEMENT_INTERNAL.ECS, entities[id], NAMESPACE_ELEMENT_INTERNAL.PHYSICS_INFO.component_signature);
    const kin_vec pos = NAMESPACE_KIN()->vec_add(p_info->body->pos, info->offset);
    const kin_vec screen_pos_perc = world_to_screen_perc(pos.x, pos.y);
    info->light.pos_perc.x = screen_pos_perc.x;
    info->light.pos_perc.y = screen_pos_perc.y;
    add_light(info->light.pos_perc, info->light.intensity);
  }
  return 0;
}

static void comp_light_constructor(ecs_ecs *ecs, const ecs_id entity_id, void *ptr, void *args) {
  comp_light *info = ptr;
  const comp_light *constructor_info = NAMESPACE_ELEMENT_INTERNAL.LIGHT_INFO.init_info;
  if (info) {
    info->light = constructor_info->light;
    info->offset = constructor_info->offset;
  }
}

void comp_light_register() {
  NAMESPACE_ELEMENT_INTERNAL.LIGHT_INFO.system_signature = ecs_register_component(NAMESPACE_ELEMENT_INTERNAL.ECS, sizeof(comp_light), comp_light_constructor, NULL);
}

void sys_light_register() {
  NAMESPACE_ELEMENT_INTERNAL.LIGHT_INFO.system_signature = ecs_register_system(NAMESPACE_ELEMENT_INTERNAL.ECS, sys_light_update, NULL, NULL, NULL);
  ecs_require_component(NAMESPACE_ELEMENT_INTERNAL.ECS, NAMESPACE_ELEMENT_INTERNAL.LIGHT_INFO.system_signature, NAMESPACE_ELEMENT_INTERNAL.LIGHT_INFO.component_signature);
  ecs_require_component(NAMESPACE_ELEMENT_INTERNAL.ECS, NAMESPACE_ELEMENT_INTERNAL.LIGHT_INFO.system_signature, NAMESPACE_ELEMENT_INTERNAL.PHYSICS_INFO.component_signature);
}

comp_light *sys_light_add(const ecs_id eid, comp_light *cl) {
  NAMESPACE_ELEMENT_INTERNAL.LIGHT_INFO.init_info = cl;
  return ecs_add(NAMESPACE_ELEMENT_INTERNAL.ECS, eid, NAMESPACE_ELEMENT_INTERNAL.LIGHT_INFO.component_signature, NULL);
}

