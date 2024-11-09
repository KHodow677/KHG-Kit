#pragma once

#include "graphics/light.h"
#include "khg_ecs/ecs.h"
#include "khg_phy/body.h"
#include "khg_phy/vector.h"

typedef struct comp_light {
  ecs_id id;
  light light;
  nvRigidBody *body;
  nvVector2 offset;
} comp_light;

typedef struct comp_light_constructor_info {
  nvRigidBody *body; 
  light light;
  nvVector2 offset;
} comp_light_constructor_info;

extern ecs_id LIGHT_COMPONENT_SIGNATURE;
extern ecs_id LIGHT_SYSTEM_SIGNATURE;

extern comp_light_constructor_info *LIGHT_CONSTRUCTOR_INFO;

void comp_light_register(void);
void sys_light_register(void);

comp_light *sys_light_add(const ecs_id eid, comp_light_constructor_info *clci);

