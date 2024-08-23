#pragma once

#include "entity/comp_animator.h"
#include "entity/comp_destroyer.h"
#include "entity/comp_follower.h"
#include "entity/comp_mover.h"
#include "entity/comp_physics.h"
#include "entity/comp_renderer.h"
#include "entity/comp_rotator.h"
#include "entity/comp_shooter.h"
#include "khg_ecs/ecs.h"
#include "khg_gfx/texture.h"
#include "khg_utl/vector.h"
#include "threading/thread_manager.h"

extern phy_space *SPACE;
extern ecs_ecs *ECS;
extern utl_vector *ENTITY_LOOKUP;
extern utl_vector *TEXTURE_LOOKUP;
extern thread_data *WORKER_THREADS;

extern gfx_texture NO_TEXTURE;
extern int MAX_TEXTURES;
extern int CURRENT_TEXTURE_ID;
extern int THREAD_COUNT;

extern sys_physics PHYSICS_SYSTEM;
extern sys_renderer RENDERER_SYSTEM;
extern sys_follower FOLLOWER_SYSTEM;
extern sys_destroyer DESTROYER_SYSTEM;
extern sys_animator ANIMATOR_SYSTEM;
extern sys_mover MOVER_SYSTEM;
extern sys_rotator ROTATOR_SYSTEM;
extern sys_shooter SHOOTER_SYSTEM;

extern comp_physics PHYSICS_COMPONENT_TYPE;
extern comp_renderer RENDERER_COMPONENT_TYPE;
extern comp_follower FOLLOWER_COMPONENT_TYPE;
extern comp_destroyer DESTROYER_COMPONENT_TYPE;
extern comp_animator ANIMATOR_COMPONENT_TYPE;
extern comp_mover MOVER_COMPONENET_TYPE;
extern comp_rotator ROTATOR_COMPONENT_TYPE;
extern comp_shooter SHOOTER_COMPONENT_TYPE;

void ecs_setup();
void ecs_cleanup();

