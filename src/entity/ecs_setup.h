#pragma once

#include "khg_ecs/ecs.h"
#include "khg_phy/phy_types.h"

extern ecs_id PHYSICS_COMPONENT;
extern ecs_id RENDERER_COMPONENT;

int ecs_setup(cpSpace *sp);

