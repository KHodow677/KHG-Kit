#pragma once

#include "entity/comp_animator.h"
#include "entity/comp_destroyer.h"
#include "entity/comp_follower.h"
#include "entity/comp_physics.h"
#include "entity/comp_renderer.h"
#include "generators/elements/particle_generator.h"
#include "generators/elements/tank_body_generator.h"
#include "generators/elements/tank_top_generator.h"
#include "khg_ecs/ecs.h"
#include "khg_phy/phy_types.h"

extern ecs_ecs *ECS;

extern sys_physics PHYSICS_SYSTEM;
extern sys_renderer RENDERER_SYSTEM;
extern sys_follower FOLLOWER_SYSTEM;
extern sys_destroyer DESTROYER_SYSTEM;
extern sys_animator ANIMATOR_SYSTEM;

extern ecs_id E1;
extern ecs_id E2;

void ecs_setup(cpSpace *sp, comp_physics *cp, comp_renderer *cr, comp_follower *cf, comp_destroyer *cd, comp_animator *ca, tank_body *tb, tank_top *tt);
void ecs_cleanup(cpSpace *sp, tank_body *tb, tank_top *tt, particle *p);

