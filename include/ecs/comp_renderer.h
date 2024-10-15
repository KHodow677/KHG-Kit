#pragma once

#include "khg_ecs/ecs.h"
#include "khg_phy/phy_types.h"

typedef struct comp_renderer comp_renderer;
struct comp_renderer {
  ecs_id id;
  int tex_id;
  phy_body *body;
  int render_layer;
};

extern ecs_id RENDERER_COMPONENT_SIGNATURE;
extern ecs_id RENDERER_SYSTEM_SIGNATURE;

void comp_renderer_register(void);
void sys_renderer_register(void);

comp_renderer *sys_renderer_add(ecs_id eid);
