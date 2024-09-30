#include "entity/comp_renderer.h"
#include "entity/comp_physics.h"
#include "entity/indicators.h"
#include "game_manager.h"
#include "generators/components/texture_generator.h"
#include "khg_ecs/ecs.h"
#include "khg_gfx/elements.h"
#include "khg_gfx/texture.h"
#include "khg_phy/body.h"
#include "khg_phy/phy_types.h"
#include "khg_utl/vector.h"
#include <stdlib.h>
#include <stdio.h>

ecs_id RENDERER_COMPONENT_SIGNATURE;

static ecs_ret sys_renderer_update(ecs_ecs *ecs, ecs_id *entities, int entity_count, ecs_dt dt, void *udata) {
  for (int layer = 0; layer < 10; layer++) {
    if (layer == 9) {
      for (int id = 0; id < entity_count; id++) {
        comp_renderer *info = ecs_get(ECS, entities[id], RENDERER_COMPONENT_SIGNATURE);
        comp_physics *p_info = ecs_get(ECS, entities[id], PHYSICS_COMPONENT_SIGNATURE);
        for (int i_index = 0; i_index < utl_vector_size(info->indicators); i_index++) {
          indicator *ind = utl_vector_at(info->indicators, i_index);
          if (ind->type == INDICATOR_POINT) {
            render_point(ind);
          }
          else if (ind->type == INDICATOR_LINE) {
            render_line(ind);
          }
          else if (ind->type == INDICATOR_BODY_POINT) {
            render_body_point(p_info, ind);
          }
          else if (ind->type == INDICATOR_BODY_LINE) {
            render_body_line(p_info, ind);
          }
        }
      }
    }
    for (int id = 0; id < entity_count; id++) {
      comp_renderer *info = ecs_get(ECS, entities[id], RENDERER_COMPONENT_SIGNATURE);
      if (layer != info->render_layer) {
        continue;
      }
      phy_vect pos = phy_body_get_position(info->body);
      phy_vect offset = phy_body_get_center_of_gravity(info->body);
      float angle = phy_body_get_angle(info->body);
      gfx_texture *tex = get_or_add_texture(info->tex_id);
      tex->angle = angle;
      gfx_image_no_block(pos.x, pos.y, *tex, offset.x, offset.y, CAMERA.position.x, CAMERA.position.y, CAMERA.zoom, true);
    }
  }
  return 0;
}

void comp_renderer_register() {
  RENDERER_COMPONENT_SIGNATURE = ecs_register_component(ECS, sizeof(comp_renderer), NULL, NULL);
}

void sys_renderer_register(sys_renderer *sr) {
  sr->id = ecs_register_system(ECS, sys_renderer_update, NULL, NULL, NULL);
  ecs_require_component(ECS, sr->id, RENDERER_COMPONENT_SIGNATURE);
  ecs_require_component(ECS, sr->id, PHYSICS_COMPONENT_SIGNATURE);
  sr->ecs = *ECS;
}

comp_renderer *sys_renderer_add(ecs_id eid) {
  return ecs_add(ECS, eid, RENDERER_COMPONENT_SIGNATURE, NULL);
}

