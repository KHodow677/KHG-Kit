#include "area/collider.h"
#include "area/tile.h"
#include "khg_utl/algorithm.h"
#include "khg_utl/array.h"
#include "khg_utl/config.h"
#include "khg_utl/string.h"
#include "resources/area_loader.h"
#include "resources/texture_loader.h"
#include <stdlib.h>
#include <string.h>

static area_asset AREA_ASSET_REF[NUM_AREAS];

static int compare_area_strings(const void *a, const void *b) {
  return strcmp(*(const char **)a, (const char *)b);
}

const area_asset_info generate_area_info(const char *tile_filepath, const char *collider_filepath, const char *object_filepath) {
  area_asset_info area_info;
  utl_config_file *tile_config = utl_config_create(tile_filepath);
  utl_config_file *collider_config = utl_config_create(collider_filepath);
  utl_config_file *object_config = utl_config_create(object_filepath);
  area_info.num_tiles = utl_config_get_int(tile_config, "area", "num_tiles", 0);
  area_info.tiles_layer = utl_config_get_int(tile_config, "area", "tiles_layer", 0);
  area_info.num_colliders = utl_config_get_int(collider_config, "area", "num_colliders", 0);
  area_info.objects_layer = utl_config_get_int(object_config, "area", "objects_layer", 0);
  utl_config_deallocate(tile_config);
  utl_config_deallocate(collider_config);
  utl_config_deallocate(object_config);
  return area_info;
}

const area_tiles generate_area_tiles_from_file(const char *tile_filepath, unsigned int num_tiles, int tiles_layer) {
  const area_tiles at = { true, tiles_layer, utl_array_create(sizeof(area_tile), num_tiles) };
  utl_config_file *config = utl_config_create(tile_filepath);
  utl_config_iterator iterator = utl_config_get_iterator(config);
  const char *section, *key, *value;
  area_tile tile = { 0 };
  unsigned int count = 0;
  while (utl_config_next_entry(&iterator, &section, &key, &value)) {
    if (strcmp(section, "area_tiles")) {
      continue;
    }
    utl_string *key_obj = utl_string_create(key);
    if (utl_string_starts_with(key_obj, "tile_tex")) {
      tile.tex_id = get_tex_id_from_string((const char *)utl_config_get_value(config, section, key));
      utl_string_deallocate(key_obj);
      continue;
    }
    else if (utl_string_starts_with(key_obj, "tile_pos")) {
      char **tile_pos = utl_config_get_array(config, section, key, 2);
      tile.pos = phy_vector2_new(atof(tile_pos[0]), atof(tile_pos[1]));
      free(tile_pos[0]);
      free(tile_pos[1]);
      free(tile_pos);
      utl_string_deallocate(key_obj);
    }
    utl_array_set(at.tiles, count++, &tile);
  }
  utl_config_deallocate(config);
  return at;
}

const area_colliders generate_area_colliders_from_file(const char *collider_filepath, unsigned int num_colliders, bool enabled) {
  const area_colliders at = { enabled, utl_array_create(sizeof(area_collider), num_colliders) };
  utl_config_file *config = utl_config_create(collider_filepath);
  utl_config_iterator iterator = utl_config_get_iterator(config);
  const char *section, *key, *value;
  phy_vector2 c_pos = { 0 };
  phy_vector2 c_size = { 0 };
  unsigned int count = 0;
  while (utl_config_next_entry(&iterator, &section, &key, &value)) {
    if (strcmp(section, "area_colliders")) {
      continue;
    }
    utl_string *key_obj = utl_string_create(key);
    if (utl_string_starts_with(key_obj, "collider_pos")) {
      char **collider_pos = utl_config_get_array(config, section, key, 2);
      c_pos = phy_vector2_new(atof(collider_pos[0]), atof(collider_pos[1]));
      free(collider_pos[0]);
      free(collider_pos[1]);
      free(collider_pos);
      utl_string_deallocate(key_obj);
      continue;
    }
    else if (utl_string_starts_with(key_obj, "collider_size")) {
      char **collider_size = utl_config_get_array(config, section, key, 2);
      c_size = phy_vector2_new(atof(collider_size[0]), atof(collider_size[1]));
      free(collider_size[0]);
      free(collider_size[1]);
      free(collider_size);
      utl_string_deallocate(key_obj);
    }
    area_collider collider = create_collider(c_pos, c_size);
    utl_array_set(at.colliders, count++, &collider);
  }
  utl_config_deallocate(config);
  return at;
}


const unsigned int get_area_id_from_string(const char *area_key) {
  return utl_algorithm_find_at(AREA_STRINGS, AREA_STRINGS_SIZE, sizeof(char *), area_key, compare_area_strings);
}

const area_tiles get_area_tiles(unsigned int rig_id) {
  const area_asset aa = AREA_ASSET_REF[rig_id];
  area_asset_info aai = generate_area_info(aa.tile_filepath, aa.collider_filepath, aa.object_filepath);
  return generate_area_tiles_from_file(aa.tile_filepath, aai.num_tiles, aai.tiles_layer);
}

const area_tiles get_area_tiles_from_string(const char *area_key) {
  const unsigned int area_id = get_area_id_from_string(area_key);
  return get_area_tiles(area_id);
}

const area_colliders get_area_colliders(unsigned int rig_id, bool enabled) {
  const area_asset aa = AREA_ASSET_REF[rig_id];
  area_asset_info aai = generate_area_info(aa.tile_filepath, aa.collider_filepath, aa.object_filepath);
  return generate_area_colliders_from_file(aa.collider_filepath, aai.num_colliders, enabled);
}

const area_colliders get_area_colliders_from_string(const char *area_key, bool enabled) {
  const unsigned int area_id = get_area_id_from_string(area_key);
  return get_area_colliders(area_id, enabled);
}

void generate_areas() {
  AREA_ASSET_REF[DUNGEON_BG] = (area_asset){ "res/assets/data/areas/dungeon/background.ini", "res/assets/data/areas/dungeon/0/colliders.ini", "res/assets/data/areas/dungeon/0/objects.ini" };
  AREA_ASSET_REF[DUNGEON_0] = (area_asset){ "res/assets/data/areas/dungeon/0/tiles.ini", "res/assets/data/areas/dungeon/0/colliders.ini", "res/assets/data/areas/dungeon/0/objects.ini" };
}

