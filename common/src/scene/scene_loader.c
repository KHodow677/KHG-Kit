#include "ecs/ecs_manager.h"
#include "khg_phy/core/phy_vector.h"
#include "khg_utl/list.h"
#include "scene/builders/map_builder.h"
#include "scene/builders/structure_builder.h"
#include "scene/scene_loader.h"
#include "util/camera/camera.h"
#include "util/physics.h"
#include <stdbool.h>
#include <stdio.h>

utl_list *GAME_SCENES = NULL;
bool SCENE_LOADED = false;

static int compare_scenes(const void *a, const void *b) {
  game_scene int_a = *(const game_scene *)a;
  game_scene int_b = *(const game_scene *)b;
  return (int_a.scene_id > int_b.scene_id) - (int_a.scene_id < int_b.scene_id);
}

static void load_main() {
  physics_setup(phy_vector2_new(0.0f, 0.0f));
  ecs_setup();
  camera_setup(&CAMERA);
  build_ovr_map(phy_vector2_new(960.0f, 540.0f), 0.25f);
  build_initial_structures();
}

static void unload_main() {
  ecs_cleanup();
  physics_cleanup();
}

void setup_scenes() {
  GAME_SCENES = utl_list_create(sizeof(game_scene), compare_scenes);
  add_scene(0, load_main, unload_main);
  toggle_scene(0);
}

void clear_scenes(void) {
  for (utl_node *node = utl_list_begin(GAME_SCENES); node != utl_list_end(GAME_SCENES); node = node->next) {
    game_scene *scene = node->value;
    if (scene->toggle) {
      scene->unload_func();
    }
  }
  utl_list_deallocate(GAME_SCENES);
}

void add_scene(const unsigned int id, void (*load_fn)(void), void (*unload_fn)(void)) {
  game_scene scene = { false, id, load_fn, unload_fn };
  utl_list_push_back(GAME_SCENES, &scene);
  utl_list_sort(GAME_SCENES);
}

void toggle_scene(const unsigned int scene_id) {
  for (utl_node *node = utl_list_begin(GAME_SCENES); node != utl_list_end(GAME_SCENES); node = node->next) {
    game_scene *scene = node->value;
    if (scene->scene_id == scene_id) {
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
    else if (scene->toggle) {
      scene->unload_func();
      SCENE_LOADED = false;
    }
  }
}

