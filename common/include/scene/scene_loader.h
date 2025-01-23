#pragma once

#include "khg_utl/vector.h"
#include <stdbool.h>

#define NUM_SCENES 1

typedef struct game_scene {
  bool toggle;
  void (*load_func)(void);
  void (*unload_func)(void);
} game_scene;

extern utl_vector *GAME_SCENES;
extern bool SCENE_LOADED;

void setup_scenes(void);
void clear_scenes(void);

void add_scene(void (*load_fn)(void), void (*unload_fn)(void));
void toggle_scene(const unsigned int scene_index);

