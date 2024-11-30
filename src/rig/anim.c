#include "rig/anim.h"
#include "khg_utl/array.h"
#include "khg_utl/easing.h"
#include "resources/texture_loader.h"
#include "rig/rig.h"
#include "khg_utl/config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

const bone_frame_info *get_bone_frame_info(const rig *r, const int state_id, const int frame_id) {
  return (bone_frame_info *)utl_array_at(*(utl_array **)utl_array_at(r->animation_states, state_id), frame_id);
}

const int last_frame_num(rig *r, const int state_id) {
  return utl_array_size(*(utl_array **)utl_array_at(r->animation_states, state_id)) - 1;
}

void generate_animation_frame(rig *r, const char *dir_path, const char *rig_section, const int anim_num, const int num_frames, const int frame_num) {
  char file[256];
  snprintf(file, 256, "%s%i.ini", dir_path, frame_num);
  if (access(file, F_OK) == -1) {
    return;
  }
  if (frame_num == 0) {
    utl_array *new_state = utl_array_create(sizeof(utl_array *), num_frames);
    utl_array_set(r->animation_states, anim_num, &new_state);
  }
  utl_array **frames = (utl_array **)utl_array_at(r->animation_states, anim_num);
  utl_array *new_frame = utl_array_create(sizeof(bone_frame_info), r->num_bones);
  utl_array_set(*frames, frame_num, &new_frame);
  bone_frame_info bone_frame;
  utl_config_file *config = utl_config_create(file);
  utl_config_iterator iterator = utl_config_get_iterator(config);
  const char *section, *key, *value;
  while (utl_config_next_entry(&iterator, &section, &key, &value)) {
    if (strcmp(section, rig_section)) {
      continue;
    }
    size_t len = strlen(key);
    char generic_key[len];
    strncpy(generic_key, key, len - 1);
    generic_key[len - 1] = '\0'; 
    if (!strcmp(generic_key, "bone_tex")) {
      bone_frame.bone_tex = get_tex_id_from_string((const char *)utl_config_get_value(config, section, key));
      continue;
    }
    else if (!strcmp(generic_key, "bone_offset")) {
      char **bone_offset = utl_config_get_array(config, section, key, 2);
      bone_frame.bone_offset = phy_vector2_new(atof(bone_offset[0]), atof(bone_offset[1]));
      free(bone_offset[0]);
      free(bone_offset[1]);
      free(bone_offset);
      continue;
    }
    else if (!strcmp(generic_key, "bone_angle_offset")) {
      bone_frame.bone_angle_offset = atof((char *)utl_config_get_value(config, section, key));
      continue;
    }
    else if (!strcmp(generic_key, "bone_num")) {
      int bone_num = utl_config_get_int(config, section, key, 0);
      utl_array_set(new_frame, bone_num, &bone_frame);
    }
  }
  utl_config_deallocate(config);
}

void generate_animation_from_path(rig *r, const char *dir_path, const char *section, const int anim_num, const int num_frames) {
  for (int i = 0; i < num_frames; i++) {
    generate_animation_frame(r, dir_path, section, anim_num, num_frames, i);
  }
}

void update_rig_with_interpolated_frame(bone *b, const bone_frame_info *current, const bone_frame_info *target, const float frame_percentage) {
  float x_diff = target->bone_offset.x - current->bone_offset.x;
  float y_diff = target->bone_offset.y - current->bone_offset.y;
  float ang_diff = target->bone_angle_offset - current->bone_angle_offset;
  b->bone_tex_id = current->bone_tex;
  b->bone_offset.x = current->bone_offset.x + x_diff * utl_easing_linear_interpolation(frame_percentage);
  b->bone_offset.y = current->bone_offset.y + y_diff * utl_easing_linear_interpolation(frame_percentage);
  b->bone_angle_offset = current->bone_angle_offset + ang_diff * utl_easing_linear_interpolation(frame_percentage);
  phy_rigid_body_set_position(b->bone_body, phy_vector2_add(phy_rigid_body_get_position(b->parent->bone_body), b->bone_offset));
  phy_rigid_body_set_angle(b->bone_body, phy_rigid_body_get_angle(b->parent->bone_body) + b->bone_angle_offset);
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

