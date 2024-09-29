#include "generators/entities/berserker_clone_generator.h"
#include "generators/components/comp_info_generator.h"

void generate_berserker_clone(berserker_clone *bc, float x, float y, float angle) {
  generate_berserker_clone_body(&bc->body, x, y, angle);
  generate_berserker_clone_top(&bc->top, &bc->body, x, y, angle);
  bc->entity = bc->top.entity;
}

void free_berserker_clone(berserker_clone *bc) {
  bc->body.comp_destroyer->destroy_now = true;
  bc->top.comp_destroyer->destroy_now = true;
  free_berserker_clone_body(&bc->body);
  free_berserker_clone_top(&bc->top);
}

