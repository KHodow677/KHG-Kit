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
  tb->comp_physics = sys_physics_add(tb->entity);
  tb->comp_destroyer = sys_destroyer_add(tb->entity);
  generate_destroyer(tb->comp_destroyer);
  generate_static_physics_circle(tb->entity, tb->comp_physics, true, 47.0f, phy_v(x, y), 0.0f, phy_v(0.0f, 0.0f), COLLISION_CATEGORY_OBJECT);
  phy_body_set_angle(tb->comp_physics->body, angle);
  generate_renderer(&tb->renderer_info, tb->comp_physics, TURRET_BASE, 2, tb->entity);
  sys_renderer_add(&tb->entity, &tb->renderer_info);
}

void free_turret_base(turret_base *tb) {
  free_physics(tb->comp_physics, false);
  free_renderer(&tb->renderer_info);
  utl_vector_assign(RENDERER_INFO, tb->entity, &NO_RENDERER);
}

