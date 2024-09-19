#pragma once

#include "controllers/input/key_controllers.h"
#include "controllers/input/mouse_controller.h"
#include "entity/camera.h"
#include "entity/comp_animator.h"
#include "entity/comp_destroyer.h"
#include "entity/comp_mover.h"
#include "entity/comp_physics.h"
#include "entity/comp_renderer.h"
#include "entity/comp_rotator.h"
#include "entity/comp_selector.h"
#include "entity/comp_shooter.h"
#include "khg_ecs/ecs.h"
#include "khg_gfx/texture.h"
#include "khg_stm/state_machine.h"
#include "khg_utl/vector.h"
#include "khg_thd/thread.h"

extern phy_space *SPACE;
extern ecs_ecs *ECS;
extern utl_vector *ENTITY_LOOKUP;
extern utl_vector *TEXTURE_LOOKUP;
extern thd_thread *WORKER_THREADS;
extern utl_vector *GAME_FLOOR_MAP;
extern utl_vector *GAME_BUILDING_MAP;
extern int GAME_MAP_SIZE;
extern int GAME_MAP_TILE_SIZE;

extern gfx_texture NO_TEXTURE;
extern int MAX_TEXTURES;
extern int CURRENT_TEXTURE_ID;
extern int THREAD_COUNT;
extern camera CAMERA;
extern mouse_state MOUSE_STATE;
extern keyboard_state KEYBOARD_STATE;

extern gfx_font LARGE_FONT;
extern gfx_font MEDIUM_FONT;

extern stm_state_machine SCENE_FSM;
extern stm_state TITLE_SCENE;
extern stm_state SANDBOX_SCENE;
extern stm_state ERROR_SCENE;

extern sys_physics PHYSICS_SYSTEM;
extern sys_renderer RENDERER_SYSTEM;
extern sys_destroyer DESTROYER_SYSTEM;
extern sys_animator ANIMATOR_SYSTEM;
extern sys_mover MOVER_SYSTEM;
extern sys_rotator ROTATOR_SYSTEM;
extern sys_shooter SHOOTER_SYSTEM;
extern sys_selector SELECTOR_SYSTEM;

extern comp_physics PHYSICS_COMPONENT_TYPE;
extern comp_renderer RENDERER_COMPONENT_TYPE;
extern comp_destroyer DESTROYER_COMPONENT_TYPE;
extern comp_animator ANIMATOR_COMPONENT_TYPE;
extern comp_mover MOVER_COMPONENET_TYPE;
extern comp_rotator ROTATOR_COMPONENT_TYPE;
extern comp_shooter SHOOTER_COMPONENT_TYPE;

void ecs_setup();
void ecs_cleanup();

