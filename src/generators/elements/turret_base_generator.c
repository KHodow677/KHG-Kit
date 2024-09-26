#include "generators/elements/turret_base_generator.h"
#include "entity/comp_destroyer.h"
#include "entity/comp_physics.h"
#include "entity/comp_renderer.h"
#include "game_manager.h"
#include "generators/components/comp_info_generator.h"
#include "generators/components/texture_generator.h"
#include "khg_ecs/ecs.h"
#include "khg_phy/body.h"
#include "khg_phy/vect.h"
#include "khg_utl/vector.h"

void generate_turret_base(turret_base *tb, float x, float y, float angle) {
  tb->entity = ecs_create(ECS);
  generate_static_physics_circle(&tb->physics_info, true, 47.0f, phy_v(x, y), 0.0f, phy_v(0.0f, 0.0f), COLLISION_CATEGORY_ENTITY);
  phy_body_set_angle(tb->physics_info.body, angle);
  generate_renderer(&tb->renderer_info, &tb->physics_info, TURRET_BASE, 2, tb->entity);
  generate_destroyer(&tb->destroyer_info);
  sys_physics_add(&tb->entity, &tb->physics_info);
  sys_renderer_add(&tb->entity, &tb->renderer_info);
  sys_destroyer_add(&tb->entity, &tb->destroyer_info);
}

void free_turret_base(turret_base *tb) {
  free_physics(&tb->physics_info, false);
  free_renderer(&tb->renderer_info);
  PHYSICS_INFO[tb->entity] = NO_PHYSICS;
  utl_vector_assign(RENDERER_INFO, tb->entity, &NO_RENDERER);
  DESTROYER_INFO[tb->entity] = NO_DESTROYER;
}

