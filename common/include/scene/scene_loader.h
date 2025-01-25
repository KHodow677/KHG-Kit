#pragma once

#include "khg_utl/list.h"
#include <stdbool.h>

#define NUM_SCENES 1

typedef struct game_scene {
  bool toggle;
  unsigned int scene_id;
  void (*load_func)(void);
  void (*unload_func)(void);
} game_scene;

extern utl_list *GAME_SCENES;
extern bool SCENE_LOADED;

void setup_scenes(void);
void clear_scenes(void);

void add_scene(const unsigned int id, void (*load_fn)(void), void (*unload_fn)(void));
void toggle_scene(const unsigned int scene_id);

