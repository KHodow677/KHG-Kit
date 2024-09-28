#include "entity/comp_shooter.h"
#include "controllers/elements/element_controller.h"
#include "data_utl/kinematic_utl.h"
#include "entity/comp_physics.h"
#include "entity/comp_rotator.h"
#include "game_manager.h"
#include "spawners/spawn_particles.h"
#include "khg_ecs/ecs.h"
#include "khg_phy/body.h"
#include "khg_phy/phy_types.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

ecs_id SHOOTER_COMPONENT_SIGNATURE;

static ecs_ret sys_shooter_update(ecs_ecs *ecs, ecs_id *entities, int entity_count, ecs_dt dt, void *udata) {
  for (int id = 0; id < entity_count; id++) {
    comp_shooter *info = ecs_get(ECS, entities[id], SHOOTER_COMPONENT_SIGNATURE);
    comp_physics *p_info = ecs_get(ECS, entities[id], PHYSICS_COMPONENT_SIGNATURE);
    comp_rotator *r_info = ecs_get(ECS, entities[id], ROTATOR_COMPONENT_SIGNATURE);
    info->shot = false;
    if (!r_info->target_health) {
      info->shoot_timer = info->shoot_cooldown;
    }
    if (r_info->target_health && element_is_targeting_position(p_info, phy_body_get_position(r_info->target_health->body), 0.2f) && info->shoot_timer <= 0) {
      info->shoot_timer = info->shoot_cooldown;
      phy_vect pos = phy_body_get_position(p_info->body);
      float ang = phy_body_get_angle(p_info->body);
      float spawn_x = pos.x + info->barrel_length * sinf(normalize_angle(ang));
      float spawn_y = pos.y + info->barrel_length * -cosf(normalize_angle(ang));
      spawn_particle(p_info, spawn_x, spawn_y);
      info->shot = true;
    }
    info->shoot_timer -= dt;
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
}

comp_shooter *sys_shooter_add(ecs_id eid) {
  return ecs_add(ECS, eid, SHOOTER_COMPONENT_SIGNATURE, NULL);
}

