#define NAMESPACE_ELEMENT_IMPL
#define NAMESPACE_TASKING_USE

#include "element/namespace.h"
#include "khg_ecs/ecs.h"
#include "khg_gfx/elements.h"
#include "khg_gfx/texture.h"
#include "tasking/namespace.h"
#include "util/camera/camera.h"
#include "util/ovr_tile.h"
#include "util/letterbox.h"
#include <stdio.h>
#include <string.h>

static ecs_ret sys_render_update(ecs_ecs *ecs, ecs_id *entities, const unsigned int entity_count, const ecs_dt dt, void *udata) {
  if (dt == 0.0f) {
    return 0;
  }
  for (unsigned int layer = 0; layer < 10; layer++) {
    for (int id = 0; id < entity_count; id++) {
      comp_render *info = ecs_get(NAMESPACE_ELEMENT_INTERNAL.ECS, entities[id], NAMESPACE_ELEMENT_INTERNAL.RENDER_INFO.component_signature);
      comp_physics *p_info = ecs_get(NAMESPACE_ELEMENT_INTERNAL.ECS, entities[id], NAMESPACE_ELEMENT_INTERNAL.PHYSICS_INFO.component_signature);
      if (info->ovr_tile.tile_id) {
        render_ovr_tile(&info->ovr_tile, &layer, dt);
        continue;
      }
      if (layer != info->render_layer) {
        continue;
      }
      kin_vec pos = p_info->body->pos;
      kin_vec cam_pos = (kin_vec){ CAMERA.position.x, CAMERA.position.y };
      const float angle = p_info->body->rot_scalar;
      const gfx_texture tex_ref = NAMESPACE_TASKING()->get_texture_data(info->tex_id_loc);
      gfx_texture tex = { tex_ref.id, tex_ref.width, tex_ref.height, tex_ref.angle };
      transform_letterbox_element_tex(LETTERBOX, &pos, &cam_pos, &tex);
      gfx_image_no_block(pos.x, pos.y, tex, cam_pos.x * info->parallax_value, cam_pos.y * info->parallax_value, CAMERA.zoom, true, info->flipped);
    }
  }
  return 0;
}

static void comp_render_constructor(ecs_ecs *ecs, const ecs_id entity_id, void *ptr, void *args) {
  comp_render *info = ptr;
  const comp_render *constructor_info = NAMESPACE_ELEMENT_INTERNAL.RENDER_INFO.init_info;
  if (info && constructor_info) {
    info->tex_id_loc = constructor_info->tex_id_loc;
    info->ovr_tile = constructor_info->ovr_tile;
    info->render_layer = constructor_info->render_layer;
    info->parallax_value = constructor_info->parallax_value;
    info->flipped = constructor_info->flipped;
  }
}

void comp_render_register() {
  NAMESPACE_ELEMENT_INTERNAL.RENDER_INFO.component_signature = ecs_register_component(NAMESPACE_ELEMENT_INTERNAL.ECS, sizeof(comp_render), comp_render_constructor, NULL);
}

void sys_render_register() {
  NAMESPACE_ELEMENT_INTERNAL.RENDER_INFO.system_signature = ecs_register_system(NAMESPACE_ELEMENT_INTERNAL.ECS, sys_render_update, NULL, NULL, NULL);
  ecs_require_component(NAMESPACE_ELEMENT_INTERNAL.ECS, NAMESPACE_ELEMENT_INTERNAL.RENDER_INFO.system_signature, NAMESPACE_ELEMENT_INTERNAL.RENDER_INFO.component_signature);
  ecs_require_component(NAMESPACE_ELEMENT_INTERNAL.ECS, NAMESPACE_ELEMENT_INTERNAL.RENDER_INFO.system_signature, NAMESPACE_ELEMENT_INTERNAL.PHYSICS_INFO.component_signature);
}

comp_render *sys_render_add(const ecs_id eid, comp_render *cr) {
  NAMESPACE_ELEMENT_INTERNAL.RENDER_INFO.init_info = cr;
  comp_render *res = ecs_add(NAMESPACE_ELEMENT_INTERNAL.ECS, eid, NAMESPACE_ELEMENT_INTERNAL.RENDER_INFO.component_signature, NULL);
  return res;
}

