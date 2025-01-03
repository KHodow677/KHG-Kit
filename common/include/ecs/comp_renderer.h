#pragma once

#include "area/tile.h"
#include "khg_ecs/ecs.h"
#include "khg_phy/body.h"
#include "rig/rig.h"
#include <stddef.h>

typedef struct comp_renderer {
  ecs_id id;
  int tex_id;
  phy_rigid_body *body;
  phy_shape *shape;
  rig rig;
  area_tiles tiles;
  int render_layer;
  float parallax_value;
  phy_vector2 offset;
  bool flipped;
  bool show_shape;
} comp_renderer;

typedef struct comp_renderer_constructor_info {
  phy_rigid_body *body; 
  phy_shape *shape; 
  const int tex_id;
  const size_t rig_id;
  const size_t area_id;
  const int render_layer;
  const float parallax_value;
  const bool flipped;
  const bool show_shape;
} comp_renderer_constructor_info;

extern ecs_id RENDERER_COMPONENT_SIGNATURE;
extern ecs_id RENDERER_SYSTEM_SIGNATURE;

extern comp_renderer_constructor_info *RENDERER_CONSTRUCTOR_INFO;

void comp_renderer_register(void);
void sys_renderer_register(void);

comp_renderer *sys_renderer_add(const ecs_id eid, comp_renderer_constructor_info *crci);

