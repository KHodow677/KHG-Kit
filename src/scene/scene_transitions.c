#include "scene/scene_transitions.h"
#include "camera/camera.h"
#include "ecs/ecs_manager.h"
#include "graphics/light.h"
#include "physics/physics.h"
#include "khg_phy/vect.h"
#include "khg_stm/state_machine.h"
#include "resources/texture_loader.h"
#include "scene/scenes/main/environment.h"
#include "scene/scenes/main/player.h"

void load_main_scene(void *old_state_data, stm_event *event, void *new_state_data) {
  physics_setup(phy_v(0.0f, 0.0f));
  ecs_setup();
  camera_setup(&CAMERA);
  setup_lights_texture();
  setup_lights_shader();
  add_light((vec2s){ 0.5f, 0.75f }, 250.0f);
  add_light((vec2s){ 0.75f, 0.75f }, 250.0f);
  /*clear_lights();*/
  generate_environment_element(MAIN_ENVIRONMENT_BACKGROUND, 640, 360, 0, false, 0);
  generate_environment_element(MAIN_ENVIRONMENT_MOUNTAIN_1, 640, 360, 0, false, 1);
  generate_environment_element(MAIN_ENVIRONMENT_MOUNTAIN_0, 640, 360, 0, false, 2);
  generate_environment_element(MAIN_ENVIRONMENT_TREE_1, 640, 360, 0, false, 3);
  generate_environment_element(MAIN_ENVIRONMENT_TREE_0, 640, 360, 0, false, 4);
  generate_environment_element(MAIN_ENVIRONMENT_GROUND, 640, 360, 0, false, 6);
  generate_environment_element(MAIN_ENVIRONMENT_ROCK_0, 360, 600, 0, false, 5);
  generate_environment_element(ANIMAL_ELK_IDLE_0, 800, 540, 0, false, 7);
  generate_environment_element(MAIN_STATION_CART, 1000, 510, 0, false, 7);
  generate_player(PLAYER_IDLE_0, PLAYER_IDLE_59, 640, 560, 8);
};

void load_game_scene(void *old_state_data, stm_event *event, void *new_state_data) {
}

