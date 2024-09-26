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

void generate_particle(particle *p, phy_body *body_body, phy_body *top_body, float x, float y) {
  p->entity = ecs_create(ECS);
  generate_physics_box(&p->physics_info, false, 80, 80, 1.0f, phy_v(x, y), 0.0f, phy_v(0.0f, 0.0f));
  generate_renderer(&p->renderer_info, &p->physics_info, PARTICLE_2_0, 9, p->entity);
  generate_destroyer(&p->destroyer_info);
  generate_animator(&p->animator_info, PARTICLE_2_0, PARTICLE_2_4, 0.032f, true);
  float ang = normalize_angle(phy_body_get_angle(top_body));
  phy_body_set_angle(p->physics_info.body, ang - M_PI / 2);
  phy_body_set_angular_velocity(p->physics_info.body, phy_body_get_angular_velocity(top_body));
  phy_body_set_velocity(p->physics_info.body, phy_body_get_velocity(body_body));
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
  utl_vector_assign(PHYSICS_INFO, p->entity, &NO_PHYSICS);
  utl_vector_assign(RENDERER_INFO, p->entity, &NO_RENDERER);
  DESTROYER_INFO[p->entity] = NO_DESTROYER;
  ANIMATOR_INFO[p->entity] = NO_ANIMATOR;
}

