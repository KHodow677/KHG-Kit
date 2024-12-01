#pragma once

#include "khg_ecs/ecs.h"
#include "khg_phy/body.h"
#include "rig/rig.h"

typedef struct comp_renderer {
  ecs_id id;
  int tex_id;
  const phy_rigid_body *body;
  rig rig;
  int render_layer;
  float parallax_value;
  phy_vector2 offset;
  bool flipped;
} comp_renderer;

typedef struct comp_renderer_constructor_info {
  const phy_rigid_body *body; 
  int tex_id;
  int render_layer;
  float parallax_value;
  bool flipped;
} comp_renderer_constructor_info;

extern ecs_id RENDERER_COMPONENT_SIGNATURE;
extern ecs_id RENDERER_SYSTEM_SIGNATURE;

extern comp_renderer_constructor_info *RENDERER_CONSTRUCTOR_INFO;

void comp_renderer_register(void);
void sys_renderer_register(void);

comp_renderer *sys_renderer_add(const ecs_id eid, comp_renderer_constructor_info *crci, const rig_builder rb);

