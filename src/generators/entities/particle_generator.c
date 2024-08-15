#include "generators/entities/particle_generator.h"
#include "entity/comp_animator.h"
#include "entity/comp_destroyer.h"
#include "entity/comp_physics.h"
#include "entity/comp_renderer.h"
#include "generators/components/comp_info_generator.h"
#include "generators/components/texture_generator.h"
#include "khg_phy/vect.h"
#include "khg_utl/map.h"

void generate_particle(particle *p, ecs_ecs *ecs, float x, float y) {
  generate_physics_box(&p->physics_info, false, 90, 90, 1.0f, cpv(x, y), 0.0f, cpv(0.0f, 0.0f));
  generate_renderer(&p->renderer_info, &p->physics_info, PARTICLE_2_0);
  generate_destroyer(&p->destroyer_info);
  generate_animator(&p->animator_info, PARTICLE_2_0, PARTICLE_2_4, 4, true);
  p->entity = ecs_create(ecs);
  sys_physics_add(ecs, &p->entity, &p->physics_info);
  sys_renderer_add(ecs, &p->entity, &p->renderer_info);
  sys_destroyer_add(ecs, &p->entity, &p->destroyer_info);
  sys_animator_add(ecs, &p->entity, &p->animator_info);
}

void free_particle(particle *p) {
  p->destroyer_info.destroy_now = true;
  free_physics(&p->physics_info);
  utl_map_erase(PHYSICS_INFO_MAP, &p->entity);
  utl_map_erase(RENDERER_INFO_MAP, &p->entity);
  utl_map_erase(DESTROYER_INFO_MAP, &p->entity);
  utl_map_erase(ANIMATOR_INFO_MAP, &p->entity);
}

