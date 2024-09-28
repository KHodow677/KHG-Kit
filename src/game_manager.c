#include "game_manager.h"
#include "controllers/input/key_controllers.h"
#include "entity/camera.h"
#include "entity/comp_commander.h"
#include "entity/comp_damage.h"
#include "entity/comp_health.h"
#include "entity/comp_mover.h"
#include "entity/comp_animator.h"
#include "entity/comp_destroyer.h"
#include "entity/comp_physics.h"
#include "entity/comp_renderer.h"
#include "entity/comp_rotator.h"
#include "entity/comp_selector.h"
#include "entity/comp_shooter.h"
#include "entity/comp_spawn.h"
#include "entity/comp_stream_spawner.h"
#include "entity/comp_targeter.h"
#include "entity/entity.h"
#include "entity/map.h"
#include "generators/components/map_generator.h"
#include "generators/components/texture_generator.h"
#include "khg_stm/state_machine.h"
#include "menus/game_menu_manager.h"
#include "physics/physics_setup.h"
#include "scenes/scene_utl.h"
#include "scenes/scene_transition.h"
#include "threading/thread_manager.h"
#include "khg_ecs/ecs.h"
#include "khg_utl/vector.h"
#include <stdlib.h>
#include <stdio.h>

int WINDOW_START_WIDTH  = 1280;
int WINDOW_START_HEIGHT = 720;

phy_space *SPACE;
ecs_ecs *ECS;
utl_vector *ENTITY_LOOKUP;
utl_vector *TEXTURE_LOOKUP;
thd_thread *WORKER_THREADS;
utl_vector *GAME_FLOOR_MAP;
utl_vector *GAME_BUILDING_MAP;
utl_vector *GAME_PATH_MAP;
utl_vector *GAME_MAP_SEGMENTS;
int GAME_MAP_WIDTH;
int GAME_MAP_HEIGHT;
int GAME_MAP_TILE_SIZE = 128;

game_overlay_info GAME_OVERLAY_TRACKER[NUM_MENUS];

gfx_texture NO_TEXTURE = { 0 };
int CURRENT_TEXTURE_ID = 0;
int THREAD_COUNT;
camera CAMERA = { 0 };
mouse_state MOUSE_STATE = { 0 };
keyboard_state KEYBOARD_STATE = { 0 };

gfx_font LARGE_FONT;
gfx_font MEDIUM_FONT;

stm_state_machine SCENE_FSM;

stm_state PARENT_SCENE = {
  .parent_state = NULL,
  .entry_state = &TITLE_SCENE,
  .transitions = (stm_transition[]){ { EVENT_SCENE_SWITCH, (void *)(intptr_t)'!', &compare_scene_switch_command, NULL, &TITLE_SCENE } },
  .num_transitions = 2,
  .data = "GROUP",
};

stm_state TITLE_SCENE = {
  .parent_state = &PARENT_SCENE,
  .entry_state = NULL,
  .transitions = (stm_transition[]){ { EVENT_SCENE_SWITCH, (void *)(intptr_t)TO_TUTORIAL_SCENE, &compare_scene_switch_command, &load_tutorial_scene, &TUTORIAL_SCENE } },
  .num_transitions = 1,
  .data = "TITLE",
};

stm_state TUTORIAL_SCENE = {
  .parent_state = &PARENT_SCENE,
  .entry_state = NULL,
  .transitions = (stm_transition[]){ { EVENT_SCENE_SWITCH, (void *)(intptr_t)TO_TITLE_SCENE, &compare_scene_switch_command, NULL, &TITLE_SCENE } },
  .num_transitions = 1,
  .data = "TUTORIAL",
};

stm_state ERROR_SCENE = {
  .data = "ERROR",
  .entry_action = &print_scene_error
};

sys_physics PHYSICS_SYSTEM = { 0 };
sys_renderer RENDERER_SYSTEM = { 0 };
sys_destroyer DESTROYER_SYSTEM = { 0 };
sys_animator ANIMATOR_SYSTEM = { 0 };
sys_mover MOVER_SYSTEM = { 0 };
sys_rotator ROTATOR_SYSTEM = { 0 };
sys_shooter SHOOTER_SYSTEM = { 0 };
sys_selector SELECTOR_SYSTEM = { 0 };
sys_spawn SPAWNER_SYSTEM = { 0 };
sys_stream_spawner STREAM_SPAWNER_SYSTEM = { 0 };
sys_commander COMMANDER_SYSTEM = { 0 };
sys_targeter TARGETER_SYSTEM = { 0 };
sys_health HEALTH_SYSTEM = { 0 };
sys_damage DAMAGE_SYSTEM = { 0 };

void ecs_setup() {
  camera_setup(&CAMERA);
  ECS = ecs_new(ECS_ENTITY_COUNT, NULL);
  comp_physics_register();
  comp_renderer_register();
  comp_destroyer_register();
  comp_animator_register();
  comp_mover_register();
  comp_rotator_register();
  comp_shooter_register();
  comp_selector_register();
  comp_spawn_register();
  comp_stream_spawner_register();
  comp_commander_register();
  comp_targeter_register();
  comp_health_register();
  comp_damage_register();
  sys_physics_register(&PHYSICS_SYSTEM);
  sys_renderer_register(&RENDERER_SYSTEM);
  sys_destroyer_register(&DESTROYER_SYSTEM);
  sys_animator_register(&ANIMATOR_SYSTEM);
  sys_mover_register(&MOVER_SYSTEM);
  sys_rotator_register(&ROTATOR_SYSTEM);
  sys_shooter_register(&SHOOTER_SYSTEM);
  sys_selector_register(&SELECTOR_SYSTEM);
  sys_spawn_register(&SPAWNER_SYSTEM);
  sys_stream_spawner_register(&STREAM_SPAWNER_SYSTEM);
  sys_commander_register(&COMMANDER_SYSTEM);
  sys_targeter_register(&TARGETER_SYSTEM);
  sys_health_register(&HEALTH_SYSTEM);
  sys_damage_register(&DAMAGE_SYSTEM);
  generate_entity_lookup();
  generate_textures();
}

void ecs_cleanup() {
  free_entity_lookup();
  free_textures();
  free_map_collision_segments(&GAME_MAP_SEGMENTS);
  free_map(&GAME_FLOOR_MAP);
  free_map(&GAME_BUILDING_MAP);
  free_map(&GAME_PATH_MAP);
  physics_free(SPACE);
  ecs_free(ECS);
  free_worker_threads();
}

