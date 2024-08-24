#pragma once

#include "khg_ecs/ecs.h"
#include "khg_utl/vector.h"

typedef struct {
  bool destroy_now;
} destroyer_info;

typedef struct {
  ecs_id id;
} comp_destroyer;

typedef struct {
  ecs_id id;
  ecs_ecs ecs;
} sys_destroyer;

extern ecs_id DESTROYER_COMPONENT_SIGNATURE;
extern destroyer_info NO_DESTROYER;
extern utl_vector *DESTROYER_INFO;

void comp_destroyer_register(comp_destroyer *cd);

void sys_destroyer_register(sys_destroyer *sd);
void sys_destroyer_add(ecs_id *eid, destroyer_info *info);
void sys_destroyer_free(bool need_free);

ecs_ret sys_destroyer_update(ecs_ecs *ecs, ecs_id *entities, int entity_count, ecs_dt dt, void *udata);

