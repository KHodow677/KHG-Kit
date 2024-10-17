#include "scene/scene_transitions.h"
#include "camera/camera.h"
#include "ecs/ecs_manager.h"
#include "lighting/light.h"
#include "physics/physics.h"
#include "khg_phy/vect.h"
#include "khg_stm/state_machine.h"
#include "resources/texture_loader.h"
#include "scene/scenes/main/environment.h"

void load_main_scene(void *old_state_data, stm_event *event, void *new_state_data) {
  physics_setup(phy_v(0.0f, 0.0f));
  ecs_setup();
  camera_setup(&CAMERA);
  setup_lights_texture();
  setup_lights_shader();
  add_light((vec2s){ 0.5f, 0.5f }, 400.0f);
  /*clear_lights();*/
  generate_environment_element(SQUARE, 0, 0, 0, false);
};

void load_game_scene(void *old_state_data, stm_event *event, void *new_state_data) {
}

