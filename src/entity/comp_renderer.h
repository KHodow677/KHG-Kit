#pragma once

#include "comp_physics.h"
#include "khg_ecs/ecs.h"
#include "khg_gfx/texture.h"
#include "khg_phy/phy_types.h"

typedef struct {
  gfx_texture *texture;
  cpBody *body;
} renderer_info;

typedef struct {
  ecs_id id;
  renderer_info info;
} comp_renderer;

typedef struct {
  ecs_id id;
  ecs_ecs ecs;
} sys_renderer;

extern ecs_id RENDERER_COMPONENT_SIGNATURE;

void comp_renderer_setup(comp_renderer *cr, comp_physics *cp);
void comp_renderer_free(comp_renderer *cr);
void comp_renderer_register(comp_renderer *cr, ecs_ecs *ecs);
void sys_renderer_register(sys_renderer *sr, comp_renderer *cr, comp_physics *cp, ecs_ecs *ecs, renderer_info *info);
ecs_ret sys_renderer_update(ecs_ecs *ecs, ecs_id *entities, int entity_count, ecs_dt dt, void *udata);

