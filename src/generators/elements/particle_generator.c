#include "generators/elements/particle_generator.h"
#include "generators/components/comp_info_generator.h"
#include "khg_phy/vect.h"

void generate_particle(particle *p, ecs_ecs *ecs, cpSpace *sp) {
  generate_physics_box(&p->physics_info, sp, false, 107, 100, 1.0f, cpv(600.0f, 300.0f), 0.0f, cpv(0.0f, 0.0f));
  generate_renderer(&p->renderer_info, &p->physics_info, "particles/ef_22_0000", "png", 107, 100);
  generate_destroyer(&p->destroyer_info);
  p->entity = ecs_create(ecs);
  sys_physics_add(ecs, &p->entity, &p->physics_info);
  sys_renderer_add(ecs, &p->entity, &p->renderer_info);
  sys_destroyer_add(ecs, &p->entity, &p->destroyer_info);
}

void free_particle(particle *p, cpSpace *sp) {
  free_physics(&p->physics_info, sp);
  free_renderer(&p->renderer_info);
}

