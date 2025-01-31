#define NAMESPACE_TASKING_IMPL

#include "khg_utl/algorithm.h"
#include "khg_utl/array.h"
#include "khg_utl/config.h"
#include "khg_utl/string.h"
#include "tasking/namespace.h"
#include "util/ovr_tile.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static ovr_tile NO_OVR_TILE = { 0 };
static ovr_tile OVR_TILE_LOOKUP[NUM_OVR_TILES];
static tasking_ovr_tile_asset OVR_TILE_ASSET_REF[NUM_OVR_TILES];

static int compare_ovr_tile_strings(const void *a, const void *b) {
  return strcmp(*(const char **)a, (const char *)b);
}

static const ovr_tile generate_ovr_tile(char *filepath, const unsigned int id) {
  ovr_tile ot = { id };
  utl_config_file *config = utl_config_create(filepath);
  const char *ground_tex_id = utl_config_get_value(config, "info", "ground_tex");
  const char *border_tex_id = utl_config_get_value(config, "info", "border_tex");
  ot.ground_tex_id_loc = NAMESPACE_TASKING_INTERNAL.get_location_tex_str(ground_tex_id);
  ot.border_tex_id_loc = NAMESPACE_TASKING_INTERNAL.get_location_tex_str(border_tex_id);
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
      const char *element_tex_id = utl_config_get_value(config, section, key);
      template_element.element_tex_id_loc = NAMESPACE_TASKING_INTERNAL.get_location_tex_str(element_tex_id);
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
      utl_array_set(ot.elements, count, &template_element);
      count++;
      utl_string_deallocate(key_obj);
      continue;
    }
  }
  utl_config_deallocate(config);
  return ot;
}

static void add_ovr_tile(void) {
  const tasking_ovr_tile_asset ota = OVR_TILE_ASSET_REF[NAMESPACE_TASKING_INTERNAL.OVR_TILE_THREAD.progress];
  OVR_TILE_LOOKUP[NAMESPACE_TASKING_INTERNAL.OVR_TILE_THREAD.progress] = generate_ovr_tile(ota.ovr_tile_filepath, NAMESPACE_TASKING_INTERNAL.OVR_TILE_THREAD.progress);
  NAMESPACE_TASKING_INTERNAL.OVR_TILE_THREAD.progress++;
}

const unsigned int get_ovr_tile_id_from_string(const char *ovr_tile_key) {
  return utl_algorithm_find_at(OVR_TILE_STRINGS, OVR_TILE_STRINGS_SIZE, sizeof(char *), ovr_tile_key, compare_ovr_tile_strings);
}

const ovr_tile get_ovr_tile(unsigned int ovr_tile_id) {
  return OVR_TILE_LOOKUP[ovr_tile_id];
}

const ovr_tile get_ovr_tile_from_string(const char *ovr_tile_key) {
  const unsigned int ovr_tile_id = get_ovr_tile_id_from_string(ovr_tile_key);
  return get_ovr_tile(ovr_tile_id);
}

void generate_ovr_tiles() {
  OVR_TILE_ASSET_REF[EMPTY_OVR_TILE] = (tasking_ovr_tile_asset){ "res/assets/data/ovr_tiles/empty.ini" };
  OVR_TILE_ASSET_REF[PLAINS_CLEARING_0] = (tasking_ovr_tile_asset){ "res/assets/data/ovr_tiles/plains/clearing/0.ini" };
  OVR_TILE_ASSET_REF[PLAINS_CLEARING_1] = (tasking_ovr_tile_asset){ "res/assets/data/ovr_tiles/plains/clearing/1.ini" };
  OVR_TILE_ASSET_REF[PLAINS_CLEARING_2] = (tasking_ovr_tile_asset){ "res/assets/data/ovr_tiles/plains/clearing/2.ini" };
  OVR_TILE_ASSET_REF[PLAINS_CLEARING_3] = (tasking_ovr_tile_asset){ "res/assets/data/ovr_tiles/plains/clearing/3.ini" };
  OVR_TILE_ASSET_REF[PLAINS_CLEARING_4] = (tasking_ovr_tile_asset){ "res/assets/data/ovr_tiles/plains/clearing/4.ini" };
  OVR_TILE_ASSET_REF[PLAINS_CLEARING_5] = (tasking_ovr_tile_asset){ "res/assets/data/ovr_tiles/plains/clearing/5.ini" };
  OVR_TILE_ASSET_REF[PLAINS_CLEARING_6] = (tasking_ovr_tile_asset){ "res/assets/data/ovr_tiles/plains/clearing/6.ini" };
  OVR_TILE_ASSET_REF[PLAINS_CLEARING_7] = (tasking_ovr_tile_asset){ "res/assets/data/ovr_tiles/plains/clearing/7.ini" };
  OVR_TILE_ASSET_REF[PLAINS_CLEARING_8] = (tasking_ovr_tile_asset){ "res/assets/data/ovr_tiles/plains/clearing/8.ini" };
  OVR_TILE_ASSET_REF[PLAINS_CLEARING_9] = (tasking_ovr_tile_asset){ "res/assets/data/ovr_tiles/plains/clearing/9.ini" };
  OVR_TILE_ASSET_REF[PLAINS_DENSE_0] = (tasking_ovr_tile_asset){ "res/assets/data/ovr_tiles/plains/dense/0.ini" };
  OVR_TILE_ASSET_REF[PLAINS_DENSE_1] = (tasking_ovr_tile_asset){ "res/assets/data/ovr_tiles/plains/dense/1.ini" };
  OVR_TILE_ASSET_REF[PLAINS_DENSE_2] = (tasking_ovr_tile_asset){ "res/assets/data/ovr_tiles/plains/dense/2.ini" };
  OVR_TILE_ASSET_REF[PLAINS_DENSE_3] = (tasking_ovr_tile_asset){ "res/assets/data/ovr_tiles/plains/dense/3.ini" };
  OVR_TILE_ASSET_REF[PLAINS_DENSE_4] = (tasking_ovr_tile_asset){ "res/assets/data/ovr_tiles/plains/dense/4.ini" };
  OVR_TILE_ASSET_REF[PLAINS_DENSE_5] = (tasking_ovr_tile_asset){ "res/assets/data/ovr_tiles/plains/dense/5.ini" };
  OVR_TILE_ASSET_REF[PLAINS_DENSE_6] = (tasking_ovr_tile_asset){ "res/assets/data/ovr_tiles/plains/dense/6.ini" };
  OVR_TILE_ASSET_REF[PLAINS_DENSE_7] = (tasking_ovr_tile_asset){ "res/assets/data/ovr_tiles/plains/dense/7.ini" };
  OVR_TILE_ASSET_REF[PLAINS_DENSE_8] = (tasking_ovr_tile_asset){ "res/assets/data/ovr_tiles/plains/dense/8.ini" };
  OVR_TILE_ASSET_REF[PLAINS_DENSE_9] = (tasking_ovr_tile_asset){ "res/assets/data/ovr_tiles/plains/dense/9.ini" };
}

int load_ovr_tile_tick(void *arg) {
  tasking_resource_thread *thread = arg;
  if (thread->progress < NUM_OVR_TILES) {
    add_ovr_tile();
  }
  return 0;
}

