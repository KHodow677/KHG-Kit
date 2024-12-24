#pragma once

#include "ecs/comp_animator.h"
#include "ecs/comp_light.h"
#include "ecs/comp_mover.h"
#include "ecs/comp_physics.h"
#include "ecs/comp_renderer.h"

typedef struct player_info {
  ecs_id id;
  comp_physics *physics;
  comp_renderer *renderer;
  comp_light *light;
  comp_animator *animator;
  comp_mover *mover;
} player_info;

extern player_info PLAYER_INFO;

void build_player(const float x, const float y, const int render_layer);

