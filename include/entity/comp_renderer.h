#pragma once

#include "khg_ecs/ecs.h"
#include "khg_phy/phy_types.h"
#include "khg_utl/map.h"

typedef struct {
  int tex_id;
  phy_body *body;
} renderer_info;

typedef struct {
  ecs_id id;
} comp_renderer;

typedef struct {
  ecs_id id;
  ecs_ecs ecs;
} sys_renderer;

extern ecs_id RENDERER_COMPONENT_SIGNATURE;
extern utl_map *RENDERER_INFO_MAP;

void comp_renderer_register(comp_renderer *cr);

void sys_renderer_register(sys_renderer *sr);
void sys_renderer_add(ecs_id *eid, renderer_info *info);
void sys_renderer_free(bool need_free);

ecs_ret sys_renderer_update(ecs_ecs *ecs, ecs_id *entities, int entity_count, ecs_dt dt, void *udata);
