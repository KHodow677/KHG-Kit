#include "ecs/comp_renderer.h"
#include "camera/camera.h"
#include "ecs/comp_physics.h"
#include "ecs/ecs_manager.h"
#include "letterbox.h"
#include "resources/texture_loader.h"
#include "khg_ecs/ecs.h"
#include "khg_gfx/elements.h"
#include "khg_gfx/texture.h"
#include "khg_phy/body.h"
#include "khg_phy/phy_types.h"

ecs_id RENDERER_COMPONENT_SIGNATURE;
ecs_id RENDERER_SYSTEM_SIGNATURE;

comp_renderer_constructor_info *RENDERER_CONSTRUCTOR_INFO = NULL;

static ecs_ret sys_renderer_update(ecs_ecs *ecs, ecs_id *entities, const int entity_count, const ecs_dt dt, void *udata) {
  for (int layer = 0; layer < 10; layer++) {
    for (int id = 0; id < entity_count; id++) {
      comp_renderer *info = ecs_get(ECS, entities[id], RENDERER_COMPONENT_SIGNATURE);
      if (layer != info->render_layer) {
        continue;
      }
      phy_vect pos = phy_body_get_position(info->body);
      phy_vect offset = phy_body_get_center_of_gravity(info->body);
      float angle = phy_body_get_angle(info->body);
      gfx_texture tex_ref = get_or_add_texture(info->tex_id);
      gfx_texture tex = { tex_ref.id, tex_ref.width, tex_ref.height, tex_ref.angle };
      transform_letterbox_element(LETTERBOX, &pos, &tex, offset.x, offset.y);
      gfx_image_no_block(pos.x, pos.y, tex, offset.x, offset.y, CAMERA.position.x, CAMERA.position.y, CAMERA.zoom, true);
    }
  }
  return 0;
}

static void comp_renderer_constructor(ecs_ecs *ecs, const ecs_id entity_id, void *ptr, void *args) {
  comp_renderer *info = ptr;
  const comp_renderer_constructor_info *constructor_info = RENDERER_CONSTRUCTOR_INFO;
  if (info && constructor_info) {
    info->body = constructor_info->body;
    info->tex_id = constructor_info->tex_id;
    info->render_layer = constructor_info->render_layer;
  }
}

void comp_renderer_register() {
  RENDERER_COMPONENT_SIGNATURE = ecs_register_component(ECS, sizeof(comp_renderer), comp_renderer_constructor, NULL);
}

void sys_renderer_register() {
  RENDERER_SYSTEM_SIGNATURE = ecs_register_system(ECS, sys_renderer_update, NULL, NULL, NULL);
  ecs_require_component(ECS, RENDERER_SYSTEM_SIGNATURE, RENDERER_COMPONENT_SIGNATURE);
  ecs_require_component(ECS, RENDERER_SYSTEM_SIGNATURE, PHYSICS_COMPONENT_SIGNATURE);
}

comp_renderer *sys_renderer_add(ecs_id eid, comp_renderer_constructor_info *crci) {
  RENDERER_CONSTRUCTOR_INFO = crci;
  return ecs_add(ECS, eid, RENDERER_COMPONENT_SIGNATURE, NULL);
}

