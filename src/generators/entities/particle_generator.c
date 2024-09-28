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
#include <math.h>

void generate_particle(particle *p, comp_physics *p_info, float x, float y) {
  p->entity = ecs_create(ECS);
  p->comp_physics = sys_physics_add(p->entity);
  p->comp_renderer = sys_renderer_add(p->entity);
  p->comp_destroyer = sys_destroyer_add(p->entity);
  p->comp_animator = sys_animator_add(p->entity);
  generate_physics_box(p->entity, p->comp_physics, false, 100, 100, 1.0f, phy_v(x, y), 0.0f, phy_v(0.0f, 0.0f), COLLISION_CATEGORY_OBJECT);
  generate_renderer(p->comp_renderer, p->comp_renderer, p->comp_physics, PARTICLE_2_0, 9);
  generate_destroyer(p->comp_destroyer);
  generate_animator(p->comp_animator, PARTICLE_2_0, PARTICLE_2_4, 0.064f, true);
  phy_body_set_angle(p->comp_physics->body, normalize_angle(phy_body_get_angle(p_info->body)) - M_PI / 2);
  phy_body_set_velocity(p->comp_physics->body, phy_body_get_velocity(p_info->body));
  p->comp_physics->move_enabled = false;
  p->comp_physics->rotate_enabled = false;
}

void free_particle(particle *p) {
  p->comp_destroyer->destroy_now = true;
  free_physics(p->comp_physics, false);
  free_renderer(p->comp_renderer);
}

