#include "rig/rig.h"
#include "camera/camera.h"
#include "letterbox.h"
#include "physics/physics.h"
#include "resources/texture_loader.h"
#include "rig/anim.h"
#include "khg_phy/body.h"
#include "khg_phy/space.h"
#include "khg_phy/core/phy_vector.h"
#include "khg_utl/array.h"
#include <string.h>
#include <unistd.h>

bone create_bone(const phy_vector2 bone_offset, const float bone_angle_offset, const int tex_id, const int layer, bone *parent) {
  phy_rigid_body_initializer bone_init = phy_rigid_body_initializer_default;
  bone_init.type = PHY_RIGID_BODY_TYPE_DYNAMIC;
  bone_init.position = parent ? phy_vector2_add(phy_rigid_body_get_position(parent->bone_body), bone_offset) : bone_offset;
  bone_init.angle = 0.0f;
  bone res = { false, phy_rigid_body_new(bone_init), phy_circle_shape_new(bone_offset, 0.0f), bone_offset, bone_angle_offset, tex_id, layer, parent };
  phy_rigid_body_disable_collisions(res.bone_body);
  phy_rigid_body_set_mass(res.bone_body, 1.0f);
  phy_rigid_body_add_shape(res.bone_body, res.bone_shape);
  phy_space_add_rigidbody(SPACE, res.bone_body);
  return res;
}

void add_bone(rig *r, const phy_vector2 bone_offset, const float bone_angle_offset, const int tex_id, const int layer, bone *parent) {
  bone b = create_bone(bone_offset, bone_angle_offset, tex_id, layer, parent);
  utl_array_set(r->bones, layer, &b);
}

void create_rig(rig *r, const rig_builder *rb) {
  r->enabled = true;
  r->num_bones = rb->num_bones;
  r->bones = utl_array_create(sizeof(bone), rb->num_bones);
  r->root_id = rb->init_layer;
  r->animation_states = utl_array_create(sizeof(utl_array *), rb->num_anim);
  bone root_bone = create_bone(phy_vector2_zero, 0.0f, rb->root_tex, rb->init_layer, NULL);
  utl_array_set(r->bones, rb->init_layer, &root_bone);
}

void free_rig(const rig *r) {
  for (bone *b = utl_array_begin(r->bones); b != (bone *)utl_array_end(r->bones); b++) {
    phy_space_remove_rigidbody(SPACE, b->bone_body);
    phy_rigid_body_free(b->bone_body);
  }
  if (utl_array_size(r->animation_states) > 0) {
    for (utl_array **state = utl_array_begin(r->animation_states); state != (utl_array **)utl_array_end(r->animation_states); state++) {
      if (state && *state) {
        for (utl_array **frame = utl_array_begin(*state); frame != (utl_array **)utl_array_end(*state); frame++) {
          if (frame && *frame) {
            utl_array_deallocate(*frame);
          }
        }
        utl_array_deallocate(*state);
      }
    }
  }
  utl_array_deallocate(r->animation_states);
  utl_array_deallocate(r->bones);
}

void update_rig(const rig *r, const phy_rigid_body *body, const float frame_percentage, utl_array *target, const bool flipped) {
  for (bone *b = utl_array_begin(r->bones); b != (bone *)utl_array_end(r->bones); b++) {
    b->updated = false;
  }
  bool updated = false;
  while (!updated) {
    for (int i = 0; i < r->num_bones; i++) {
      bone *b = utl_array_at(r->bones, i);
      const phy_rigid_body *rb = (!b->parent) ? body : NULL;
      update_rig_with_interpolated_frame(b, utl_array_at(r->current_frame_bones, i), utl_array_at(target, i), frame_percentage, rb, flipped);
    }
    updated = true;
    for (bone *b = utl_array_begin(r->bones); b != (bone *)utl_array_end(r->bones); b++) {
      if (!b->updated) {
        updated = false;
        break;
      }
    }
  }
};

void render_rig(const rig *r, const float parallax_value, const bool flipped) {
  for (bone *b = utl_array_begin(r->bones); b != (bone *)utl_array_end(r->bones); b++) {
    const float angle = phy_rigid_body_get_angle(b->bone_body);
    phy_vector2 pos = phy_rigid_body_get_position(b->bone_body);
    phy_vector2 cam_pos = phy_vector2_new(CAMERA.position.x, CAMERA.position.y);
    const gfx_texture tex_ref = get_or_add_texture(b->bone_tex_id);
    gfx_texture tex = { tex_ref.id, tex_ref.width, tex_ref.height, angle };
    transform_letterbox_element_tex(LETTERBOX, &pos, &cam_pos, &tex);
    gfx_image_no_block(pos.x, pos.y, tex, cam_pos.x * parallax_value, cam_pos.y * parallax_value, CAMERA.zoom, true, flipped);
  }
}

