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

void generate_physics_box(ecs_id eid, comp_physics *info, float width, float height, float mass, phy_vect pos, float ang, phy_vect cog) {
  float moment = phy_moment_for_box(mass, width, height);
  info->body = phy_space_add_body(SPACE, phy_body_new(mass, moment));
  phy_body_set_position(info->body, pos);
  phy_body_set_center_of_gravity(info->body, cog);
  phy_body_set_angle(info->body, ang);
  info->has_constraint = false;
  info->is_moving = false;
  info->is_turning = false;
  info->target_vel = 0.0f;
  info->target_ang_vel = 0.0f;
  info->move_enabled = true;
  info->rotate_enabled = true;
}

void free_physics(comp_physics *info) {
  if (info->has_constraint) {
    phy_space_remove_constraint(SPACE, info->pivot);
  }
  phy_space_remove_body(SPACE, info->body);
  phy_body_free(info->body);
}
