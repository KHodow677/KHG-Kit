#include "generators/entities/berserker_generator.h"
#include "generators/elements/berserker_body_generator.h"
#include "generators/elements/berserker_top_generator.h"
#include "generators/components/comp_info_generator.h"

void generate_berserker(berserker *t, float x, float y, float angle) {
  generate_berserker_body(&t->body, x, y, angle);
  generate_berserker_top(&t->top, &t->body, x, y, angle);
  t->entity = t->top.entity;
}

void free_berserker(berserker *t) {
  t->body.comp_destroyer->destroy_now = true;
  t->top.comp_destroyer->destroy_now = true;
  free_berserker_body(&t->body);
  free_berserker_top(&t->top);
}

