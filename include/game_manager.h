#pragma once

#include "controllers/input/key_controllers.h"
#include "controllers/input/mouse_controller.h"
#include "entity/camera.h"
#include "entity/comp_animator.h"
#include "entity/comp_commander.h"
#include "entity/comp_damage.h"
#include "entity/comp_destroyer.h"
#include "entity/comp_health.h"
#include "entity/comp_mover.h"
#include "entity/comp_physics.h"
#include "entity/comp_renderer.h"
#include "entity/comp_rotator.h"
#include "entity/comp_selector.h"
#include "entity/comp_shooter.h"
#include "entity/comp_spawn.h"
#include "entity/comp_stream_spawner.h"
#include "entity/comp_targeter.h"
#include "khg_ecs/ecs.h"
#include "khg_gfx/texture.h"
#include "khg_stm/state_machine.h"
#include "khg_utl/vector.h"
#include "khg_thd/thread.h"
#include "menus/game_menu_manager.h"

#define ECS_ENTITY_COUNT 1024
#define COLLISION_CATEGORY_ENTITY (1 << 0)
#define COLLISION_CATEGORY_OBJECT (1 << 1)
#define SENSOR_COLLISION_TYPE 1
#define NORMAL_COLLISION_TYPE 2

extern int WINDOW_START_WIDTH;
extern int WINDOW_START_HEIGHT;

extern phy_space *SPACE;
extern ecs_ecs *ECS;
extern utl_vector *ENTITY_LOOKUP;
extern utl_vector *TEXTURE_LOOKUP;
extern thd_thread *WORKER_THREADS;
extern utl_vector *GAME_FLOOR_MAP;
extern utl_vector *GAME_BUILDING_MAP;
extern utl_vector *GAME_PATH_MAP;
extern utl_vector *GAME_MAP_SEGMENTS;
extern int GAME_MAP_WIDTH;
extern int GAME_MAP_HEIGHT;
extern int GAME_MAP_TILE_SIZE;

extern game_overlay_info GAME_OVERLAY_TRACKER[NUM_MENUS];

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
extern stm_state TUTORIAL_SCENE;
extern stm_state ERROR_SCENE;

extern sys_physics PHYSICS_SYSTEM;
extern sys_renderer RENDERER_SYSTEM;
extern sys_destroyer DESTROYER_SYSTEM;
extern sys_animator ANIMATOR_SYSTEM;
extern sys_mover MOVER_SYSTEM;
extern sys_rotator ROTATOR_SYSTEM;
extern sys_shooter SHOOTER_SYSTEM;
extern sys_selector SELECTOR_SYSTEM;
extern sys_spawn SPAWNER_SYSTEM;
extern sys_stream_spawner STREAM_SPAWNER_SYSTEM;
extern sys_commander COMMANDER_SYSTEM;
extern sys_targeter TARGETER_SYSTEM;
extern sys_health HEALTH_SYSTEM;
extern sys_damage DAMAGE_SYSTEM;

void ecs_setup();
void ecs_cleanup();

