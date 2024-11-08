#include "skeleton/skeleton_get.h"
#include "khg_ani/Atlas.h"
#include "khg_ani/SkeletonData.h"
#include "khg_ani/SkeletonJson.h"
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

spAtlas *get_anim_atlas(const char *asset_name) {
#if defined(_WIN32) || defined(_WIN64)
  char cwd[PATH_MAX];
  _getcwd(cwd, sizeof(cwd));
  size_t cwd_len = strlen(cwd);
  size_t asset_dir_len = cwd_len + strlen("\\res") + 1;
  size_t path_len = asset_dir_len + strlen("\\assets\\textures\\") + strlen(asset_name) + strlen(".atlas") + 1;
  char asset_dir[asset_dir_len];
  char path[path_len];
  snprintf(asset_dir, sizeof(asset_dir), "%s\\res", cwd);
  snprintf(path, sizeof(path), "%s\\assets\\anim\\%s.atlas", asset_dir, asset_name);
  return spAtlas_createFromFile(path, 0);
#else
  char cwd[PATH_MAX];
  getcwd(cwd, sizeof(cwd));
  size_t cwd_len = strlen(cwd);
  size_t asset_dir_len = cwd_len + strlen("/res") + 1;
  size_t path_len = asset_dir_len + strlen("/assets/anim/") + strlen(asset_name) + strlen(".atlas") + 1;
  char asset_dir[asset_dir_len];
  char path[path_len];
  snprintf(asset_dir, sizeof(asset_dir), "%s/res", cwd);
  snprintf(path, sizeof(path), "%s/assets/anim/%s.atlas", asset_dir, asset_name);
  return spAtlas_createFromFile(path, 0);
#endif
}

spSkeletonData *get_anim_skeleton_data(const char *asset_name, spSkeletonJson *json) {
#if defined(_WIN32) || defined(_WIN64)
  char cwd[PATH_MAX];
  _getcwd(cwd, sizeof(cwd));
  size_t cwd_len = strlen(cwd);
  size_t asset_dir_len = cwd_len + strlen("\\res") + 1;
  size_t path_len = asset_dir_len + strlen("\\assets\\textures\\") + strlen(asset_name) + strlen(".atlas") + 1;
  char asset_dir[asset_dir_len];
  char path[path_len];
  snprintf(asset_dir, sizeof(asset_dir), "%s\\res", cwd);
  snprintf(path, sizeof(path), "%s\\assets\\anim\\%s.atlas", asset_dir, asset_name);
  return spAtlas_createFromFile(path, 0);
#else
  char cwd[PATH_MAX];
  getcwd(cwd, sizeof(cwd));
  size_t cwd_len = strlen(cwd);
  size_t asset_dir_len = cwd_len + strlen("/res") + 1;
  size_t path_len = asset_dir_len + strlen("/assets/anim/") + strlen(asset_name) + strlen(".json") + 1;
  char asset_dir[asset_dir_len];
  char path[path_len];
  snprintf(asset_dir, sizeof(asset_dir), "%s/res", cwd);
  snprintf(path, sizeof(path), "%s/assets/anim/%s.json", asset_dir, asset_name);
  return spSkeletonJson_readSkeletonDataFile(json, path);
#endif
}

