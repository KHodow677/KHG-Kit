#include "scenes/scene_transition.h"
#include "entity/map.h"
#include "game_manager.h"
#include "generators/components/map_generator.h"
#include "khg_phy/vect.h"
#include "khg_stm/state_machine.h"
#include "menus/game_menu_manager.h"
#include "spawners/spawn_depot.h"
#include "spawners/spawn_hangar.h"
#include "spawners/spawn_spawner.h"
#include "spawners/spawn_turret.h"
#include <math.h>

void load_tutorial_scene(void *old_state_data, stm_event *event, void *new_state_data) {
  GAME_INFO.cash = 100;
  GAME_INFO.lives = 5;
  GAME_INFO.rounds_left = 5;
  load_map("Tutorial-Floor", &GAME_FLOOR_MAP);
  load_map("Tutorial-Building", &GAME_BUILDING_MAP);
  load_map("Tutorial-Path", &GAME_PATH_MAP);
  add_map_collision_segments(GAME_BUILDING_MAP, &GAME_MAP_SEGMENTS);
  setup_game_overlay();
  spawn_spawner(2162, 0, -0.5f * M_PI, (phy_vect[]){ phy_v(-1408, 0) }, 1);
  spawn_turret(-1600, -88, M_PI * 0.5f);
  spawn_turret(-1600, 80, M_PI * 0.5f);
  spawn_hangar(0, 1768, 0.0f);
  spawn_depot(-1407, -5, M_PI * 0.5f);
};

