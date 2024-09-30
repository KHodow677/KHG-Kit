#include "generators/entities/turret_generator.h"
#include "generators/elements/turret_base_generator.h"
#include "generators/elements/turret_top_generator.h"
#include "generators/components/comp_info_generator.h"

void generate_turret(turret *t, float x, float y, float angle) {
  generate_turret_base(&t->base, x, y, angle);
  generate_turret_top(&t->top, &t->base, x, y, angle);
  t->entity = t->top.entity;
}

void free_turret(turret *t) {
  t->base.comp_destroyer->destroy_now = true;
  t->top.comp_destroyer->destroy_now = true;
  free_turret_base(&t->base);
  free_turret_top(&t->top);
}

