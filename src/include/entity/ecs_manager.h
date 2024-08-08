#pragma once

#include "entity/comp_physics.h"
#include "entity/comp_renderer.h"
#include "generators/elements/tank_body_generator.h"
#include "khg_ecs/ecs.h"
#include "khg_phy/phy_types.h"

extern ecs_ecs *ECS;

extern sys_physics PHYSICS_SYSTEM;
extern sys_renderer RENDERER_SYSTEM;

extern ecs_id E1;
extern ecs_id E2;

void ecs_setup(cpSpace *sp, comp_physics *cp1, comp_renderer *cr1, tank_body *tb);
void ecs_cleanup(cpSpace *sp, tank_body *tb);

