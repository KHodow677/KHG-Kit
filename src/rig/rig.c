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
#include "khg_utl/config.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>

rig_builder generate_rig_builder_from_file(const char *filepath, const char *section, const int num_anim) {
  utl_config_file *config = utl_config_create(filepath);
  rig_builder rb;
  rb.valid = true;
  rb.num_bones = utl_config_get_int(config, section, "num_bones", 1);
  rb.root_tex = get_tex_id_from_string((const char *)utl_config_get_value(config, section, "root_bone_tex"));
  rb.init_layer = utl_config_get_int(config, section, "root_bone_num", 0);
  char **root_offset = utl_config_get_array(config, section, "root_bone_offset", 2);
  rb.root_offset = phy_vector2_new(atof(root_offset[0]), atof(root_offset[1]));
  rb.root_angle_offset = atof(utl_config_get_value(config, section, "root_bone_angle_offset"));
  rb.num_anim = num_anim;
  free(root_offset[0]);
  free(root_offset[1]);
  free(root_offset);
  utl_config_deallocate(config);
  return rb;
}

void generate_rig_from_file(rig *r, const char *filepath, const char *rig_section) {
  utl_config_file *config = utl_config_create(filepath);
  utl_config_iterator iterator = utl_config_get_iterator(config);
  const char *section, *key, *value;
  bone_info new_bone;
  while (utl_config_next_entry(&iterator, &section, &key, &value)) {
    if (strcmp(section, rig_section)) {
      continue;
    }
    size_t len = strlen(key);
    char generic_key[len];
    strncpy(generic_key, key, len - 1);
    generic_key[len - 1] = '\0'; 
    if (!strcmp(generic_key, "bone_tex")) {
      new_bone.bone_tex = get_tex_id_from_string((const char *)utl_config_get_value(config, section, key));
      continue;
    }
    else if (!strcmp(generic_key, "bone_num")) {
      new_bone.bone_num = utl_config_get_int(config, section, key, 0);
      continue;
    }
    else if (!strcmp(generic_key, "bone_parent_num")) {
      new_bone.bone_parent_num = utl_config_get_int(config, section, key, 0);
    }
    add_bone(r, phy_vector2_zero, 0.0f, new_bone.bone_tex, new_bone.bone_num, (bone *)utl_array_at(r->bones, new_bone.bone_parent_num));
  }
  utl_config_deallocate(config);
}

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

void create_rig(rig *r, const size_t num_bones, const phy_rigid_body *bone_body, const phy_vector2 root_offset, const int root_tex, const size_t init_layer, const int num_anim) {
  r->enabled = true;
  r->num_bones = num_bones;
  r->bones = utl_array_create(sizeof(bone), num_bones);
  r->root_offset = root_offset;
  r->root_id = init_layer;
  r->animation_states = utl_array_create(sizeof(utl_array *), num_anim);
  bone root_bone = create_bone(phy_vector2_add(phy_rigid_body_get_position(bone_body), root_offset), 0.0f, root_tex, init_layer, NULL);
  utl_array_set(r->bones, init_layer, &root_bone);
}

void free_rig(const rig *r) {
  for (bone *b = utl_array_begin(r->bones); b != (bone *)utl_array_end(r->bones); b++) {
    phy_space_remove_rigidbody(SPACE, b->bone_body);
    phy_rigid_body_free(b->bone_body);
  }
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
  utl_array_deallocate(r->animation_states);
  utl_array_deallocate(r->bones);
}

void update_rig(const rig *r, const phy_rigid_body *body, const float frame_percentage, utl_array *target) {
  for (bone *b = utl_array_begin(r->bones); b != (bone *)utl_array_end(r->bones); b++) {
    b->updated = false;
  }
  bool updated = false;
  while (!updated) {
    for (int i = 0; i < r->num_bones; i++) {
      bone *b = utl_array_at(r->bones, i);
      if (i == r->root_id && !b->updated) {
        b->bone_offset = r->root_offset;
        phy_rigid_body_set_position(b->bone_body, phy_vector2_add(phy_rigid_body_get_position(body), r->root_offset));
        phy_rigid_body_set_angle(b->bone_body, phy_rigid_body_get_angle(body) + r->root_angle_offset);
        b->updated = true;
        break;
      }
      if (!b->parent || !b->parent->updated) {
        continue;
      }
      bone_frame_info *bfi = utl_array_at(r->current_frame_bones, i);
      bone_frame_info *tgt = utl_array_at(target, i);
      update_rig_with_interpolated_frame(b, bfi, tgt, frame_percentage);
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
    phy_vector2 pos = phy_vector2_add(phy_rigid_body_get_position(b->bone_body), b->bone_offset);
    phy_vector2 cam_pos = phy_vector2_new(CAMERA.position.x, CAMERA.position.y);
    const gfx_texture tex_ref = get_or_add_texture(b->bone_tex_id);
    gfx_texture tex = { tex_ref.id, tex_ref.width, tex_ref.height, angle };
    transform_letterbox_element(LETTERBOX, &pos, &cam_pos, &tex);
    gfx_image_no_block(pos.x, pos.y, tex, 0.0f, 0.0f, cam_pos.x * parallax_value, cam_pos.y * parallax_value, CAMERA.zoom, true, flipped);
  }
}

