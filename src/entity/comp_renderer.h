#pragma once

#include "comp_physics.h"
#include "khg_ecs/ecs.h"
#include "khg_gfx/texture.h"
#include "khg_phy/phy_types.h"
#include "khg_utl/map.h"

typedef struct {
  gfx_texture *texture;
  cpBody *body;
} renderer_info;

typedef struct {
  ecs_id id;
} comp_renderer;

typedef struct {
  ecs_id id;
  ecs_ecs ecs;
} sys_renderer;

extern ecs_id RENDERER_COMPONENT_SIGNATURE;
extern map *PHYSICS_INFO_MAP;

void info_renderer_setup(renderer_info *info, physics_info *p_info, char *file_name, char *file_type);
void info_renderer_free(renderer_info *info);

void comp_renderer_register(comp_renderer *cr, ecs_ecs *ecs);

void sys_renderer_register(sys_renderer *sr, ecs_ecs *ecs);
void sys_renderer_add(ecs_ecs *ecs, ecs_id *eid, renderer_info *info);
void sys_renderer_free(bool need_free);
ecs_ret sys_renderer_update(ecs_ecs *ecs, ecs_id *entities, int entity_count, ecs_dt dt, void *udata);

