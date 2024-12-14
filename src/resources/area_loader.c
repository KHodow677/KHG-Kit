#include "resources/area_loader.h"
#include "resources/texture_loader.h"
#include "tile/area.h"
#include "tile/tile.h"
#include "khg_utl/algorithm.h"
#include "khg_utl/array.h"
#include "khg_utl/config.h"
#include <stddef.h>
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

const area_tiles generate_area_tiles_from_file(const char *tile_filepath, size_t num_tiles, int tiles_layer) {
  area_tiles at = { true, tiles_layer, utl_array_create(sizeof(area_tile), num_tiles) };
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
    utl_array_set(at.tiles, count++, &tile);
  }
  utl_config_deallocate(config);
  return at;
}

const size_t get_area_id_from_string(const char *area_key) {
  return (size_t)utl_algorithm_find_at(AREA_STRINGS, AREA_STRINGS_SIZE, sizeof(char *), area_key, compare_area_strings);
}

const area_tiles get_area_tiles(size_t rig_id) {
  const area_asset aa = AREA_ASSET_REF[rig_id];
  area_asset_info aai = generate_area_info(aa.tile_filepath, aa.collider_filepath, aa.object_filepath);
  return generate_area_tiles_from_file(aa.tile_filepath, aai.num_tiles, aai.tiles_layer);
}

const area_tiles get_area_tiles_from_string(const char *area_key) {
  const size_t area_id = get_area_id_from_string(area_key);
  return get_area_tiles(area_id);
}

void generate_areas() {
  AREA_ASSET_REF[DUNGEON_0] = (area_asset){ "res/assets/data/areas/dungeon/0/tiles.ini", "res/assets/data/areas/dungeon/0/colliders.ini", "res/assets/data/areas/dungeon/0/objects.ini" };
}

