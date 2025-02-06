#define NAMESPACE_TASKING_IMPL

#include "khg_thd/concurrent.h"
#include "khg_utl/algorithm.h"
#include "khg_utl/random.h"
#include "khg_utl/string.h"
#include "khg_utl/vector.h"
#include "tasking/namespace.h"
#include "util/ovr_tile.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static utl_vector *TILE_DATA;
static thd_mutex TILE_DATA_MUTEX;

static int compare_tile_names(const void *a, const void *b) {
  const tile_object *tile_a = a;
  return strcmp(tile_a->name, (const char *)b);
}

void populate_tile_data(const char *filename) {
  thd_mutex_init(&TILE_DATA_MUTEX, THD_MUTEX_PLAIN);
  thd_mutex_lock(&TILE_DATA_MUTEX);
  TILE_DATA = utl_vector_create(sizeof(tile_object));
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
    const char *path = utl_config_get_value(config, section, "path");
    tile_object tile_obj = { .loaded = false, .fetching = false, .id = utl_vector_size(TILE_DATA) };
    strcpy(tile_obj.name, section);
    strcpy(tile_obj.path, path);
    utl_vector_push_back(TILE_DATA, &tile_obj);
  }
  utl_config_deallocate(config);
  thd_mutex_unlock(&TILE_DATA_MUTEX);
}

void load_tile_data(void *arg) {
  if (!arg) {
    return;
  }
  unsigned int *tile_id = arg;
  thd_mutex_lock(&TILE_DATA_MUTEX);
  tile_object *tile_obj = utl_vector_at(TILE_DATA, *tile_id);
  thd_mutex_unlock(&TILE_DATA_MUTEX);
  tile_obj->tile = (ovr_tile){ *tile_id };
  utl_config_file *config = utl_config_create(tile_obj->path);
  const char *ground_tex_id = utl_config_get_value(config, "info", "ground_tex");
  const char *border_tex_id = utl_config_get_value(config, "info", "border_tex");
  tile_obj->tile.ground_tex_id_loc = NAMESPACE_TASKING_INTERNAL.get_texture_id(ground_tex_id);
  tile_obj->tile.border_tex_id_loc = NAMESPACE_TASKING_INTERNAL.get_texture_id(border_tex_id);
  tile_obj->tile.num_elements = utl_config_get_int(config, "info", "num_elements", 0);
  tile_obj->tile.elements = utl_array_create(sizeof(ovr_tile_element), tile_obj->tile.num_elements);
  utl_config_iterator iterator = utl_config_get_iterator(config);
  const char *section, *key, *value;
  unsigned int count = 0;
  ovr_tile_element template_element;
  while (utl_config_next_entry(&iterator, &section, &key, &value)) {
    if (strcmp(section, "elements")) {
      continue;
    }
    utl_string *key_obj = utl_string_create(key);
    if (utl_string_starts_with(key_obj, "element_tex")) {
      const char *element_tex_id = utl_config_get_value(config, section, key);
      template_element.element_tex_id_loc = NAMESPACE_TASKING_INTERNAL.get_texture_id(element_tex_id);
      utl_string_deallocate(key_obj);
      continue;
    }
    else if (utl_string_starts_with(key_obj, "element_pos")) {
      char **element_pos = utl_config_get_array(config, section, key, 2);
      template_element.pos = phy_vector2_new(atof(element_pos[0]), atof(element_pos[1]));
      free(element_pos[0]);
      free(element_pos[1]);
      free(element_pos);
      utl_string_deallocate(key_obj);
      continue;
    }
    else if (utl_string_starts_with(key_obj, "element_flipped")) {
      template_element.flipped = utl_config_get_bool(config, section, key, false);
      utl_string_deallocate(key_obj);
      continue;
    }
    else if (utl_string_starts_with(key_obj, "element_stable")) {
      template_element.stable = utl_config_get_bool(config, section, key, false);
      template_element.parent_tile = NULL;
      thd_mutex_lock(&TILE_DATA_MUTEX);
      utl_array_set(tile_obj->tile.elements, count, &template_element);
      count++;
      thd_mutex_unlock(&TILE_DATA_MUTEX);
      utl_string_deallocate(key_obj);
      continue;
    }
  }
  utl_config_deallocate(config);
  thd_mutex_lock(&TILE_DATA_MUTEX);
  tile_obj->loaded = true;
  tile_obj->fetching = false;
  thd_mutex_unlock(&TILE_DATA_MUTEX);
}

const ovr_tile get_tile_data(const unsigned int tex_id) {
  thd_mutex_lock(&TILE_DATA_MUTEX);
  tile_object *tile_obj = utl_vector_at(TILE_DATA, tex_id);
  if (tile_obj->loaded) {
    thd_mutex_unlock(&TILE_DATA_MUTEX);
    return tile_obj->tile;
  }
  if (!tile_obj->fetching) {
    tile_obj->fetching = true;
    thd_mutex_unlock(&TILE_DATA_MUTEX);
    NAMESPACE_TASKING_INTERNAL.task_enqueue(load_tile_data, &tile_obj->id);
  }
  else {
    thd_mutex_unlock(&TILE_DATA_MUTEX);
  }
  return ((tile_object *)utl_vector_at(TILE_DATA, 0))->tile;
}

const unsigned int get_tile_id(const char *tex_name) {
  thd_mutex_lock(&TILE_DATA_MUTEX);
  unsigned int res = utl_algorithm_find_at(utl_vector_data(TILE_DATA), utl_vector_size(TILE_DATA), sizeof(tile_object), tex_name, compare_tile_names);
  thd_mutex_unlock(&TILE_DATA_MUTEX);
  return res;
}

const char *get_random_tile_name() {
  thd_mutex_lock(&TILE_DATA_MUTEX);
  tile_object *rand_tile = utl_random_choice(utl_vector_data(TILE_DATA) + sizeof(tile_object), utl_vector_size(TILE_DATA) - 1, sizeof(tile_object));
  thd_mutex_unlock(&TILE_DATA_MUTEX);
  return rand_tile->name;
}

void clear_tile_data() {
  thd_mutex_lock(&TILE_DATA_MUTEX);
  utl_vector_deallocate(TILE_DATA);
  thd_mutex_unlock(&TILE_DATA_MUTEX);
}
