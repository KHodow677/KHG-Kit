#pragma once

#include "entity/comp_animator.h"
#include "entity/comp_destroyer.h"
#include "entity/comp_physics.h"
#include "entity/comp_renderer.h"
#include "khg_ecs/ecs.h"

typedef struct {
  ecs_id entity;
  comp_physics *comp_physics;
  renderer_info renderer_info;
  comp_destroyer *comp_destroyer;
  comp_animator *comp_animator;
} particle;

void generate_particle(particle *p, comp_physics *p_info, float x, float y);
void free_particle(particle *p);

