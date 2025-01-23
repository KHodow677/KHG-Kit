#include "area/ovr_tile.h"
#include "khg_utl/algorithm.h"
#include "khg_utl/array.h"
#include "khg_utl/config.h"
#include "khg_utl/string.h"
#include "resources/texture_loader.h"
#include "resources/ovr_tile_loader.h"
#include <stdlib.h>
#include <string.h>

static ovr_tile NO_OVR_TILE = { 0 };
static ovr_tile OVR_TILE_LOOKUP[NUM_OVR_TILES];
static ovr_tile_asset OVR_TILE_ASSET_REF[NUM_OVR_TILES];

static int compare_ovr_tile_strings(const void *a, const void *b) {
  return strcmp(*(const char **)a, (const char *)b);
}

const ovr_tile generate_ovr_tile(char *filepath, const unsigned int id) {
  ovr_tile ot = { id };
  utl_config_file *config = utl_config_create(filepath);
  ot.ground_tex_id = get_tex_id_from_string(utl_config_get_value(config, "info", "ground_tex"));
  ot.border_tex_id = get_tex_id_from_string(utl_config_get_value(config, "info", "border_tex"));
  ot.num_elements = utl_config_get_int(config, "info", "num_elements", 0);
  ot.elements = utl_array_create(sizeof(ovr_tile_element), ot.num_elements);
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
      template_element.element_tex_id = get_tex_id_from_string(utl_config_get_value(config, section, key));
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
      template_element.parent_tile = NULL;
      utl_array_set(ot.elements, count, &template_element);
      count++;
      utl_string_deallocate(key_obj);
      continue;
    }
  }
  utl_config_deallocate(config);
  return ot;
}

const bool check_ovr_tile_loaded(unsigned int ovr_tile_id) {
  return (ovr_tile_id == NULL_OVR_TILE || OVR_TILE_LOOKUP[ovr_tile_id].tile_id != NO_OVR_TILE.tile_id);
}

const unsigned int get_ovr_tile_id_from_string(const char *ovr_tile_key) {
  return utl_algorithm_find_at(OVR_TILE_STRINGS, OVR_TILE_STRINGS_SIZE, sizeof(char *), ovr_tile_key, compare_ovr_tile_strings);
}

const ovr_tile get_or_add_ovr_tile(unsigned int ovr_tile_id) {
  if (check_ovr_tile_loaded(ovr_tile_id)) {
    return OVR_TILE_LOOKUP[ovr_tile_id];
  }
  const ovr_tile_asset ota = OVR_TILE_ASSET_REF[ovr_tile_id];
  OVR_TILE_LOOKUP[ovr_tile_id] = generate_ovr_tile(ota.ovr_tile_filepath, ovr_tile_id);
  return OVR_TILE_LOOKUP[ovr_tile_id];
}

const ovr_tile get_or_add_ovr_tile_from_string(const char *ovr_tile_key) {
  const unsigned int ovr_tile_id = get_ovr_tile_id_from_string(ovr_tile_key);
  return get_or_add_ovr_tile(ovr_tile_id);
}

void generate_ovr_tiles() {
  OVR_TILE_ASSET_REF[PLAINS_CLEARING_0] = (ovr_tile_asset){ "res/assets/data/ovr_tiles/plains/clearing/0.ini" };
  OVR_TILE_ASSET_REF[PLAINS_CLEARING_1] = (ovr_tile_asset){ "res/assets/data/ovr_tiles/plains/clearing/1.ini" };
  OVR_TILE_ASSET_REF[PLAINS_CLEARING_2] = (ovr_tile_asset){ "res/assets/data/ovr_tiles/plains/clearing/2.ini" };
  OVR_TILE_ASSET_REF[PLAINS_CLEARING_3] = (ovr_tile_asset){ "res/assets/data/ovr_tiles/plains/clearing/3.ini" };
  OVR_TILE_ASSET_REF[PLAINS_CLEARING_4] = (ovr_tile_asset){ "res/assets/data/ovr_tiles/plains/clearing/4.ini" };
  OVR_TILE_ASSET_REF[PLAINS_CLEARING_5] = (ovr_tile_asset){ "res/assets/data/ovr_tiles/plains/clearing/5.ini" };
  OVR_TILE_ASSET_REF[PLAINS_CLEARING_6] = (ovr_tile_asset){ "res/assets/data/ovr_tiles/plains/clearing/6.ini" };
  OVR_TILE_ASSET_REF[PLAINS_CLEARING_7] = (ovr_tile_asset){ "res/assets/data/ovr_tiles/plains/clearing/7.ini" };
  OVR_TILE_ASSET_REF[PLAINS_CLEARING_8] = (ovr_tile_asset){ "res/assets/data/ovr_tiles/plains/clearing/8.ini" };
  OVR_TILE_ASSET_REF[PLAINS_CLEARING_9] = (ovr_tile_asset){ "res/assets/data/ovr_tiles/plains/clearing/9.ini" };
  OVR_TILE_ASSET_REF[PLAINS_DENSE_0] = (ovr_tile_asset){ "res/assets/data/ovr_tiles/plains/dense/0.ini" };
  OVR_TILE_ASSET_REF[PLAINS_DENSE_1] = (ovr_tile_asset){ "res/assets/data/ovr_tiles/plains/dense/1.ini" };
  OVR_TILE_ASSET_REF[PLAINS_DENSE_2] = (ovr_tile_asset){ "res/assets/data/ovr_tiles/plains/dense/2.ini" };
  OVR_TILE_ASSET_REF[PLAINS_DENSE_3] = (ovr_tile_asset){ "res/assets/data/ovr_tiles/plains/dense/3.ini" };
  OVR_TILE_ASSET_REF[PLAINS_DENSE_4] = (ovr_tile_asset){ "res/assets/data/ovr_tiles/plains/dense/4.ini" };
  OVR_TILE_ASSET_REF[PLAINS_DENSE_5] = (ovr_tile_asset){ "res/assets/data/ovr_tiles/plains/dense/5.ini" };
  OVR_TILE_ASSET_REF[PLAINS_DENSE_6] = (ovr_tile_asset){ "res/assets/data/ovr_tiles/plains/dense/6.ini" };
  OVR_TILE_ASSET_REF[PLAINS_DENSE_7] = (ovr_tile_asset){ "res/assets/data/ovr_tiles/plains/dense/7.ini" };
  OVR_TILE_ASSET_REF[PLAINS_DENSE_8] = (ovr_tile_asset){ "res/assets/data/ovr_tiles/plains/dense/8.ini" };
  OVR_TILE_ASSET_REF[PLAINS_DENSE_9] = (ovr_tile_asset){ "res/assets/data/ovr_tiles/plains/dense/9.ini" };
  for (unsigned int i = 0; i < NUM_OVR_TILES; i++) {
    get_or_add_ovr_tile(i);
  }
}

void reset_ovr_tiles() {
  for (unsigned int i = 0; i < NUM_OVR_TILES; i++) {
    OVR_TILE_LOOKUP[i].tile_id = NO_OVR_TILE.tile_id;
  }
}

