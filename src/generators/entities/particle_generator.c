#include "generators/entities/particle_generator.h"
#include "data_utl/kinematic_utl.h"
#include "entity/comp_animator.h"
#include "entity/comp_destroyer.h"
#include "entity/comp_physics.h"
#include "entity/comp_renderer.h"
#include "game_manager.h"
#include "generators/components/comp_info_generator.h"
#include "generators/components/texture_generator.h"
#include "khg_phy/body.h"
#include "khg_phy/vect.h"
#include "khg_utl/vector.h"
#include <math.h>
#include <stdio.h>

void generate_particle(particle *p, physics_info *p_info, float x, float y) {
  p->entity = ecs_create(ECS);
  generate_physics_box(p->entity, &p->physics_info, false, 100, 100, 1.0f, phy_v(x, y), 0.0f, phy_v(0.0f, 0.0f), COLLISION_CATEGORY_OBJECT);
  generate_renderer(&p->renderer_info, &p->physics_info, PARTICLE_2_0, 9, p->entity);
  generate_destroyer(&p->destroyer_info);
  generate_animator(&p->animator_info, PARTICLE_2_0, PARTICLE_2_4, 0.064f, true);
  phy_body_set_angle(p->physics_info.body, normalize_angle(phy_body_get_angle(p_info->body)) - M_PI / 2);
  /*phy_body_set_angular_velocity(p->physics_info.body, phy_body_get_angular_velocity(p_info->body));*/
  phy_body_set_velocity(p->physics_info.body, phy_body_get_velocity(p_info->body));
  p->physics_info.move_enabled = false;
  p->physics_info.rotate_enabled = false;
  sys_physics_add(&p->entity, &p->physics_info);
  sys_renderer_add(&p->entity, &p->renderer_info);
  sys_destroyer_add(&p->entity, &p->destroyer_info);
  sys_animator_add(&p->entity, &p->animator_info);
}

void free_particle(particle *p) {
  p->destroyer_info.destroy_now = true;
  free_physics(&p->physics_info, false);
  free_renderer(&p->renderer_info);
  PHYSICS_INFO[p->entity] = NO_PHYSICS;
  utl_vector_assign(RENDERER_INFO, p->entity, &NO_RENDERER);
  DESTROYER_INFO[p->entity] = NO_DESTROYER;
  ANIMATOR_INFO[p->entity] = NO_ANIMATOR;
}

