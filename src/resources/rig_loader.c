#include "resources/rig_loader.h"
#include "khg_utl/config.h"
#include "resources/texture_loader.h"
#include "rig/rig.h"
#include <string.h>

rig_builder generate_rig_builder_from_file(const char *filepath, const char *section, const int num_anim) {
  utl_config_file *config = utl_config_create(filepath);
  rig_builder rb;
  rb.valid = true;
  rb.num_bones = utl_config_get_int(config, section, "num_bones", 1);
  rb.root_tex = get_tex_id_from_string((const char *)utl_config_get_value(config, section, "root_bone_tex"));
  rb.init_layer = utl_config_get_int(config, section, "root_bone_num", 0);
  rb.num_anim = num_anim;
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
      if (new_bone.bone_parent_num == -1) {
        continue;
      }
    }
    add_bone(r, phy_vector2_zero, 0.0f, new_bone.bone_tex, new_bone.bone_num, (bone *)utl_array_at(r->bones, new_bone.bone_parent_num));
  }
  utl_config_deallocate(config);
}

