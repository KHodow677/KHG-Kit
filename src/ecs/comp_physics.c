#include "ecs/comp_physics.h"
#include "ecs/ecs_manager.h"
#include "physics/physics.h"
#include "utility/math_utl.h"
#include "khg_phy/body.h"
#include "khg_phy/phy.h"
#include "khg_phy/space.h"
#include "khg_phy/vect.h"

ecs_id PHYSICS_COMPONENT_SIGNATURE;
ecs_id PHYSICS_SYSTEM_SIGNATURE;

comp_physics_constructor_info *PHYSICS_CONSTRUCTOR_INFO = NULL;

static ecs_ret sys_physics_update(ecs_ecs *ecs, ecs_id *entities, const int entity_count, const ecs_dt dt, void *udata) {
  if (dt == 0.0f) {
    return 0;
  }
  for (int id = 0; id < entity_count; id++) {
    comp_physics *info = ecs_get(ECS, entities[id], PHYSICS_COMPONENT_SIGNATURE);
    if (info->move_enabled) {
      phy_body_set_velocity(info->body, phy_v(info->target_vel, 0.0f));
    }
  }
  return 0;
}

static void comp_physics_constructor(ecs_ecs *ecs, const ecs_id entity_id, void *ptr, void *args) {
  comp_physics *info = ptr;
  const comp_physics_constructor_info *constructor_info = PHYSICS_CONSTRUCTOR_INFO;
  if (info && constructor_info && constructor_info->mode == PHYSICS_BOX) {
    const float moment = phy_moment_for_box(constructor_info->mass, constructor_info->width, constructor_info->height);
    info->body = phy_space_add_body(SPACE, phy_body_new(constructor_info->mass, moment));
    phy_body_set_position(info->body, constructor_info->pos);
    phy_body_set_center_of_gravity(info->body, constructor_info->cog);
    phy_body_set_angle(info->body, constructor_info->ang);
    info->has_constraint = false;
    info->is_moving = false;
    info->target_vel = 0.0f;
    info->move_enabled = true;
  }
}

static void comp_physics_destructor(ecs_ecs *ecs, const ecs_id entity_id, void *ptr) {
  const comp_physics *info = ptr;
  if (info) {
    if (info->has_constraint) {
      phy_space_remove_constraint(SPACE, info->pivot);
    }
    phy_space_remove_body(SPACE, info->body);
    phy_body_free(info->body);
  }
}

void comp_physics_register() {
  PHYSICS_COMPONENT_SIGNATURE = ecs_register_component(ECS, sizeof(comp_physics), comp_physics_constructor, comp_physics_destructor);
}

void sys_physics_register() {
  PHYSICS_SYSTEM_SIGNATURE = ecs_register_system(ECS, sys_physics_update, NULL, NULL, NULL);
  ecs_require_component(ECS, PHYSICS_SYSTEM_SIGNATURE, PHYSICS_COMPONENT_SIGNATURE);
}

comp_physics *sys_physics_add(const ecs_id eid, comp_physics_constructor_info *cpci) {
  PHYSICS_CONSTRUCTOR_INFO = cpci;
  return ecs_add(ECS, eid, PHYSICS_COMPONENT_SIGNATURE, NULL);
}

