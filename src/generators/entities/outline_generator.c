#include "generators/entities/outline_generator.h"
#include "data_utl/kinematic_utl.h"
#include "game_manager.h"
#include "generators/components/comp_info_generator.h"
#include "generators/components/texture_generator.h"
#include "khg_phy/body.h"
#include "khg_phy/vect.h"

void generate_tank_outline(tank_outline *to, phy_body *body_body, phy_body *top_body, float x, float y) {
  generate_physics_box(&to->physics_info, false, 152, 190, 1.0f, phy_v(x, y), 0.0f, phy_v(0.0f, 0.0f));
  generate_renderer(&to->renderer_info, &to->physics_info, TANK_BODY_OUTLINE, 0);
  generate_destroyer(&to->destroyer_info);
  float ang = normalize_angle(phy_body_get_angle(body_body));
  phy_body_set_angle(to->physics_info.body, ang);
  to->physics_info.move_enabled = false;
  to->physics_info.rotate_enabled = false;
  to->entity = ecs_create(ECS);
  sys_physics_add(&to->entity, &to->physics_info);
  sys_renderer_add(&to->entity, &to->renderer_info);
  sys_destroyer_add(&to->entity, &to->destroyer_info);
}

void free_tank_outline(tank_outline *to) {
  to->destroyer_info.destroy_now = true;
  free_physics(&to->physics_info, false);
  utl_vector_assign(PHYSICS_INFO, to->entity, &NO_PHYSICS);
  utl_vector_assign(RENDERER_INFO, to->entity, &NO_RENDERER);
  utl_vector_assign(DESTROYER_INFO, to->entity, &NO_DESTROYER);
}

