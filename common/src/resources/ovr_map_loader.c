#include "area/ovr_tile.h"
#include "khg_utl/algorithm.h"
#include "khg_utl/config.h"
#include "khg_utl/string.h"
#include "khg_utl/vector.h"
#include "resources/ovr_tile_loader.h"
#include "resources/ovr_map_loader.h"
#include <stdlib.h>
#include <string.h>

static ovr_map_asset OVR_MAP_ASSET_REF[NUM_OVR_MAPS];

static int compare_ovr_map_strings(const void *a, const void *b) {
  return strcmp(*(const char **)a, (const char *)b);
}

const ovr_map generate_ovr_map(const char *filepath) {
  ovr_map map = { true, utl_vector_create(sizeof(ovr_tile_info)) };
  generate_ovr_map_from_file(&map, filepath);
  return map;
}

void generate_ovr_map_from_file(ovr_map *m, const char *filepath) {
  utl_config_file *config = utl_config_create(filepath);
  utl_config_iterator iterator = utl_config_get_iterator(config);
  const char *section, *key, *value;
  ovr_tile_info template_tile_info;
  while (utl_config_next_entry(&iterator, &section, &key, &value)) {
    if (strcmp(section, "tiles")) {
      continue;
    }
    utl_string *key_obj = utl_string_create(key);
    if (utl_string_starts_with(key_obj, "tile_id")) {
      template_tile_info.id = get_ovr_tile_id_from_string(utl_config_get_value(config, section, key));
      utl_string_deallocate(key_obj);
      continue;
    }
    else if (utl_string_starts_with(key_obj, "tile_pos")) {
      char **tile_pos = utl_config_get_array(config, section, key, 2);
      template_tile_info.pos = phy_vector2_new(atof(tile_pos[0]), atof(tile_pos[1]));
      free(tile_pos[0]);
      free(tile_pos[1]);
      free(tile_pos);
      utl_vector_push_back(m->tiles, &template_tile_info);
      utl_string_deallocate(key_obj);
    }
  }
  utl_config_deallocate(config);
}

const unsigned int get_ovr_map_id_from_string(const char *ovr_map_key) {
  return utl_algorithm_find_at(OVR_MAP_STRINGS, OVR_MAP_STRINGS_SIZE, sizeof(char *), ovr_map_key, compare_ovr_map_strings);
}

const ovr_map get_ovr_map(unsigned int ovr_map_id) {
  const ovr_map_asset ra = OVR_MAP_ASSET_REF[ovr_map_id];
  return generate_ovr_map(ra.ovr_map_filepath);
}

const ovr_map get_ovr_map_from_string(const char *ovr_map_key) {
  const int area_id = get_ovr_map_id_from_string(ovr_map_key);
  return get_ovr_map(area_id);
}

void generate_ovr_maps() {
  OVR_MAP_ASSET_REF[PLAYER_OVR_MAP] = (ovr_map_asset){ "res/assets/data/ovr_maps/0.ini" };
}

