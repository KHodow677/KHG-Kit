#include "generators/elements/tank_top_generator.h"
#include "entity/comp_physics.h"
#include "entity/comp_renderer.h"
#include "generators/components/comp_info_generator.h"
#include "khg_ecs/ecs.h"
#include "khg_phy/vect.h"

void generate_tank_top(tank_top *tt, ecs_ecs *ecs, cpSpace *sp) {
  generate_physics_box(&tt->physics_info, sp, false, 102.0f, 209.0f, 1.0f, cpv(200.0f, 150.0f), 0.0f, cpv(0.0f, 55.5f));
  generate_renderer(&tt->renderer_info, &tt->physics_info, "Tank-Top-Blue", "png");
  tt->entity = ecs_create(ecs);
  sys_physics_add(ecs, &tt->entity, &tt->physics_info);
  sys_renderer_add(ecs, &tt->entity, &tt->renderer_info);
}

void free_tank_top(tank_top *tt) {
  free_physics(&tt->physics_info);
  free_renderer(&tt->renderer_info);
}

