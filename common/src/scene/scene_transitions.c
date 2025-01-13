#include "area/light.h"
#include "scene/scene_transitions.h"
#include "camera/camera.h"
#include "ecs/ecs_manager.h"
#include "physics/physics.h"
#include "khg_phy/core/phy_vector.h"
#include "khg_stm/state_machine.h"
#include "scene/scenes/main/map_builder.h"

void load_main_scene(void *old_state_data, stm_event *event, void *new_state_data) {
  physics_setup(phy_vector2_new(0.0f, 0.0f));
  ecs_setup();
  camera_setup(&CAMERA);
  setup_lights_texture();
  setup_lights_shader();
  build_ovr_map(phy_vector2_new(960.0f, 540.0f), 0.25f);
};

void load_game_scene(void *old_state_data, stm_event *event, void *new_state_data) {
}

