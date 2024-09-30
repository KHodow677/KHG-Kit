#include "scenes/scene_transition.h"
#include "controllers/elements/camera_controller.h"
#include "entity/map.h"
#include "game_manager.h"
#include "generators/components/map_generator.h"
#include "khg_gfx/internal.h"
#include "khg_phy/threaded_space.h"
#include "khg_phy/vect.h"
#include "khg_stm/state_machine.h"
#include "menus/game_menu_manager.h"
#include "menus/pause_menu.h"
#include "spawners/spawn_barrier.h"
#include "spawners/spawn_depot.h"
#include "spawners/spawn_hangar.h"
#include "spawners/spawn_spawner.h"
#include "spawners/spawn_turret.h"
#include <math.h>

void load_tutorial_scene(void *old_state_data, stm_event *event, void *new_state_data) {
  free_map(&GAME_FLOOR_MAP);
  free_map(&GAME_BUILDING_MAP);
  free_map(&GAME_PATH_MAP);
  ecs_cleanup();
  PAUSED = false;
  GAME_INFO.cash = 100;
  GAME_INFO.lives = 5;
  GAME_INFO.time = 0.0f;
  load_map("Tutorial-Floor", &GAME_FLOOR_MAP);
  load_map("Tutorial-Building", &GAME_BUILDING_MAP);
  load_map("Tutorial-Path", &GAME_PATH_MAP);
  ecs_setup(true);
  add_map_collision_segments(GAME_BUILDING_MAP, &GAME_MAP_SEGMENTS);
  setup_game_overlay();
  spawn_spawner(2162, 0, -0.5f * M_PI, (phy_vect[]){ phy_v(-1408, 0) }, 1);
  spawn_turret(-1600, -88, M_PI * 0.5f);
  spawn_turret(-1600, 80, M_PI * 0.5f);
  spawn_hangar(0, 1768, 0.0f);
  spawn_depot(-1407, -5, M_PI * 0.5f);
  for (int i = -1408; i < 2048; i += 256) {
    spawn_barrier(i, -128, 0.0f);
    spawn_barrier(i, 118, M_PI);
  }
};

void load_game_scene(void *old_state_data, stm_event *event, void *new_state_data) {
  free_map(&GAME_FLOOR_MAP);
  free_map(&GAME_BUILDING_MAP);
  free_map(&GAME_PATH_MAP);
  ecs_cleanup();
  PAUSED = false;
  GAME_INFO.cash = 100;
  GAME_INFO.lives = 5;
  GAME_INFO.time = 0.0f;
  load_map("Game-Floor", &GAME_FLOOR_MAP);
  load_map("Game-Building", &GAME_BUILDING_MAP);
  load_map("Game-Path", &GAME_PATH_MAP);
  ecs_setup(true);
  add_map_collision_segments(GAME_BUILDING_MAP, &GAME_MAP_SEGMENTS);
  setup_game_overlay();
  spawn_spawner(2162, 0, -0.5f * M_PI, (phy_vect[]){ phy_v(-1408, 0) }, 1);
  spawn_turret(-1600, -88, M_PI * 0.5f);
  spawn_turret(-1600, 80, M_PI * 0.5f);
  spawn_hangar(0, 1768, 0.0f);
  spawn_depot(-1407, -5, M_PI * 0.5f);
  for (int i = -1408; i < 2048; i += 256) {
    spawn_barrier(i, -128, 0.0f);
    spawn_barrier(i, 118, M_PI);
  }
};

void load_title_scene(void *old_state_data, stm_event *event, void *new_state_data) {
  PAUSED = false;
  free_map_collision_segments(&GAME_MAP_SEGMENTS);
  free_map(&GAME_FLOOR_MAP);
  free_map(&GAME_BUILDING_MAP);
  free_map(&GAME_PATH_MAP);
  ecs_cleanup();
  ecs_setup(false);
  load_map("Game-Floor", &GAME_FLOOR_MAP);
  load_map("Game-Building", &GAME_BUILDING_MAP);
  load_map("Game-Path", &GAME_PATH_MAP);
};

void run_title() {
  render_map(GAME_FLOOR_MAP);
  render_map(GAME_BUILDING_MAP);
  render_map(GAME_PATH_MAP);
}

bool run_scene_gameplay(float delta) {
  gfx_clear_style_props();
  update_mouse_controls(&MOUSE_STATE);
  update_key_controls(&KEYBOARD_STATE);
  PAUSED = handle_pause();
  delta = PAUSED ? 0.0f : delta;
  move_camera(&CAMERA, delta);
  render_map(GAME_FLOOR_MAP);
  render_map(GAME_BUILDING_MAP);
  render_map(GAME_PATH_MAP);
  ecs_update_system(ECS, COMMANDER_SYSTEM.id, delta);
  ecs_update_system(ECS, SELECTOR_SYSTEM.id, delta);
  ecs_update_system(ECS, SPAWNER_SYSTEM.id, delta);
  ecs_update_system(ECS, STREAM_SPAWNER_SYSTEM.id, delta);
  ecs_update_system(ECS, TARGETER_SYSTEM.id, delta);
  ecs_update_system(ECS, MOVER_SYSTEM.id, delta);
  ecs_update_system(ECS, ROTATOR_SYSTEM.id, delta);
  ecs_update_system(ECS, COPIER_SYSTEM.id, delta);
  ecs_update_system(ECS, SHOOTER_SYSTEM.id, delta);
  ecs_update_system(ECS, DAMAGE_SYSTEM.id, delta);
  ecs_update_system(ECS, HEALTH_SYSTEM.id, delta);
  ecs_update_system(ECS, LIFE_TAKER_SYSTEM.id, delta);
  ecs_update_system(ECS, TARGETER_SYSTEM.id, delta);
  ecs_update_system(ECS, PHYSICS_SYSTEM.id, delta);
  ecs_update_system(ECS, ANIMATOR_SYSTEM.id, delta);
  ecs_update_system(ECS, RENDERER_SYSTEM.id, delta);
  ecs_update_system(ECS, DESTROYER_SYSTEM.id, delta);
  ecs_update_system(ECS, STATUS_SYSTEM.id, delta);
  phy_threaded_space_step(SPACE, delta);
  bool res = mangage_game_overlays();
  gfx_rect_no_block(gfx_get_display_width() / 2.0f, gfx_get_display_height() / 2.0f, gfx_get_display_width(), gfx_get_display_height(), OVERLAY_FILTER_COLOR, 0.0f, 0.0f);
  state.current_div.scrollable = false;
  GAME_INFO.time += delta;
  return res;
}

