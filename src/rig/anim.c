#include "rig/anim.h"
#include "rig/rig.h"
#include "khg_phy/body.h"
#include "khg_utl/array.h"
#include "khg_utl/easing.h"
#include <unistd.h>

const bone_frame_info *get_bone_frame_info(const rig *r, const int state_id, const int frame_id) {
  return (bone_frame_info *)utl_array_at(*(utl_array **)utl_array_at(r->animation_states, state_id), frame_id);
}

const int last_frame_num(rig *r, const int state_id) {
  return utl_array_size(*(utl_array **)utl_array_at(r->animation_states, state_id)) - 1;
}

void update_rig_with_interpolated_frame(bone *b, const bone_frame_info *current, const bone_frame_info *target, const float frame_percentage, const phy_rigid_body *root_body) {
  float x_diff = target->bone_offset.x - current->bone_offset.x;
  float y_diff = target->bone_offset.y - current->bone_offset.y;
  float ang_diff = target->bone_angle_offset - current->bone_angle_offset;
  b->bone_tex_id = current->bone_tex;
  b->bone_offset.x = current->bone_offset.x + x_diff * utl_easing_linear_interpolation(frame_percentage);
  b->bone_offset.y = current->bone_offset.y + y_diff * utl_easing_linear_interpolation(frame_percentage);
  b->bone_angle_offset = current->bone_angle_offset + ang_diff * utl_easing_linear_interpolation(frame_percentage);
  if (!root_body) {
    phy_rigid_body_set_position(b->bone_body, phy_vector2_add(phy_rigid_body_get_position(b->parent->bone_body), b->bone_offset));
    phy_rigid_body_set_angle(b->bone_body, phy_rigid_body_get_angle(b->parent->bone_body) + b->bone_angle_offset);
  }
  else {
    phy_rigid_body_set_position(b->bone_body, phy_vector2_add(phy_rigid_body_get_position(root_body), b->bone_offset));
    phy_rigid_body_set_angle(b->bone_body, phy_rigid_body_get_angle(root_body) + b->bone_angle_offset);
  }
  b->updated = true;
}

void set_state_and_frame(rig *r, const int state_id, const int frame_id) {
  r->current_state_id = state_id;
  r->current_frame_id = frame_id;
  r->current_frame_bones = *(utl_array **)utl_array_at(*(utl_array **)utl_array_at(r->animation_states, r->current_state_id), r->current_frame_id);
}

utl_array *get_frame(const rig *r, const int state_id, const int frame_id) {
  return *(utl_array **)utl_array_at(*(utl_array **)utl_array_at(r->animation_states, state_id), frame_id);
}

