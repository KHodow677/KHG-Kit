#pragma once

#include "khg_phy/body.h"
#include "khg_phy/core/phy_vector.h"
#include "khg_phy/shape.h"
#include "khg_utl/array.h"

struct bone;
typedef struct bone {
  phy_rigid_body *bone_body;
  phy_shape *bone_shape;
  phy_vector2 bone_offset;
  int bone_tex_id;
  int layer;
  struct bone *parent;
} bone;

typedef struct rig {
  bool enabled;
  size_t num_bones;
  utl_array *bones;
  phy_vector2 root_offset;
} rig;

typedef struct bone_info {
  int bone_tex;
  int bone_num;
  phy_vector2 bone_offset;
  int bone_parent_num;
} bone_info;

typedef struct rig_builder {
  bool valid;
  int num_bones;
  int root_tex;
  int init_layer;
  phy_vector2 root_offset;
} rig_builder;

rig_builder generate_rig_builder_from_file(const char *filepath, const char *section);
void generate_rig_from_file(rig *r, const char *filepath, const char *rig_section);
void generate_animation_from_path(const char *dir_path);

bone create_bone(const phy_vector2 bone_offset, const int tex_id, const int layer, bone *parent);
void add_bone(rig *r, const phy_vector2 bone_offset, const int tex_id, const int layer, bone *parent);

void create_rig(rig *r, const size_t num_bones, const phy_rigid_body *bone_body, const phy_vector2 root_offset, const int root_tex, const size_t init_layer);
void free_rig(const rig *r);
void render_rig(const rig *r, const float parallax_value, const bool flipped);

