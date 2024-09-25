#pragma once

#include "entity/comp_animator.h"
#include "entity/comp_destroyer.h"
#include "entity/comp_mover.h"
#include "entity/comp_physics.h"
#include "entity/comp_renderer.h"
#include "khg_ecs/ecs.h"

typedef struct {
  ecs_id entity;
  physics_info physics_info;
  renderer_info renderer_info;
  destroyer_info destroyer_info;
  animator_info animator_info;
  mover_info mover_info;
} slug;

void generate_slug(slug *s, float x, float y, float angle, phy_vect *init_path, int init_path_length);
void free_slug(slug *s);
