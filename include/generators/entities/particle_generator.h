#pragma once

#include "entity/comp_animator.h"
#include "entity/comp_destroyer.h"
#include "entity/comp_follower.h"
#include "entity/comp_physics.h"
#include "entity/comp_renderer.h"
#include "khg_ecs/ecs.h"

typedef struct {
  ecs_id entity;
  physics_info physics_info;
  renderer_info renderer_info;
  follower_info follower_info;
  destroyer_info destroyer_info;
  animator_info animator_info;
} particle;

void generate_particle(particle *p, phy_body *body_body, phy_body *top_body, float x, float y);
void free_particle(particle *p);

