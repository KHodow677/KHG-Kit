#include <string.h>
#define NAMESPACE_ELEMENT_IMPL

#define NAMESPACE_LOADING_USE

#include "element/namespace.h"
#include "khg_ecs/ecs.h"
#include "khg_gfx/elements.h"
#include "khg_gfx/texture.h"
#include "khg_phy/body.h"
#include "khg_phy/core/phy_vector.h"
#include "khg_phy/shape.h"
#include "loading/namespace.h"
#include "util/camera/camera.h"
#include "util/ovr_tile.h"
#include "util/letterbox.h"
#include <math.h>
#include <stdio.h>

static const gfx_aabb get_aabb_from_shape(phy_shape *shape) {
  phy_vector2 point1 = shape->polygon.vertices[0];
  phy_vector2 point2 = shape->polygon.vertices[2];
  return (gfx_aabb){ (point1.x + point2.x) / 2.0f, (point1.y + point2.y) / 2.0f, fabsf(point2.x - point1.x), fabsf(point2.y - point1.y) };
}

static ecs_ret sys_render_update(ecs_ecs *ecs, ecs_id *entities, const unsigned int entity_count, const ecs_dt dt, void *udata) {
  if (dt == 0.0f) {
    return 0;
  }
  for (unsigned int layer = 0; layer < 10; layer++) {
    for (int id = 0; id < entity_count; id++) {
      element_comp_render *info = ecs_get(NAMESPACE_ELEMENT_INTERNAL.ECS, entities[id], NAMESPACE_ELEMENT_INTERNAL.RENDER_INFO.component_signature);
      element_comp_physics *p_info = ecs_get(NAMESPACE_ELEMENT_INTERNAL.ECS, entities[id], NAMESPACE_ELEMENT_INTERNAL.PHYSICS_INFO.component_signature);
      if (info->ovr_tile.tile_id) {
        render_ovr_tile(&info->ovr_tile, &layer, dt);
        continue;
      }
      if (layer != info->render_layer) {
        continue;
      }
      phy_vector2 pos = phy_rigid_body_get_position(p_info->body);
      phy_vector2 cam_pos = phy_vector2_new(CAMERA.position.x, CAMERA.position.y);
      const float angle = phy_rigid_body_get_angle(p_info->body);
      const gfx_texture tex_ref = NAMESPACE_LOADING()->get_tex_def(info->tex_id);
      gfx_texture tex = { tex_ref.id, tex_ref.width, tex_ref.height, tex_ref.angle };
      transform_letterbox_element_tex(LETTERBOX, &pos, &cam_pos, &tex);
      gfx_image_no_block(pos.x, pos.y, tex, cam_pos.x * info->parallax_value, cam_pos.y * info->parallax_value, CAMERA.zoom, true, info->flipped);
    }
  }
  return 0;
}

static void comp_render_constructor(ecs_ecs *ecs, const ecs_id entity_id, void *ptr, void *args) {
  element_comp_render *info = ptr;
  const element_comp_render *constructor_info = NAMESPACE_ELEMENT_INTERNAL.RENDER_INFO.init_info;
  if (info && constructor_info) {
    strcpy(info->tex_id, constructor_info->tex_id);
    info->ovr_tile = constructor_info->ovr_tile;
    info->render_layer = constructor_info->render_layer;
    info->parallax_value = constructor_info->parallax_value;
    info->flipped = constructor_info->flipped;
  }
}

void comp_render_register() {
  NAMESPACE_ELEMENT_INTERNAL.RENDER_INFO.component_signature = ecs_register_component(NAMESPACE_ELEMENT_INTERNAL.ECS, sizeof(element_comp_render), comp_render_constructor, NULL);
}

void sys_render_register() {
  NAMESPACE_ELEMENT_INTERNAL.RENDER_INFO.system_signature = ecs_register_system(NAMESPACE_ELEMENT_INTERNAL.ECS, sys_render_update, NULL, NULL, NULL);
  ecs_require_component(NAMESPACE_ELEMENT_INTERNAL.ECS, NAMESPACE_ELEMENT_INTERNAL.RENDER_INFO.system_signature, NAMESPACE_ELEMENT_INTERNAL.RENDER_INFO.component_signature);
  ecs_require_component(NAMESPACE_ELEMENT_INTERNAL.ECS, NAMESPACE_ELEMENT_INTERNAL.RENDER_INFO.system_signature, NAMESPACE_ELEMENT_INTERNAL.PHYSICS_INFO.component_signature);
}

element_comp_render *sys_render_add(const ecs_id eid, element_comp_render *cr) {
  NAMESPACE_ELEMENT_INTERNAL.RENDER_INFO.init_info = cr;
  element_comp_render *res = ecs_add(NAMESPACE_ELEMENT_INTERNAL.ECS, eid, NAMESPACE_ELEMENT_INTERNAL.RENDER_INFO.component_signature, NULL);
  return res;
}

