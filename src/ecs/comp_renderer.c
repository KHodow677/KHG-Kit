#include "ecs/comp_renderer.h"
#include "camera/camera.h"
#include "ecs/comp_physics.h"
#include "ecs/ecs_manager.h"
#include "resources/texture_loader.h"
#include "khg_ecs/ecs.h"
#include "khg_gfx/elements.h"
#include "khg_gfx/texture.h"
#include "khg_phy/body.h"
#include "khg_phy/phy_types.h"
#include <stdlib.h>
#include <stdio.h>

ecs_id RENDERER_COMPONENT_SIGNATURE;
ecs_id RENDERER_SYSTEM_SIGNATURE;

static ecs_ret sys_renderer_update(ecs_ecs *ecs, ecs_id *entities, int entity_count, ecs_dt dt, void *udata) {
  for (int layer = 0; layer < 10; layer++) {
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

void sys_renderer_register() {
  RENDERER_SYSTEM_SIGNATURE = ecs_register_system(ECS, sys_renderer_update, NULL, NULL, NULL);
  ecs_require_component(ECS, RENDERER_SYSTEM_SIGNATURE, RENDERER_COMPONENT_SIGNATURE);
  ecs_require_component(ECS, RENDERER_SYSTEM_SIGNATURE, PHYSICS_COMPONENT_SIGNATURE);
}

comp_renderer *sys_renderer_add(ecs_id eid) {
  return ecs_add(ECS, eid, RENDERER_COMPONENT_SIGNATURE, NULL);
}

void generate_renderer(comp_renderer *info, comp_physics *p_info, int tex_id, int render_layer) {
  info->tex_id = tex_id;
  info->body = p_info->body;
  info->render_layer = render_layer;
}

