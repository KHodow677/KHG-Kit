#pragma once

#include "entity/comp_animator.h"
#include "entity/comp_destroyer.h"
#include "entity/comp_follower.h"
#include "entity/comp_physics.h"
#include "entity/comp_renderer.h"
#include "khg_ecs/ecs.h"
#include "khg_utl/map.h"
#include "khg_utl/vector.h"

extern ecs_ecs *ECS;
extern utl_map *ENTITY_LOOKUP;
extern utl_vector *TEXTURE_LOOKUP;

extern sys_physics PHYSICS_SYSTEM;
extern sys_renderer RENDERER_SYSTEM;
extern sys_follower FOLLOWER_SYSTEM;
extern sys_destroyer DESTROYER_SYSTEM;
extern sys_animator ANIMATOR_SYSTEM;

extern comp_physics PHYSICS_COMPONENT_TYPE;
extern comp_renderer RENDERER_COMPONENT_TYPE;
extern comp_follower FOLLOWER_COMPONENT_TYPE;
extern comp_destroyer DESTROYER_COMPONENT_TYPE;
extern comp_animator ANIMATOR_COMPONENT_TYPE;

void ecs_setup();
void ecs_cleanup();

