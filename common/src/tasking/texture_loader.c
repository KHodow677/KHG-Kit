#include <stdio.h>
#define NAMESPACE_TASKING_IMPL

#include "khg_gfx/texture.h"
#include "khg_thd/concurrent.h"
#include "khg_utl/algorithm.h"
#include "khg_utl/config.h"
#include "khg_utl/vector.h"
#include "tasking/namespace.h"
#include <string.h>

static utl_vector *TEXTURE_DATA = NULL;
static thd_mutex TEXTURE_DATA_MUTEX = { 0 };

static int compare_texture_names(const void *a, const void *b) {
  const texture_object *tex_a = a;
  return strcmp(tex_a->name, (const char *)b);
}

static unsigned int get_num_textures(utl_config_iterator iterator) {
  const char *section, *key, *value;
  char last_section[128];
  unsigned int count = 0;
  while (utl_config_next_entry(&iterator, &section, &key, &value)) {
    if (!strcmp(last_section, section)) {
      strcpy(last_section, section);
      continue;
    };
    strcpy(last_section, section);
    count++;
  }
  return count;
}

void populate_texture_data(const char *filename) {
  thd_mutex_init(&TEXTURE_DATA_MUTEX, THD_MUTEX_PLAIN);
  TEXTURE_DATA = utl_vector_create(sizeof(texture_object));
  utl_config_file *config = utl_config_create(filename);
  utl_config_iterator iterator = utl_config_get_iterator(config);
  utl_vector_reserve(TEXTURE_DATA, get_num_textures(iterator));
  const char *section, *key, *value;
  char last_section[128];
  while (utl_config_next_entry(&iterator, &section, &key, &value)) {
    if (!strcmp(last_section, section)) {
      strcpy(last_section, section);
      continue;
    };
    strcpy(last_section, section);
    const char *path = utl_config_get_value(config, section, "path");
    const int width = utl_config_get_int(config, section, "width", 512);
    const int height = utl_config_get_int(config, section, "height", 512);
    texture_object tex_obj = { .loaded = false, .fetching = false, .uploaded = false, .id = utl_vector_size(TEXTURE_DATA) };
    strcpy(tex_obj.name, section);
    strcpy(tex_obj.path, path);
    tex_obj.width = width;
    tex_obj.height = height;
    utl_vector_push_back(TEXTURE_DATA, &tex_obj);
  }
  utl_config_deallocate(config);
  unsigned int empty_id = 0;
  NAMESPACE_TASKING_INTERNAL.load_texture_data(&empty_id);
}

void load_texture_data(void *arg) {
  if (!arg) {
    return;
  }
  unsigned int *tex_id = arg;
  thd_mutex_lock(&TEXTURE_DATA_MUTEX);
  texture_object *tex_obj = utl_vector_at(TEXTURE_DATA, *tex_id);
  thd_mutex_unlock(&TEXTURE_DATA_MUTEX);
  tex_obj->texture = gfx_load_texture_asset(tex_obj->path);
  int width, height, channels;
  gfx_fetch_texture_raw(&tex_obj->texture_raw, tex_obj->path, &width, &height, &channels);
  tex_obj->texture.width = tex_obj->width;
  tex_obj->texture.height = tex_obj->height;
  tex_obj->loaded = true;
  tex_obj->fetching = false;
}

gfx_texture get_texture_data(const unsigned int tex_id) {
  thd_mutex_lock(&TEXTURE_DATA_MUTEX);
  texture_object *tex_obj = utl_vector_at(TEXTURE_DATA, tex_id);
  if (tex_obj && tex_obj->loaded) {
    if (!tex_obj->uploaded) {
      tex_obj->texture = gfx_load_texture_asset_raw(tex_obj->texture_raw, tex_obj->width, tex_obj->height, tex_obj->channels);
      gfx_free_texture_raw(tex_obj->texture_raw);
      tex_obj->uploaded = true;
    }
    thd_mutex_unlock(&TEXTURE_DATA_MUTEX);
    return tex_obj->texture;
  }
  if (!tex_obj->fetching) {
    NAMESPACE_TASKING_INTERNAL.task_enqueue(load_texture_data, &tex_obj->id);
    tex_obj->fetching = true;
  }
  thd_mutex_unlock(&TEXTURE_DATA_MUTEX);
  return ((texture_object *)utl_vector_at(TEXTURE_DATA, 0))->texture;
}

const unsigned int get_texture_id(const char *tex_name) {
  return utl_algorithm_find_at(utl_vector_data(TEXTURE_DATA), utl_vector_size(TEXTURE_DATA), sizeof(texture_object), tex_name, compare_texture_names);
}

void clear_texture_data() {
  utl_vector_deallocate(TEXTURE_DATA);
}

