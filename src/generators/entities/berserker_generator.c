#include "generators/entities/berserker_generator.h"
#include "generators/elements/berserker_body_generator.h"
#include "generators/elements/berserker_top_generator.h"
#include "generators/components/comp_info_generator.h"

void generate_berserker(berserker *b, float x, float y, float angle) {
  generate_berserker_body(&b->body, x, y, angle);
  generate_berserker_top(&b->top, &b->body, x, y, angle);
  b->entity = b->top.entity;
}

void free_berserker(berserker *b) {
  b->body.comp_destroyer->destroy_now = true;
  b->top.comp_destroyer->destroy_now = true;
  free_berserker_body(&b->body);
  free_berserker_top(&b->top);
}

