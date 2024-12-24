#include "resources/anim_loader.h"
#include "resources/texture_loader.h"
#include "rig/anim.h"
#include "rig/rig.h"
#include "khg_utl/config.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

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
    else if (!strcmp(generic_key, "bone_scale")) {
      char **bone_scale = utl_config_get_array(config, section, key, 2);
      bone_frame.bone_scale = phy_vector2_new(atof(bone_scale[0]), atof(bone_scale[1]));
      free(bone_scale[0]);
      free(bone_scale[1]);
      free(bone_scale);
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

