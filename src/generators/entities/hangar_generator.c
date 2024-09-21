#include "generators/entities/hangar_generator.h"
#include "game_manager.h"
#include "generators/components/comp_info_generator.h"
#include "generators/components/texture_generator.h"
#include "khg_phy/vect.h"

void generate_hangar(hangar *h, float x, float y) {
  h->entity = ecs_create(ECS);
  generate_physics_box(&h->physics_info, false, 80, 80, 1.0f, phy_v(x, y), 0.0f, phy_v(0.0f, 0.0f));
  generate_static_renderer_segments(&h->renderer_info, &h->physics_info, phy_v(x, y), HANGAR, 1, h->entity);
  generate_destroyer(&h->destroyer_info);
  sys_physics_add(&h->entity, &h->physics_info);
  sys_renderer_add(&h->entity, &h->renderer_info);
  sys_destroyer_add(&h->entity, &h->destroyer_info);
}

void free_hangar(hangar *h) {
  free_physics(&h->physics_info, false);
  free_renderer(&h->renderer_info);
  utl_vector_assign(PHYSICS_INFO, h->entity, &NO_PHYSICS);
  utl_vector_assign(RENDERER_INFO, h->entity, &NO_RENDERER);
  utl_vector_assign(DESTROYER_INFO, h->entity, &NO_DESTROYER);
}

