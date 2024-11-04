#pragma once

#include "ecs/comp_animator.h"
#include "ecs/comp_light.h"
#include "ecs/comp_mover.h"
#include "ecs/comp_physics.h"
#include "ecs/comp_renderer.h"

typedef struct player_info {
  ecs_id id;
  const comp_physics *physics;
  const comp_renderer *renderer;
  const comp_animator *animator;
  const comp_light *light;
  const comp_mover *mover;
} player_info;

extern player_info PLAYER_INFO;

void build_player(const int min_tex_id, const int max_tex_id, const float x, const float y, const int render_layer);

