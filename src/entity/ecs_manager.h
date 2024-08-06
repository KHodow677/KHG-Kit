#pragma once

#include "comp_physics.h"
#include "comp_renderer.h"
#include "khg_ecs/ecs.h"
#include "khg_phy/phy_types.h"

extern ecs_ecs *ECS;

extern sys_physics PHYSICS_SYSTEM;
extern sys_renderer RENDERER_SYSTEM;

extern physics_info PHYSICS_INFO;
extern renderer_info RENDERER_INFO;

void ecs_setup(cpSpace *sp, comp_physics *cp, comp_renderer *cr);
void ecs_cleanup(cpSpace *sp, comp_physics *cp, comp_renderer *cr);

