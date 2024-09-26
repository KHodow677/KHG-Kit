#include "entity/comp_shooter.h"
#include "controllers/elements/element_controller.h"
#include "controllers/input/key_controllers.h"
#include "data_utl/kinematic_utl.h"
#include "entity/comp_physics.h"
#include "entity/comp_rotator.h"
#include "game_manager.h"
#include "spawners/spawn_particles.h"
#include "khg_ecs/ecs.h"
#include "khg_phy/body.h"
#include "khg_phy/phy_types.h"
#include "khg_utl/vector.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

ecs_id SHOOTER_COMPONENT_SIGNATURE;
shooter_info NO_SHOOTER = { 0 };
utl_vector *SHOOTER_INFO = NULL;

static ecs_ret sys_shooter_update(ecs_ecs *ecs, ecs_id *entities, int entity_count, ecs_dt dt, void *udata) {
  for (int id = 0; id < entity_count; id++) {
    shooter_info *info = utl_vector_at(SHOOTER_INFO, entities[id]);
    physics_info *p_info = &PHYSICS_INFO[entities[id]];
    rotator_info *r_info = utl_vector_at(ROTATOR_INFO, entities[id]);
    if (KEYBOARD_STATE.space_key_went_down && r_info->target_aim_body && element_is_targeting_position(p_info, phy_body_get_position(r_info->target_aim_body), 0.2f) && info->shoot_cooldown == 0) {
      info->shoot_cooldown = 0.16f;
      phy_vect pos = phy_body_get_position(p_info->body);
      float ang = phy_body_get_angle(p_info->body);
      float spawn_x = pos.x + info->barrel_length * sinf(normalize_angle(ang));
      float spawn_y = pos.y + info->barrel_length * -cosf(normalize_angle(ang));
      spawn_x += 5.0f * cosf(ang);
      spawn_y += 5.0f * sinf(ang);
      spawn_particle(p_info->target_body, p_info->body, spawn_x, spawn_y);
    }
    else {
      info->shoot_cooldown = fmaxf(info->shoot_cooldown - dt, 0.0f);
    }
  }
  return 0;
}

void comp_shooter_register(comp_shooter *cs) {
  cs->id = ecs_register_component(ECS, sizeof(comp_shooter), NULL, NULL);
  SHOOTER_COMPONENT_SIGNATURE = cs->id; 
}

void sys_shooter_register(sys_shooter *ss) {
  ss->id = ecs_register_system(ECS, sys_shooter_update, NULL, NULL, NULL);
  ecs_require_component(ECS, ss->id, SHOOTER_COMPONENT_SIGNATURE);
  ecs_require_component(ECS, ss->id, ROTATOR_COMPONENT_SIGNATURE);
  ecs_require_component(ECS, ss->id, PHYSICS_COMPONENT_SIGNATURE);
  ss->ecs = *ECS;
  SHOOTER_INFO = utl_vector_create(sizeof(shooter_info));
  for (int i = 0; i < ECS->entity_count; i++) {
    utl_vector_push_back(SHOOTER_INFO, &NO_SHOOTER);
  }
}

void sys_shooter_add(ecs_id *eid, shooter_info *info) {
  ecs_add(ECS, *eid, SHOOTER_COMPONENT_SIGNATURE, NULL);
  utl_vector_assign(SHOOTER_INFO, *eid, info);
}

