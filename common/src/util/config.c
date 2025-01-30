#include "khg_utl/config.h"
#include "khg_gfx/texture.h"
#include "khg_utl/map.h"
#include "khg_utl/string.h"
#include "util/config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

utl_map *TEXTURE_ASSETS = NULL;
utl_map *TILE_ASSETS = NULL;

static int compare_config_strings(const utl_key_type a, const utl_key_type b) {
  return strcmp((const char *)a, (const char *)b);
}

static const gfx_texture generate_texture_raw(config_texture_raw_info raw_info) {
  gfx_texture tex = gfx_load_texture_asset_raw(raw_info.tex_raw, raw_info.width, raw_info.height, raw_info.channels);
  tex.width = raw_info.width;
  tex.height = raw_info.height;
  tex.angle = 0;
  return tex;
}

void generate_tex_defs(const char *filename) {
  TEXTURE_ASSETS = utl_map_create(compare_config_strings, free, free);
  utl_config_file *config = utl_config_create(filename);
  utl_config_iterator iterator = utl_config_get_iterator(config);
  const char *section, *key, *value;
  char last_section[128];
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
    util_texture_asset *tex_asset = malloc(sizeof(util_texture_asset));
    strcpy(tex_asset->tex_filepath, path);
    tex_asset->tex_width = width;
    tex_asset->tex_height = height;
    utl_map_insert(TEXTURE_ASSETS, tex_str, tex_asset);
  }
  utl_config_deallocate(config);
}

void emplace_tex_defs_tick(utl_map_iterator *map_it) {
  if (!map_it || map_it->node == utl_map_end(TEXTURE_ASSETS).node) {
    return;
  }
  char *tex_str = utl_map_node_get_key(map_it->node);
  util_texture_asset tex_asset = *(util_texture_asset *)utl_map_node_get_value(map_it->node);
  int width, height, channels;
  unsigned char *tex_raw;
  gfx_fetch_texture_raw(&tex_raw, tex_asset.tex_filepath, &width, &height, &channels);
  free(map_it->node->value);
  config_texture_raw_info *tex_raw_info = malloc(sizeof(config_texture_raw_info));
  tex_raw_info->tex_raw = tex_raw;
  tex_raw_info->width = tex_asset.tex_width;
  tex_raw_info->height = tex_asset.tex_height;
  tex_raw_info->channels = channels;
  map_it->node->value = tex_raw_info;
  utl_map_iterator_increment(map_it);
}

void emplace_tex_defs() {
  for (utl_map_iterator map_it = utl_map_begin(TEXTURE_ASSETS); map_it.node != utl_map_end(TEXTURE_ASSETS).node; utl_map_iterator_increment(&map_it)) {
    config_texture_raw_info raw_info = *(config_texture_raw_info *)utl_map_node_get_value(map_it.node);
    free(map_it.node->value);
    gfx_texture *tex = malloc(sizeof(gfx_texture));
    *tex = gfx_load_texture_asset_raw(raw_info.tex_raw, raw_info.width, raw_info.height, raw_info.channels);
    tex->width = raw_info.width;
    tex->height = raw_info.height;
    tex->angle = 0;
    map_it.node->value = tex;
    gfx_free_texture_raw(raw_info.tex_raw);
  }
  gfx_texture t = get_tex_def("BORDER_BLACK");
  printf("SIZE: %i %i\n", t.width, t.height); 
}

gfx_texture get_tex_def(char *tex_str) {
  gfx_texture *tex = utl_map_at(TEXTURE_ASSETS, tex_str);
  if (!tex) {
    return (gfx_texture){ 0 };
  }
  return *tex;
}

void free_tex_defs() {
  utl_map_deallocate(TEXTURE_ASSETS);
}

void generate_tile_defs(const char *filename) {
  utl_config_file *config = utl_config_create(filename);
}

void free_tile_defs() {
  utl_map_deallocate(TEXTURE_ASSETS);
}

