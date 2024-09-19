#include "generators/elements/turret_base_generator.h"
#include "entity/comp_destroyer.h"
#include "entity/comp_physics.h"
#include "entity/comp_renderer.h"
#include "game_manager.h"
#include "generators/components/comp_info_generator.h"
#include "generators/components/texture_generator.h"
#include "khg_ecs/ecs.h"
#include "khg_phy/vect.h"
#include "khg_utl/vector.h"

void generate_turret_base(turret_base *tb, float x, float y) {
  generate_static_physics_box(&tb->physics_info, true, 135.0f, 128.0f, phy_v(x, y), 0.0f, phy_v(0.0f, 0.0f));
  generate_renderer(&tb->renderer_info, &tb->physics_info, TURRET_BASE, 1);
  generate_destroyer(&tb->destroyer_info);
  tb->entity = ecs_create(ECS);
  sys_physics_add(&tb->entity, &tb->physics_info);
  sys_renderer_add(&tb->entity, &tb->renderer_info);
  sys_destroyer_add(&tb->entity, &tb->destroyer_info);
}

void free_turret_base(turret_base *tb) {
  free_physics(&tb->physics_info, false);
  free_renderer(&tb->renderer_info);
  utl_vector_assign(PHYSICS_INFO, tb->entity, &NO_PHYSICS);
  utl_vector_assign(RENDERER_INFO, tb->entity, &NO_RENDERER);
  utl_vector_assign(DESTROYER_INFO, tb->entity, &NO_DESTROYER);
  utl_vector_assign(MOVER_INFO, tb->entity, &NO_MOVER);
}

