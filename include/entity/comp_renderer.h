#pragma once

#include "khg_ecs/ecs.h"
#include "khg_phy/phy_types.h"
#include "khg_utl/vector.h"

typedef struct {
  int tex_id;
  phy_body *body;
  int render_layer;
} renderer_info;

typedef struct {
  ecs_id id;
} comp_renderer;

typedef struct {
  ecs_id id;
  ecs_ecs ecs;
} sys_renderer;

extern ecs_id RENDERER_COMPONENT_SIGNATURE;
extern renderer_info NO_RENDERER;
extern utl_vector *RENDERER_INFO;

void comp_renderer_register(comp_renderer *cr);

void sys_renderer_register(sys_renderer *sr);
void sys_renderer_add(ecs_id *eid, renderer_info *info);

