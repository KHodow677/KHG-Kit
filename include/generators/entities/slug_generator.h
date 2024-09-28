#pragma once

#include "entity/comp_animator.h"
#include "entity/comp_destroyer.h"
#include "entity/comp_health.h"
#include "entity/comp_mover.h"
#include "entity/comp_physics.h"
#include "entity/comp_renderer.h"
#include "khg_ecs/ecs.h"

typedef struct {
  ecs_id entity;
  comp_physics *comp_physics;
  comp_renderer *comp_renderer;
  comp_destroyer *comp_destroyer;
  comp_animator *comp_animator;
  comp_mover *comp_mover;
  comp_health *comp_health;
} slug;

void generate_slug(slug *s, float x, float y, float angle, phy_vect *init_path, int init_path_length);
void free_slug(slug *s);
