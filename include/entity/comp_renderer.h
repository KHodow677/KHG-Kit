#pragma once

#include "khg_ecs/ecs.h"
#include "khg_phy/phy_types.h"
#include "khg_utl/vector.h"

#define RENDERER_SEGMENTS 4

typedef struct comp_renderer comp_renderer;
struct comp_renderer {
  ecs_id id;
  int tex_id;
  phy_body *body;
  utl_vector *indicators;
  phy_shape *segments[4];
  int render_layer;
  comp_renderer *linked_r_info;
};

typedef struct {
  ecs_id id;
  ecs_ecs ecs;
} sys_renderer;

extern ecs_id RENDERER_COMPONENT_SIGNATURE;

void comp_renderer_register(comp_renderer *cr);

void sys_renderer_register(sys_renderer *sr);
comp_renderer *sys_renderer_add(ecs_id eid);

