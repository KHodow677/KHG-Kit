#pragma once

#include "khg_ecs/ecs.h"
#include "khg_phy/phy_types.h"
#include "khg_utl/map.h"

typedef struct {
  cpBody *body;
  cpShape *shape;
  cpSpace *space;
  cpFloat target_vel;
  cpFloat target_ang_vel;
  cpBool is_moving;
  cpBool is_turning;
} physics_info;

typedef struct {
  ecs_id id;
} comp_physics;

typedef struct {
  ecs_id id;
  ecs_ecs ecs;
} sys_physics;

extern ecs_id PHYSICS_COMPONENT_SIGNATURE;
extern map *PHYSICS_INFO_MAP;

void comp_physics_register(comp_physics *cp, ecs_ecs *ecs);

void sys_physics_register(sys_physics *sp, ecs_ecs *ecs);
void sys_physics_add(ecs_ecs *ecs, ecs_id *eid, physics_info *info);
void sys_physics_free(bool need_free);
ecs_ret sys_physics_update(ecs_ecs *ecs, ecs_id *entities, int entity_count, ecs_dt dt, void *udata);

