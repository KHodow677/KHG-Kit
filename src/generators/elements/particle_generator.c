#include "generators/elements/particle_generator.h"
#include "generators/components/comp_info_generator.h"
#include "khg_gfx/texture.h"
#include "khg_phy/vect.h"
#include "khg_utl/vector.h"
#include <stdlib.h>

void generate_particle(particle *p, ecs_ecs *ecs, cpSpace *sp) {
  utl_vector *tex_vec = utl_vector_create(sizeof(gfx_texture *));
  generate_physics_box(&p->physics_info, sp, false, 107, 100, 1.0f, cpv(600.0f, 300.0f), 0.0f, cpv(0.0f, 0.0f));
  generate_renderer(&p->renderer_info, &p->physics_info, "particles/ef_22_0000", "png", 107, 100);
  generate_destroyer(&p->destroyer_info);
  generate_animator(&p->animator_info, tex_vec, 1);
  p->entity = ecs_create(ecs);
  sys_physics_add(ecs, &p->entity, &p->physics_info);
  sys_renderer_add(ecs, &p->entity, &p->renderer_info);
  sys_destroyer_add(ecs, &p->entity, &p->destroyer_info);
  sys_animator_add(ecs, &p->entity, &p->animator_info);
  gfx_texture *tex1 = malloc(sizeof(gfx_texture));
  *tex1 = gfx_load_texture_asset("particles/ef_22_0000", "png");
  tex1->width = 107;
  tex1->height = 100;
  utl_vector_push_back(p->animator_info.tex_vec, &tex1);
  gfx_texture *tex2 = malloc(sizeof(gfx_texture));
  *tex2 = gfx_load_texture_asset("particles/ef_22_0001", "png");
  tex2->width = 107;
  tex2->height = 100;
  utl_vector_push_back(p->animator_info.tex_vec, &tex2);
}

void free_particle(particle *p, cpSpace *sp) {
  free_physics(&p->physics_info, sp);
  free_animator(&p->animator_info);
}

