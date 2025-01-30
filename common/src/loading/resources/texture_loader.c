#define NAMESPACE_LOADING_IMPL

#include "khg_utl/config.h"
#include "khg_utl/string.h"
#include "khg_utl/vector.h"
#include "khg_gfx/texture.h"
#include "khg_utl/algorithm.h"
#include "loading/namespace.h"
#include <stdlib.h>
#include <string.h>

static utl_vector *TEXTURE_NAMES;
static utl_vector *TEXTURE_ASSETS;
static utl_vector *TEXTURE_RAWS;
static utl_vector *TEXTURES;

static int compare_texture_strings(const void *a, const void *b) {
  return strcmp(*(const char **)a, (const char *)b);
}

void generate_tex_defs(const char *filename) {
  TEXTURE_NAMES = utl_vector_create(sizeof(char *));
  TEXTURE_ASSETS = utl_vector_create(sizeof(loading_texture_asset));
  TEXTURE_RAWS = utl_vector_create(sizeof(loading_texture_raw_info));
  TEXTURES = utl_vector_create(sizeof(gfx_texture));
  utl_config_file *config = utl_config_create(filename);
  utl_config_iterator iterator = utl_config_get_iterator(config);
  const char *section, *key, *value;
  char last_section[128] = "";
  while (utl_config_next_entry(&iterator, &section, &key, &value)) {
    if (!strcmp(last_section, section)) {
      strcpy(last_section, section);
      continue;
    };
    strcpy(last_section, section);
    char *tex_str = utl_string_strdup(section);
    const char *path = utl_config_get_value(config, section, "path");
    const int width = utl_config_get_int(config, section, "width", 512);
    const int height = utl_config_get_int(config, section, "height", 512);
    loading_texture_asset tex_asset = { .tex_width = width, .tex_height = height };
    strcpy(tex_asset.tex_filepath, path);
    NAMESPACE_LOADING_INTERNAL.TEXTURE_ASSET_THREAD.progress++;
    utl_vector_push_back(TEXTURE_NAMES, &tex_str);
    utl_vector_push_back(TEXTURE_ASSETS, &tex_asset);
  }
  utl_config_deallocate(config);
  NAMESPACE_LOADING_INTERNAL.TEXTURE_ASSET_THREAD.max = utl_vector_size(TEXTURE_NAMES);
  NAMESPACE_LOADING_INTERNAL.TEXTURE_RAW_THREAD.max = utl_vector_size(TEXTURE_NAMES);
  NAMESPACE_LOADING_INTERNAL.TEXTURE_THREAD.max = utl_vector_size(TEXTURE_NAMES);
}

void emplace_tex_defs_tick(void *arg) {
  loading_resource_thread *thread = arg;
  const unsigned int num_names = utl_vector_size(TEXTURE_NAMES);
  if (thread->progress < num_names) {
    char *tex_str = utl_vector_at(TEXTURE_NAMES, thread->progress);
    loading_texture_asset tex_asset = *(loading_texture_asset *)utl_vector_at(TEXTURE_ASSETS, thread->progress);
    int width, height, channels;
    unsigned char *tex_raw;
    gfx_fetch_texture_raw(&tex_raw, tex_asset.tex_filepath, &width, &height, &channels);
    loading_texture_raw_info tex_raw_info = { tex_raw, tex_asset.tex_width, tex_asset.tex_height, channels };
    NAMESPACE_LOADING_INTERNAL.TEXTURE_RAW_THREAD.progress++;
    utl_vector_push_back(TEXTURE_RAWS, &tex_raw_info);
  }
}

int emplace_tex_defs(void *arg) {
  loading_resource_thread *thread = arg;
  const unsigned int num_names = utl_vector_size(TEXTURE_NAMES);
  while (thread->progress < num_names) {
    loading_texture_raw_info *tex_raw_info = utl_vector_at(TEXTURE_RAWS, NAMESPACE_LOADING_INTERNAL.TEXTURE_THREAD.progress);
    gfx_texture tex = gfx_load_texture_asset_raw(tex_raw_info->tex_raw, tex_raw_info->width, tex_raw_info->height, tex_raw_info->channels);
    tex.width = tex_raw_info->width;
    tex.height = tex_raw_info->height;
    tex.angle = 0;
    gfx_free_texture_raw(tex_raw_info->tex_raw);
    NAMESPACE_LOADING_INTERNAL.TEXTURE_THREAD.progress++;
    utl_vector_push_back(TEXTURES, &tex);
  }
  return 0;
}

gfx_texture get_tex_def_by_location(unsigned int loc) {
  gfx_texture *tex = utl_vector_at(TEXTURES, loc);
  if (!tex) {
    return (gfx_texture){ 0 };
  }
  return *tex;
}

gfx_texture get_tex_def(char *tex_str) {
  unsigned int loc = utl_algorithm_find_at(utl_vector_data(TEXTURE_NAMES), utl_vector_size(TEXTURE_NAMES), sizeof(char *), tex_str, compare_texture_strings);
  return get_tex_def_by_location(loc);
}

unsigned int get_location_tex_str(const char *tex_str) {
  return utl_algorithm_find_at(utl_vector_data(TEXTURE_NAMES), utl_vector_size(TEXTURE_NAMES), sizeof(char *), tex_str, compare_texture_strings);
}

void free_tex_defs() {
  for (char **tex_name = utl_vector_begin(TEXTURE_NAMES); tex_name != utl_vector_end(TEXTURE_NAMES); tex_name++) {
    free(*tex_name);
  }
  utl_vector_deallocate(TEXTURE_NAMES);
  utl_vector_deallocate(TEXTURE_ASSETS);
  utl_vector_deallocate(TEXTURE_RAWS);
  utl_vector_deallocate(TEXTURES);
}
