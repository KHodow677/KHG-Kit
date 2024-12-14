#include "khg_utl/algorithm.h"
#include "khg_utl/config.h"
#include "khg_utl/string.h"
#include "resources/rig_loader.h"
#include "resources/texture_loader.h"
#include "rig/rig.h"
#include <string.h>

static rig_asset RIG_ASSET_REF[NUM_RIGS];

static int compare_rig_strings(const void *a, const void *b) {
  return strcmp(*(const char **)a, (const char *)b);
}

const rig generate_rig(const char *filepath, const char *rb_section, const char *rig_section, const size_t num_anim) {
  rig_builder rb = generate_rig_builder_from_file(filepath, rb_section, num_anim);
  rig r;
  create_rig(&r, &rb);
  generate_rig_from_file(&r, filepath, rig_section);
  return r;
}

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
    utl_string *key_obj = utl_string_create(key);
    if (utl_string_starts_with(key_obj, "bone_tex")) {
      new_bone.bone_tex = get_tex_id_from_string((const char *)utl_config_get_value(config, section, key));
      utl_string_deallocate(key_obj);
      continue;
    }
    else if (utl_string_starts_with(key_obj, "bone_num")) {
      new_bone.bone_num = utl_config_get_int(config, section, key, 0);
      utl_string_deallocate(key_obj);
      continue;
    }
    else if (utl_string_starts_with(key_obj, "bone_parent_num")) {
      new_bone.bone_parent_num = utl_config_get_int(config, section, key, 0);
      utl_string_deallocate(key_obj);
      if (new_bone.bone_parent_num == -1) {
        continue;
      }
    }
    add_bone(r, phy_vector2_zero, 0.0f, new_bone.bone_tex, new_bone.bone_num, (bone *)utl_array_at(r->bones, new_bone.bone_parent_num));
  }
  utl_config_deallocate(config);
}

const size_t get_rig_id_from_string(const char *rig_key) {
  return (size_t)utl_algorithm_find_at(RIG_STRINGS, RIG_STRINGS_SIZE, sizeof(char *), rig_key, compare_rig_strings);
}

const rig get_rig(size_t rig_id) {
  const rig_asset ra = RIG_ASSET_REF[rig_id];
  return generate_rig(ra.rig_filepath, ra.rb_section, ra.rig_section, ra.num_anim);
}

const rig get_rig_from_string(const char *rig_key) {
  const int area_id = get_rig_id_from_string(rig_key);
  return get_rig(area_id);
}

void generate_rigs() {
  RIG_ASSET_REF[PLAYER_RIG] = (rig_asset){ "res/assets/data/anim/rigs/player.ini", "player_root", "player_bones", 1 };
}

