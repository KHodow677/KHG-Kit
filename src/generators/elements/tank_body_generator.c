#include "generators/elements/tank_body_generator.h"
#include "entity/comp_destroyer.h"
#include "entity/comp_physics.h"
#include "entity/comp_renderer.h"
#include "generators/components/comp_info_generator.h"
#include "khg_ecs/ecs.h"
#include "khg_phy/vect.h"

void generate_tank_body(tank_body *tb, ecs_ecs *ecs, cpSpace *sp) {
  generate_physics_box(&tb->physics_info, sp, false, 145.0f, 184.0f, 1.0f, cpv(600.0f, 300.0f), 0.0f, cpv(0.0f, 0.0f));
  generate_renderer(&tb->renderer_info, &tb->physics_info, "Tank-Body-Blue", "png", 145, 184);
  generate_destroyer(&tb->destroyer_info);
  tb->entity = ecs_create(ecs);
  sys_physics_add(ecs, &tb->entity, &tb->physics_info);
  sys_renderer_add(ecs, &tb->entity, &tb->renderer_info);
  sys_destroyer_add(ecs, &tb->entity, &tb->destroyer_info);
}

void free_tank_body(tank_body *tb, cpSpace *sp) {
  free_physics(&tb->physics_info, sp);
  free_renderer(&tb->renderer_info);
}

