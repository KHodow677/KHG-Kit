#pragma once

#include "comp_follower.h"
#include "comp_physics.h"
#include "comp_renderer.h"
#include "khg_ecs/ecs.h"
#include "khg_phy/phy_types.h"

extern ecs_ecs *ECS;

extern sys_physics PHYSICS_SYSTEM;
extern sys_renderer RENDERER_SYSTEM;

extern ecs_id E1;
extern ecs_id E2;

void ecs_setup(cpSpace *sp, comp_physics *cp1, comp_renderer *cr1, comp_physics *cp2, comp_renderer *cr2, comp_follower *cf);
void ecs_cleanup(cpSpace *sp, comp_physics *cp1, comp_renderer *cr1, comp_physics *cp2, comp_renderer *cr2);

