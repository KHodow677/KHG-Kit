#include "area/light.h"
#include "scene/scene_transitions.h"
#include "camera/camera.h"
#include "ecs/ecs_manager.h"
#include "physics/physics.h"
#include "resources/area_loader.h"
#include "scene/scenes/main/area_builder.h"
#include "scene/scenes/main/player_builder.h"
#include "khg_phy/core/phy_vector.h"
#include "khg_stm/state_machine.h"

void load_main_scene(void *old_state_data, stm_event *event, void *new_state_data) {
  physics_setup(phy_vector2_new(0.0f, 1000.0f));
  ecs_setup();
  camera_setup(&CAMERA);
  setup_lights_texture();
  setup_lights_shader();
  build_area(DUNGEON_BG, 7, true);
  build_area(DUNGEON_0, 8, false);
  build_player(256, 726, 9);
  /*build_player(256, 826, 9);*/
  /*build_zone(PLAYER_INFO.physics->body, 950, 1150);*/
};

void load_game_scene(void *old_state_data, stm_event *event, void *new_state_data) {
}

