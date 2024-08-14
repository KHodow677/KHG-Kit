#include "generators/entities/particle_generator.h"
#include "generators/components/comp_info_generator.h"
#include "generators/components/texture_generator.h"
#include "khg_phy/vect.h"

void generate_particle(particle *p, ecs_ecs *ecs) {
  generate_physics_box(&p->physics_info, false, 107, 100, 1.0f, cpv(600.0f, 300.0f), 0.0f, cpv(0.0f, 0.0f));
  generate_renderer(&p->renderer_info, &p->physics_info, PARTICLE_0);
  generate_destroyer(&p->destroyer_info);
  generate_animator(&p->animator_info, PARTICLE_0, PARTICLE_15, 2, true);
  p->entity = ecs_create(ecs);
  sys_physics_add(ecs, &p->entity, &p->physics_info);
  sys_renderer_add(ecs, &p->entity, &p->renderer_info);
  sys_destroyer_add(ecs, &p->entity, &p->destroyer_info);
  sys_animator_add(ecs, &p->entity, &p->animator_info);
}

void free_particle(particle *p) {
  p->destroyer_info.destroy_now = true;
  free_physics(&p->physics_info);
}

