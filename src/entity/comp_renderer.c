#include "entity/comp_renderer.h"
#include "entity/comp_physics.h"
#include "data_utl/map_utl.h"
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
utl_map *RENDERER_INFO_MAP = NULL;

void comp_renderer_register(comp_renderer *cr) {
  cr->id = ecs_register_component(ECS, sizeof(comp_renderer), NULL, NULL);
  RENDERER_COMPONENT_SIGNATURE = cr->id; 
}

void sys_renderer_register(sys_renderer *sr) {
  sr->id = ecs_register_system(ECS, sys_renderer_update, NULL, NULL, NULL);
  ecs_require_component(ECS, sr->id, RENDERER_COMPONENT_SIGNATURE);
  sr->ecs = *ECS;
  RENDERER_INFO_MAP = utl_map_create(compare_ints, no_deallocator, no_deallocator);
}

void sys_renderer_add(ecs_id *eid, renderer_info *info) {
  ecs_add(ECS, *eid, RENDERER_COMPONENT_SIGNATURE, NULL);
  utl_map_insert(RENDERER_INFO_MAP, eid, info);
}

void sys_renderer_free(bool need_free) {
  if (need_free) {
    utl_map_deallocate(RENDERER_INFO_MAP);
  }
}

ecs_ret sys_renderer_update(ecs_ecs *ecs, ecs_id *entities, int entity_count, ecs_dt dt, void *udata) {
  renderer_info *info = utl_map_at(RENDERER_INFO_MAP, &entities[0]);
  for (int id = 0; id < entity_count; id++) {
    info = utl_map_at(RENDERER_INFO_MAP, &entities[id]);
    phy_vect pos = phy_body_get_position(info->body);
    float angle = phy_body_get_angle(info->body);
    gfx_texture *tex = utl_vector_at(TEXTURE_LOOKUP, info->tex_id);
    tex->angle = angle;
    gfx_image_no_block(pos.x, pos.y, *tex, CAMERA.position.x, CAMERA.position.y, CAMERA.zoom);
  }
  return 0;
}
