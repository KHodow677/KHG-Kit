#pragma once

#include "khg_ecs/ecs.h"
#include "khg_phy/phy_types.h"

typedef struct comp_renderer {
  ecs_id id;
  int tex_id;
  phy_body *body;
  int render_layer;
  float parallax_value;
} comp_renderer;

typedef struct comp_renderer_constructor_info {
  phy_body *body; 
  int tex_id;
  int render_layer;
  float parallax_value;
} comp_renderer_constructor_info;

extern ecs_id RENDERER_COMPONENT_SIGNATURE;
extern ecs_id RENDERER_SYSTEM_SIGNATURE;

extern comp_renderer_constructor_info *RENDERER_CONSTRUCTOR_INFO;

const bool current_tex_in_range(const comp_renderer *cr, const int min, const int max);

void comp_renderer_register(void);
void sys_renderer_register(void);

comp_renderer *sys_renderer_add(const ecs_id eid, comp_renderer_constructor_info *crci);

