#pragma once

#include "entity/comp_destroyer.h"
#include "entity/comp_physics.h"
#include "entity/comp_renderer.h"
#include "entity/comp_stream_spawner.h"
#include "khg_ecs/ecs.h"

typedef struct {
  ecs_id entity;
  comp_physics *comp_physics;
  renderer_info renderer_info;
  comp_destroyer *comp_destroyer;
  stream_spawner_info stream_spawner_info;
} spawner;

void generate_spawner(spawner *s, float x, float y, float angle, phy_vect *path, int path_length);
void free_spawner(spawner *s);

