#include "data_utl/kinematic_utl.h"
#include "generators/entities/particle_generator.h"
#include "entity/comp_animator.h"
#include "entity/comp_destroyer.h"
#include "entity/comp_follower.h"
#include "entity/comp_physics.h"
#include "entity/comp_renderer.h"
#include "game_manager.h"
#include "generators/components/comp_info_generator.h"
#include "generators/components/texture_generator.h"
#include "khg_phy/body.h"
#include "khg_phy/vect.h"
#include "khg_utl/map.h"
#include "khg_utl/vector.h"
#include <math.h>

void generate_particle(particle *p, physics_info *source_info, float x, float y) {
  generate_physics_box(&p->physics_info, false, 80, 80, 1.0f, phy_v(x, y), 0.0f, phy_v(0.0f, 0.0f));
  generate_renderer(&p->renderer_info, p->physics_info.body, PARTICLE_2_0);
  generate_destroyer(&p->destroyer_info);
  generate_animator(&p->animator_info, PARTICLE_2_0, PARTICLE_2_4, 0.032f, true);
  generate_follower(&p->follower_info, source_info, 80, 80, 1.0f, phy_v(x, y), 0.0f, phy_v(0.0f, 0.0f));
  phy_body_set_center_of_gravity(p->physics_info.body, phy_v(-145.0f, -5.0f));
  float ang = normalize_angle(phy_body_get_angle(source_info->body));
  phy_body_set_angle(p->physics_info.body, ang - M_PI / 2);
  p->entity = ecs_create(ECS);
  sys_physics_add(&p->entity, &p->physics_info);
  sys_renderer_add(&p->entity, &p->renderer_info);
  sys_destroyer_add(&p->entity, &p->destroyer_info);
  sys_animator_add(&p->entity, &p->animator_info);
  sys_follower_add(&p->entity, &p->follower_info);
}

void free_particle(particle *p) {
  p->destroyer_info.destroy_now = true;
  free_physics(&p->physics_info);
  utl_vector_assign(PHYSICS_INFO, p->entity, &NO_PHYSICS);
  utl_vector_assign(DESTROYER_INFO, p->entity, &NO_DESTROYER);
  utl_vector_assign(ANIMATOR_INFO, p->entity, &NO_ANIMATOR);
  utl_map_erase(RENDERER_INFO_MAP, &p->entity);
  utl_vector_assign(FOLLOWER_INFO, p->entity, &NO_FOLLOWER);
}

