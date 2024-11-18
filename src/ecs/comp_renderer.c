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
#include "khg_phy/core/phy_vector.h"
#include "rig/rig.h"

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
      if (info->rig.enabled) {
        render_rig(&info->rig, info->parallax_value, info->flipped);
      }
      else {
        phy_vector2 pos = phy_vector2_add(phy_rigid_body_get_position(info->body), info->offset);
        phy_vector2 cam_pos = phy_vector2_new(CAMERA.position.x, CAMERA.position.y);
        const float angle = phy_rigid_body_get_angle(info->body);
        const gfx_texture tex_ref = get_or_add_texture(info->tex_id);
        gfx_texture tex = { tex_ref.id, tex_ref.width, tex_ref.height, tex_ref.angle };
        transform_letterbox_element(LETTERBOX, &pos, &cam_pos, &tex);
        gfx_image_no_block(pos.x, pos.y, tex, 0.0f, 0.0f, cam_pos.x * info->parallax_value, cam_pos.y * info->parallax_value, CAMERA.zoom, true, info->flipped);
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
    info->rig = (rig){ false, 0, NULL };
    info->tex_id = constructor_info->tex_id;
    info->render_layer = constructor_info->render_layer;
    info->parallax_value = constructor_info->parallax_value;
    info->flipped = constructor_info->flipped;
    info->offset = phy_vector2_new(0.0f, 0.0f);
  }
}

static void comp_renderer_destructor(ecs_ecs *ecs, const ecs_id entity_id, void *ptr) {
  const comp_renderer *info = ptr;
  if (info && info->rig.enabled) {
    free_rig(&info->rig);
  }
}

const bool current_tex_in_range(const comp_renderer *cr, const int min, const int max) {
  return (cr->tex_id >= min && cr->tex_id <= max);
}

void comp_renderer_register() {
  RENDERER_COMPONENT_SIGNATURE = ecs_register_component(ECS, sizeof(comp_renderer), comp_renderer_constructor, comp_renderer_destructor);
}

void sys_renderer_register() {
  RENDERER_SYSTEM_SIGNATURE = ecs_register_system(ECS, sys_renderer_update, NULL, NULL, NULL);
  ecs_require_component(ECS, RENDERER_SYSTEM_SIGNATURE, RENDERER_COMPONENT_SIGNATURE);
  ecs_require_component(ECS, RENDERER_SYSTEM_SIGNATURE, PHYSICS_COMPONENT_SIGNATURE);
}

comp_renderer *sys_renderer_add(const ecs_id eid, comp_renderer_constructor_info *crci, const rig_builder* rb) {
  RENDERER_CONSTRUCTOR_INFO = crci;
  comp_renderer *res = ecs_add(ECS, eid, RENDERER_COMPONENT_SIGNATURE, NULL);
  if (rb) {
    create_rig(&res->rig, rb->num_bones, res->body, rb->root_tex, rb->init_layer);
  }
  return res;
}

