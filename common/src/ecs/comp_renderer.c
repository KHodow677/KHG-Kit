#include "area/ovr_tile.h"
#include "camera/camera.h"
#include "ecs/comp_physics.h"
#include "ecs/comp_renderer.h"
#include "ecs/ecs_manager.h"
#include "khg_ecs/ecs.h"
#include "khg_gfx/elements.h"
#include "khg_gfx/texture.h"
#include "khg_gfx/ui.h"
#include "khg_phy/body.h"
#include "khg_phy/core/phy_vector.h"
#include "khg_phy/shape.h"
#include "letterbox.h"
#include "resources/rig_loader.h"
#include "resources/texture_loader.h"
#include "rig/rig.h"
#include <math.h>
#include <stdio.h>

ecs_id RENDERER_COMPONENT_SIGNATURE;
ecs_id RENDERER_SYSTEM_SIGNATURE;

comp_renderer_constructor_info *RENDERER_CONSTRUCTOR_INFO = NULL;

static const gfx_aabb get_aabb_from_shape(phy_shape *shape) {
  phy_vector2 point1 = shape->polygon.vertices[0];
  phy_vector2 point2 = shape->polygon.vertices[2];
  return (gfx_aabb){ (point1.x + point2.x) / 2.0f, (point1.y + point2.y) / 2.0f, fabsf(point2.x - point1.x), fabsf(point2.y - point1.y) };
}

static ecs_ret sys_renderer_update(ecs_ecs *ecs, ecs_id *entities, const unsigned int entity_count, const ecs_dt dt, void *udata) {
  if (dt == 0.0f) {
    return 0;
  }
  for (int layer = 0; layer < 10; layer++) {
    for (int id = 0; id < entity_count; id++) {
      comp_renderer *info = ecs_get(ECS, entities[id], RENDERER_COMPONENT_SIGNATURE);
      if (layer != info->render_layer) {
        continue;
      }
      if (info->show_shape) {
        phy_vector2 pos = phy_vector2_add(phy_rigid_body_get_position(info->body), info->offset);
        phy_vector2 cam_pos = phy_vector2_new(CAMERA.position.x, CAMERA.position.y);
        const float angle = phy_rigid_body_get_angle(info->body);
        gfx_aabb aabb = get_aabb_from_shape(info->shape);
        transform_letterbox_element_aabb(LETTERBOX, &pos, &cam_pos, &aabb);
        gfx_rect_no_block(pos.x, pos.y, aabb.size.x, aabb.size.y, GFX_GREEN, 0.0, angle);
      }
      if (info->rig.enabled) {
        render_rig(&info->rig, info->parallax_value, info->flipped);
      }
      else if (info->ovr_tile.id) {
        phy_vector2 pos = phy_vector2_add(phy_rigid_body_get_position(info->body), info->offset);
        render_ovr_tile(info->ovr_tile.id, info->ovr_tile.pos);
      }
      else {
        phy_vector2 pos = phy_vector2_add(phy_rigid_body_get_position(info->body), info->offset);
        phy_vector2 cam_pos = phy_vector2_new(CAMERA.position.x, CAMERA.position.y);
        const float angle = phy_rigid_body_get_angle(info->body);
        const gfx_texture tex_ref = get_or_add_texture(info->tex_id);
        gfx_texture tex = { tex_ref.id, tex_ref.width, tex_ref.height, tex_ref.angle };
        transform_letterbox_element_tex(LETTERBOX, &pos, &cam_pos, &tex);
        gfx_image_no_block(pos.x, pos.y, tex, cam_pos.x * info->parallax_value, cam_pos.y * info->parallax_value, CAMERA.zoom, true, info->flipped);
      }
    }
  }
  return 0;
}

static void comp_renderer_constructor(ecs_ecs *ecs, const ecs_id entity_id, void *ptr, void *args) {
  comp_renderer *info = ptr;
  const comp_renderer_constructor_info *constructor_info = RENDERER_CONSTRUCTOR_INFO;
  if (info && constructor_info) {
    info->body = constructor_info->body;
    info->shape = constructor_info->shape;
    info->rig = constructor_info->rig_id != NULL_RIG ? get_rig(constructor_info->rig_id) : (rig){ false };
    info->ovr_tile = constructor_info->ovr_tile;
    info->tex_id = constructor_info->tex_id;
    info->render_layer = constructor_info->render_layer;
    info->parallax_value = constructor_info->parallax_value;
    info->flipped = constructor_info->flipped;
    info->show_shape = constructor_info->show_shape;
    info->offset = phy_vector2_new(0.0f, 0.0f);
  }
}

static void comp_renderer_destructor(ecs_ecs *ecs, const ecs_id entity_id, void *ptr) {
  const comp_renderer *info = ptr;
  if (info && info->rig.enabled) {
    free_rig(&info->rig);
  }
}

void comp_renderer_register() {
  RENDERER_COMPONENT_SIGNATURE = ecs_register_component(ECS, sizeof(comp_renderer), comp_renderer_constructor, comp_renderer_destructor);
}

void sys_renderer_register() {
  RENDERER_SYSTEM_SIGNATURE = ecs_register_system(ECS, sys_renderer_update, NULL, NULL, NULL);
  ecs_require_component(ECS, RENDERER_SYSTEM_SIGNATURE, RENDERER_COMPONENT_SIGNATURE);
  ecs_require_component(ECS, RENDERER_SYSTEM_SIGNATURE, PHYSICS_COMPONENT_SIGNATURE);
}

comp_renderer *sys_renderer_add(const ecs_id eid, comp_renderer_constructor_info *crci) {
  RENDERER_CONSTRUCTOR_INFO = crci;
  comp_renderer *res = ecs_add(ECS, eid, RENDERER_COMPONENT_SIGNATURE, NULL);
  return res;
}
