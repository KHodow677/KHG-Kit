#include "generators/elements/tank_body_generator.h"
#include "generators/elements/tank_top_generator.h"
#include "generators/components/comp_info_generator.h"
#include "generators/entities/tank_generator.h"

void generate_tank(tank *t, ecs_ecs *ecs) {
  generate_tank_body(&t->body, ecs);
  generate_tank_top(&t->top, ecs, &t->body);
}

void free_tank(tank *t) {
  t->body.destroyer_info.destroy_now = true;
  t->top.destroyer_info.destroy_now = true;
  free_tank_body(&t->body);
  free_tank_top(&t->top);
}

