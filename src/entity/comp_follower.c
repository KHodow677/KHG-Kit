#include "comp_follower.h"
#include "khg_phy/phy.h"
#include "khg_phy/phy_types.h"

ecs_id FOLLOWER_COMPONENT_SIGNATURE;

void comp_follower_setup(comp_follower *cf, comp_physics *cp, comp_physics *follow_cp) {
  cf->info.cp = cp;
  cf->info.follow_cp = follow_cp;
}

void comp_follower_register(comp_follower *cf, ecs_ecs *ecs) {
  cf->id = ecs_register_component(ecs, sizeof(comp_follower), NULL, NULL);
  FOLLOWER_COMPONENT_SIGNATURE = cf->id; 
} 

void sys_follower_register(sys_follower *sr, comp_follower *cf, comp_physics *cp, ecs_ecs *ecs, follower_info *info) {
  sr->id = ecs_register_system(ecs, sys_follower_update, NULL, NULL, info);
  ecs_require_component(ecs, sr->id, cf->id);
  ecs_require_component(ecs, sr->id, cp->id);
  sr->ecs = *ecs;
}

ecs_ret sys_follower_update(ecs_ecs *ecs, ecs_id *entities, int entity_count, ecs_dt dt, void *udata) {
  (void)ecs;
  (void)dt;
  (void)udata;
  follower_info *info = udata;
  for (int id = 0; id < entity_count; id++) {
    comp_follower *cf = ecs_get(ecs, entities[id], FOLLOWER_COMPONENT_SIGNATURE);
    comp_physics *cp = ecs_get(ecs, entities[id], PHYSICS_COMPONENT_SIGNATURE);
    cpVect follow_pos = cpBodyGetPosition(info->follow_cp->info.body);
    cpFloat follow_angle = cpBodyGetAngle(info->follow_cp->info.body);
    cpVect new_pos = { follow_pos.x + cf->pos_offset.x, follow_pos.y + cf->pos_offset.y };
    cpFloat new_angle = follow_angle + cf->angle_offset;
    cpBodySetPosition(info->cp->info.body, new_pos);
    cpBodySetAngle(info->cp->info.body, new_angle);
  }
  return 0;
}

