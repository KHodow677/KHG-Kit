#pragma once

#include "khg_ecs/ecs.h"

typedef struct {
  ecs_id id;
} comp_renderer;

typedef struct {
  ecs_id id;
  ecs_ecs ecs;
} sys_renderer;

void comp_renderer_setup(comp_renderer *cr);
void comp_renderer_free(comp_renderer *cr);
void comp_renderer_register(comp_renderer *cr, ecs_ecs *ecs);
void sys_renderer_register(sys_renderer *sr, comp_renderer *cr, ecs_ecs *ecs);
ecs_ret sys_renderer_update(ecs_ecs *ecs, ecs_id *entities, int entity_count, ecs_dt dt, void *udata);

