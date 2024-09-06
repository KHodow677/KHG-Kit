#include "entity/comp_renderer.h"
#include "game_manager.h"
#include "khg_ecs/ecs.h"
#include "khg_gfx/elements.h"
#include "khg_gfx/texture.h"
#include "khg_phy/body.h"
#include "khg_phy/phy_types.h"
#include "khg_utl/vector.h"
#include <stdlib.h>
#include <stdio.h>

ecs_id RENDERER_COMPONENT_SIGNATURE;
renderer_info NO_RENDERER = { 0 };
utl_vector *RENDERER_INFO = NULL;

static ecs_ret sys_renderer_update(ecs_ecs *ecs, ecs_id *entities, int entity_count, ecs_dt dt, void *udata) {
  renderer_info *info;
  for (int layer = 0; layer < 10; layer++) {
    for (int id = 0; id < entity_count; id++) {
      info = utl_vector_at(RENDERER_INFO, entities[id]);
      if (layer != info->render_layer) {
        continue;
      }
      phy_vect pos = phy_body_get_position(info->body);
      float angle = phy_body_get_angle(info->body);
      gfx_texture *tex = utl_vector_at(TEXTURE_LOOKUP, info->tex_id);
      tex->angle = angle;
      gfx_image_no_block(pos.x, pos.y, *tex, CAMERA.position.x, CAMERA.position.y, CAMERA.zoom);
    }
  }
  return 0;
}

void comp_renderer_register(comp_renderer *cr) {
  cr->id = ecs_register_component(ECS, sizeof(comp_renderer), NULL, NULL);
  RENDERER_COMPONENT_SIGNATURE = cr->id; 
}

void sys_renderer_register(sys_renderer *sr) {
  sr->id = ecs_register_system(ECS, sys_renderer_update, NULL, NULL, NULL);
  ecs_require_component(ECS, sr->id, RENDERER_COMPONENT_SIGNATURE);
  ecs_require_component(ECS, sr->id, PHYSICS_COMPONENT_SIGNATURE);
  sr->ecs = *ECS;
  RENDERER_INFO = utl_vector_create(sizeof(renderer_info));
  for (int i = 0; i < ECS->entity_count; i++) {
    utl_vector_push_back(RENDERER_INFO, &NO_RENDERER);
  }
}

void sys_renderer_add(ecs_id *eid, renderer_info *info) {
  ecs_add(ECS, *eid, RENDERER_COMPONENT_SIGNATURE, NULL);
  utl_vector_assign(RENDERER_INFO, *eid, info);
}

void sys_renderer_free(bool need_free) {
  if (need_free) {
    utl_vector_deallocate(RENDERER_INFO);
  }
}

