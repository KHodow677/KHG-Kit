#include "area/light.h"
#include "area/ovr_tile.h"
#include "camera/camera.h"
#include "ecs/ecs_manager.h"
#include "khg_phy/core/phy_vector.h"
#include "khg_utl/vector.h"
#include "physics/physics.h"
#include "scene/builders/map_builder.h"
#include "scene/builders/structure_builder.h"
#include "scene/scene_loader.h"
#include <stdbool.h>

utl_vector *GAME_SCENES = NULL;

static bool SCENE_LOADED = false;

static void load_main() {
  physics_setup(phy_vector2_new(0.0f, 0.0f));
  ecs_setup();
  camera_setup(&CAMERA);
  setup_lights_texture();
  setup_lights_shader();
  set_ovr_tile_options();
  build_ovr_map(phy_vector2_new(960.0f, 540.0f), 0.25f);
  build_initial_structures();
}

static void unload_main() {
  ecs_cleanup();
  physics_cleanup();
  clear_ovr_tile_options(); 
}

void setup_scenes() {
  GAME_SCENES = utl_vector_create(sizeof(game_scene));
  add_scene(load_main, unload_main);
  toggle_scene(0);
}

void clear_scenes(void) {
  for (game_scene *it = utl_vector_begin(GAME_SCENES); it != utl_vector_end(GAME_SCENES); it++) {
    if (it->toggle) {
      it->unload_func();
    }
  }
  utl_vector_deallocate(GAME_SCENES);
}

const bool scene_loaded() {
  return SCENE_LOADED;
}

void add_scene(void (*load_fn)(void), void (*unload_fn)(void)) {
  game_scene scene = { false, load_fn, unload_fn };
  utl_vector_push_back(GAME_SCENES, &scene);
}

void toggle_scene(const unsigned int scene_index) {
  for (game_scene *it = utl_vector_begin(GAME_SCENES); it != utl_vector_end(GAME_SCENES); it++) {
    if (it->toggle) {
      it->unload_func();
      SCENE_LOADED = false;
    }
  }
  game_scene *scene = utl_vector_at(GAME_SCENES, scene_index);
  if (scene->toggle) {
    scene->unload_func();
    scene->toggle = false;
  }
  else {
    scene->load_func();
    scene->toggle = true;
  }
  SCENE_LOADED = scene->toggle;
}

