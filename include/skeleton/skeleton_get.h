#pragma once

#include "khg_ani/Atlas.h"
#include "khg_ani/SkeletonJson.h"

spAtlas *get_anim_atlas(const char *asset_name);
spSkeletonData *get_anim_skeleton_data(const char *asset_name, spSkeletonJson *json);

