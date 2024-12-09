#include "resources/area_loader.h"
#include "khg_utl/algorithm.h"
#include "khg_utl/array.h"
#include "khg_utl/config.h"
#include "resources/texture_loader.h"
#include "tile/area.h"
#include <stddef.h>
#include <string.h>

static area NO_TEXTURE = { 0 };
static area AREA_LOOKUP[NUM_AREAS];
static area_asset AREA_ASSET_REF[NUM_AREAS];

static int compare_area_strings(const void *a, const void *b) {
  return strcmp(*(const char **)a, (const char *)b);
}

const area generate_area(const char *tile_filepath, const char *collider_filepath, const char *object_filepath) {
  area a;
  generate_area_setup(&a, tile_filepath, collider_filepath, object_filepath);
  generate_area_tiles_from_file(&a, tile_filepath);
  generate_area_colliders_from_file(&a, collider_filepath);
  generate_area_objects_from_file(&a, object_filepath);
  return a;
}

void generate_area_setup(area *a, const char *tile_filepath, const char *collider_filepath, const char *object_filepath) {
  utl_config_file *tile_config = utl_config_create(tile_filepath);
  utl_config_file *collider_config = utl_config_create(collider_filepath);
  utl_config_file *object_config = utl_config_create(object_filepath);
  int num_tiles = utl_config_get_int(tile_config, "area", "num_tiles", 0);
  int tiles_layer = utl_config_get_int(tile_config, "area", "tiles_layer", 0);
  int num_colliders = utl_config_get_int(collider_config, "area", "num_colliders", 0);
  int objects_layer = utl_config_get_int(object_config, "area", "objects_layer", 0);
  create_area(a, num_tiles,  num_colliders, tiles_layer, objects_layer);
  utl_config_deallocate(tile_config);
  utl_config_deallocate(collider_config);
  utl_config_deallocate(object_config);
}

void generate_area_tiles_from_file(area *a, const char *tile_filepath) {
  utl_config_file *config = utl_config_create(tile_filepath);
  utl_config_iterator iterator = utl_config_get_iterator(config);
  const char *section, *key, *value;
  area_tile tile = { 0 };
  size_t count = 0;
  while (utl_config_next_entry(&iterator, &section, &key, &value)) {
    if (strcmp(section, "area_tiles")) {
      continue;
    }
    size_t len = strlen(key);
    char generic_key[len];
    strncpy(generic_key, key, len - 1);
    generic_key[len - 1] = '\0'; 
    if (!strcmp(generic_key, "tile_tex")) {
      tile.tex_id = get_tex_id_from_string((const char *)utl_config_get_value(config, section, key));
      continue;
    }
    else if (!strcmp(generic_key, "tile_pos")) {
      char **tile_pos = utl_config_get_array(config, section, key, 2);
      tile.pos = phy_vector2_new(atof(tile_pos[0]), atof(tile_pos[1]));
      free(tile_pos[0]);
      free(tile_pos[1]);
      free(tile_pos);
    }
    utl_array_set(a->tiles, count++, &tile);
  }
  utl_config_deallocate(config);
}

void generate_area_colliders_from_file(area *a, const char *object_filepath) {

}

void generate_area_objects_from_file(area *a, const char *object_filepath) {

}

const bool check_area_loaded(size_t area_id) {
  return (AREA_LOOKUP[area_id].loaded != NO_TEXTURE.loaded);
}

const size_t get_area_id_from_string(const char *area_key) {
  return (size_t)utl_algorithm_find_at(AREA_STRINGS, AREA_STRINGS_SIZE, sizeof(char *), area_key, compare_area_strings);
}

const area get_or_add_area(size_t area_id) {
  if (check_area_loaded(area_id)) {
    return AREA_LOOKUP[area_id];
  }
  const area_asset aa = AREA_ASSET_REF[area_id];
  AREA_LOOKUP[area_id] = generate_area(aa.tile_filepath, aa.collider_filepath, aa.object_filepath);
  return AREA_LOOKUP[area_id];
}

const area get_or_add_area_from_string(const char *area_key) {
  const int area_id = get_area_id_from_string(area_key);
  return get_or_add_area(area_id);
}

void generate_areas() {
  AREA_ASSET_REF[DUNGEON_0] = (area_asset){ "res/assets/data/areas/dungeon/0/tiles.ini", "res/assets/data/areas/dungeon/0/colliders.ini", "res/assets/data/areas/dungeon/0/objects.ini" };
  for (size_t i = 0; i < NUM_AREAS; i++) {
    get_or_add_area(i);
  }
}

void cleanup_areas() {
  for (size_t i = 0; i < NUM_AREAS; i++) {
    if (check_area_loaded(i)) {
      area a = AREA_LOOKUP[i];
      free_area(&a);
    }
  }
}

void reset_areas() {
  for (size_t i = 0; i < NUM_AREAS; i++) {
    AREA_LOOKUP[i].loaded = NO_TEXTURE.loaded;
  }
}
