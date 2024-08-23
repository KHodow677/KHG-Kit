#pragma once

#include "khg_ecs/ecs.h"
#include "khg_phy/phy_types.h"
#include "khg_utl/vector.h"

typedef struct {
  phy_body *body;
  phy_shape *shape;
  float target_vel;
  float target_ang_vel;
  bool is_moving;
  bool is_turning;
  bool is_locked_on;
} physics_info;

typedef struct {
  ecs_id id;
} comp_physics;

typedef struct {
  ecs_id id;
  ecs_ecs ecs;
} sys_physics;

extern ecs_id PHYSICS_COMPONENT_SIGNATURE;
extern physics_info NO_PHYSICS;
extern utl_vector* PHYSICS_INFO;

void comp_physics_register(comp_physics *cp);

void sys_physics_register(sys_physics *sp);
void sys_physics_add(ecs_id *eid, physics_info *info);
void sys_physics_free(bool need_free);

ecs_ret sys_physics_update(ecs_ecs *ecs, ecs_id *entities, int entity_count, ecs_dt dt, void *udata);
