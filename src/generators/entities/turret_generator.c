#include "generators/entities/turret_generator.h"
#include "generators/elements/turret_base_generator.h"
#include "generators/elements/turret_top_generator.h"
#include "generators/components/comp_info_generator.h"

void generate_turret(turret *t, float x, float y) {
  generate_turret_base(&t->base, x, y);
  generate_turret_top(&t->top, &t->base, x, y);
  t->entity = t->base.entity;
}

void free_turret(turret *t) {
  t->base.destroyer_info.destroy_now = true;
  t->top.destroyer_info.destroy_now = true;
  free_turret_base(&t->base);
  free_turret_top(&t->top);
}

