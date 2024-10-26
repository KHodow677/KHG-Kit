#pragma once

#include "graphics/light.h"
#include "khg_ecs/ecs.h"
#include "khg_phy/phy_types.h"

typedef struct comp_light {
  ecs_id id;
  light light;
  phy_body *body;
  phy_vect offset;
} comp_light;

typedef struct comp_light_constructor_info {
  phy_body *body; 
  light light;
  phy_vect offset;
} comp_light_constructor_info;

extern ecs_id LIGHT_COMPONENT_SIGNATURE;
extern ecs_id LIGHT_SYSTEM_SIGNATURE;

extern comp_light_constructor_info *LIGHT_CONSTRUCTOR_INFO;

void comp_light_register(void);
void sys_light_register(void);

comp_light *sys_light_add(ecs_id eid, comp_light_constructor_info *clci);

