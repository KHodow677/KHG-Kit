#include "scene/scene_transitions.h"
#include "camera/camera.h"
#include "ecs/ecs_manager.h"
#include "graphics/light.h"
#include "physics/physics.h"
#include "khg_phy/vect.h"
#include "khg_stm/state_machine.h"
#include "resources/texture_loader.h"
#include "scene/scenes/main/animal_builder.h"
#include "scene/scenes/main/environment_builder.h"
#include "scene/scenes/main/light_builder.h"
#include "scene/scenes/main/player_builder.h"
#include "scene/scenes/main/zone_builder.h"

void load_main_scene(void *old_state_data, stm_event *event, void *new_state_data) {
  physics_setup(phy_v(0.0f, 0.0f));
  ecs_setup();
  camera_setup(&CAMERA);
  setup_lights_texture();
  setup_lights_shader();
  build_player(PLAYER_IDLE_0, PLAYER_IDLE_59, 340, 560, 9);
  build_environment_element(MAIN_ENVIRONMENT_BACKGROUND, 640, 360, 0, false, 0, 0.0f);
  build_environment_element(MAIN_ENVIRONMENT_MOUNTAIN_1, 640, 360, 0, false, 1, 0.6f);
  build_environment_element(MAIN_ENVIRONMENT_MOUNTAIN_0, 640, 360, 0, false, 2, 0.7f);
  build_environment_element(MAIN_ENVIRONMENT_TREE_1, 640, 360, 0, false, 3, 0.8f);
  build_environment_element(MAIN_ENVIRONMENT_TREE_0, 640, 360, 0, false, 4, 0.9f);
  build_environment_element(MAIN_ENVIRONMENT_GROUND, 640, 360, 0, false, 6, 1.0f);
  build_environment_element(MAIN_ENVIRONMENT_ROCK_0, 360, 600, 0, false, 5, 1.0f);
  build_environment_element(MAIN_ENVIRONMENT_MOUNTAIN_1, 1920, 360, 0, false, 1, 0.6f);
  build_environment_element(MAIN_ENVIRONMENT_MOUNTAIN_0, 1920, 360, 0, false, 2, 0.7f);
  build_environment_element(MAIN_ENVIRONMENT_TREE_1, 1920, 360, 0, false, 3, 0.8f);
  build_environment_element(MAIN_ENVIRONMENT_TREE_0, 1920, 360, 0, false, 4, 0.9f);
  build_environment_element(MAIN_ENVIRONMENT_GROUND, 1920, 360, 0, false, 7, 1.0f);
  build_animal(ANIMAL_ELK_IDLE_0, ANIMAL_ELK_IDLE_29, 780, 530, 8);
  build_environment_element(MAIN_STATION_CART, 1000, 510, 0, false, 8, 1.0f);
  build_environment_element(MAIN_STATION_TWIN_HOUSE, 2000, 410, 0, false, 6, 1.0f);
  build_light(2000, 510, 400.0f);
  build_zone(PLAYER_INFO.physics->body, 950, 1150);
};

void load_game_scene(void *old_state_data, stm_event *event, void *new_state_data) {
}

