#pragma once

#include "khg_ecs/ecs.h"
#include <stdbool.h>

typedef struct {
  ecs_id spawner_id;
  int spawner_tex_id;
} spawn_menu_info;

extern spawn_menu_info SPAWN_SETTINGS;

bool render_spawn_menu(void);

