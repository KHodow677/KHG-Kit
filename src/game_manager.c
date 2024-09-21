#include "game_manager.h"
#include "controllers/input/key_controllers.h"
#include "entity/camera.h"
#include "entity/comp_mover.h"
#include "entity/comp_animator.h"
#include "entity/comp_destroyer.h"
#include "entity/comp_physics.h"
#include "entity/comp_renderer.h"
#include "entity/comp_rotator.h"
#include "entity/comp_selector.h"
#include "entity/comp_shooter.h"
#include "entity/entity.h"
#include "entity/map.h"
#include "generators/components/map_generator.h"
#include "generators/components/texture_generator.h"
#include "khg_stm/state_machine.h"
#include "physics/physics_setup.h"
#include "scenes/scene_utl.h"
#include "threading/thread_manager.h"
#include "khg_ecs/ecs.h"
#include "khg_utl/vector.h"
#include <stdlib.h>
#include <stdio.h>

phy_space *SPACE;
ecs_ecs *ECS;
utl_vector *ENTITY_LOOKUP;
utl_vector *TEXTURE_LOOKUP;
thd_thread *WORKER_THREADS;
utl_vector *GAME_FLOOR_MAP;
utl_vector *GAME_BUILDING_MAP;
utl_vector *GAME_MAP_SEGMENTS;
int GAME_MAP_SIZE = 64;
int GAME_MAP_TILE_SIZE = 128;

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
  .transitions = (stm_transition[]){ { EVENT_SCENE_SWITCH, (void *)(intptr_t)TO_SANDBOX_SCENE, &compare_scene_switch_command, NULL, &SANDBOX_SCENE } },
  .num_transitions = 1,
  .data = "TITLE",
};

stm_state SANDBOX_SCENE = {
  .parent_state = &PARENT_SCENE,
  .entry_state = NULL,
  .transitions = (stm_transition[]){ { EVENT_SCENE_SWITCH, (void *)(intptr_t)TO_TITLE_SCENE, &compare_scene_switch_command, NULL, &TITLE_SCENE } },
  .num_transitions = 1,
  .data = "SANDBOX",
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

comp_physics PHYSICS_COMPONENT_TYPE;
comp_renderer RENDERER_COMPONENT_TYPE;
comp_destroyer DESTROYER_COMPONENT_TYPE;
comp_animator ANIMATOR_COMPONENT_TYPE;
comp_mover MOVER_COMPONENT_TYPE;
comp_rotator ROTATOR_COMPONENT_TYPE;
comp_shooter SHOOTER_COMPONENT_TYPE;
comp_selector SELECTOR_COMPONENT_TYPE;

void ecs_setup() {
  camera_setup(&CAMERA);
  ECS = ecs_new(1024, NULL);
  comp_physics_register(&PHYSICS_COMPONENT_TYPE);
  comp_renderer_register(&RENDERER_COMPONENT_TYPE);
  comp_destroyer_register(&DESTROYER_COMPONENT_TYPE);
  comp_animator_register(&ANIMATOR_COMPONENT_TYPE);
  comp_mover_register(&MOVER_COMPONENT_TYPE);
  comp_rotator_register(&ROTATOR_COMPONENT_TYPE);
  comp_shooter_register(&SHOOTER_COMPONENT_TYPE);
  comp_selector_register(&SELECTOR_COMPONENT_TYPE);
  sys_physics_register(&PHYSICS_SYSTEM);
  sys_renderer_register(&RENDERER_SYSTEM);
  sys_destroyer_register(&DESTROYER_SYSTEM);
  sys_animator_register(&ANIMATOR_SYSTEM);
  sys_mover_register(&MOVER_SYSTEM);
  sys_rotator_register(&ROTATOR_SYSTEM);
  sys_shooter_register(&SHOOTER_SYSTEM);
  sys_selector_register(&SELECTOR_SYSTEM);
  generate_entity_lookup();
  generate_textures();
}

void ecs_cleanup() {
  free_entity_lookup();
  free_textures();
  free_map_collision_segments(&GAME_MAP_SEGMENTS);
  free_map(&GAME_FLOOR_MAP);
  free_map(&GAME_BUILDING_MAP);
  physics_free(SPACE);
  ecs_free(ECS);
  free_worker_threads();
}

