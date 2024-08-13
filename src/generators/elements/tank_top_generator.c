#include "generators/elements/tank_top_generator.h"
#include "entity/comp_destroyer.h"
#include "entity/comp_follower.h"
#include "entity/comp_physics.h"
#include "entity/comp_renderer.h"
#include "generators/components/comp_info_generator.h"
#include "generators/components/texture_generator.h"
#include "generators/elements/tank_body_generator.h"
#include "khg_ecs/ecs.h"
#include "khg_phy/vect.h"

void generate_tank_top(tank_top *tt, ecs_ecs *ecs, cpSpace *sp, tank_body *tb) {
  generate_physics_box(&tt->physics_info, sp, false, 102.0f, 209.0f, 1.0f, cpv(600.0f, 300.0f), 0.0f, cpv(0.0f, 55.5f));
  generate_renderer(&tt->renderer_info, &tt->physics_info, TANK_TOP);
  generate_follower(&tt->follower_info, &tt->physics_info, &tb->physics_info, true, false);
  generate_destroyer(&tt->destroyer_info);
  tt->entity = ecs_create(ecs);
  sys_physics_add(ecs, &tt->entity, &tt->physics_info);
  sys_renderer_add(ecs, &tt->entity, &tt->renderer_info);
  sys_follower_add(ecs, &tt->entity, &tt->follower_info);
  sys_destroyer_add(ecs, &tt->entity, &tt->destroyer_info);
}

void free_tank_top(tank_top *tt, cpSpace *sp) {
  free_physics(&tt->physics_info, sp);
}

