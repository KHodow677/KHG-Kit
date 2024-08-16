#include "generators/entities/tank_generator.h"
#include "generators/elements/tank_body_generator.h"
#include "generators/elements/tank_top_generator.h"
#include "generators/components/comp_info_generator.h"

void generate_tank(tank *t, ecs_ecs *ecs, float x, float y) {
  generate_tank_body(&t->body, ecs, x, y);
  generate_tank_top(&t->top, ecs, &t->body, x, y);
}

void free_tank(tank *t) {
  t->body.destroyer_info.destroy_now = true;
  t->top.destroyer_info.destroy_now = true;
  free_tank_body(&t->body);
  free_tank_top(&t->top);
}

