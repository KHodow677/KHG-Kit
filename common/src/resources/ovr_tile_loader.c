#include "resources/ovr_tile_loader.h"
#include "area/ovr_tile.h"
#include "khg_utl/algorithm.h"
#include <stdio.h>
#include <string.h>

static ovr_tile NO_OVR_TILE = { 0 };
static ovr_tile OVR_TILE_LOOKUP[NUM_OVR_TILES];
static ovr_tile_asset OVR_TILE_ASSET_REF[NUM_OVR_TILES];

static int compare_ovr_tile_strings(const void *a, const void *b) {
  return strcmp((const char *)a, (const char *)b);
}

const ovr_tile generate_ovr_tile(char *filepath) {
  ovr_tile ot;
  printf("%s\n", filepath);
  return ot;
}

const bool check_ovr_tile_loaded(unsigned int ovr_tile_id) {
  return (ovr_tile_id == NULL_OVR_TILE || OVR_TILE_LOOKUP[ovr_tile_id].id != NO_OVR_TILE.id);
}

const unsigned int get_ovr_tile_id_from_string(const char *ovr_tile_key) {
  return utl_algorithm_find_at(OVR_TILE_STRINGS, OVR_TILE_STRINGS_SIZE, sizeof(char *), ovr_tile_key, compare_ovr_tile_strings);
}

const ovr_tile get_or_add_ovr_tile(unsigned int ovr_tile_id) {
  if (check_ovr_tile_loaded(ovr_tile_id)) {
    return OVR_TILE_LOOKUP[ovr_tile_id];
  }
  const ovr_tile_asset ota = OVR_TILE_ASSET_REF[ovr_tile_id];
  OVR_TILE_LOOKUP[ovr_tile_id] = generate_ovr_tile(ota.ovr_tile_filepath);
  return OVR_TILE_LOOKUP[ovr_tile_id];
}

const ovr_tile get_or_add_ovr_tile_from_string(const char *ovr_tile_key) {
  const unsigned int ovr_tile_id = get_ovr_tile_id_from_string(ovr_tile_key);
  return get_or_add_ovr_tile(ovr_tile_id);
}

void generate_ovr_tiles() {
  OVR_TILE_ASSET_REF[PLAINS_CLEARING_0] = (ovr_tile_asset){ "res/assets/ovr_tiles/plains/clearing/0.ini" };
  for (unsigned int i = 0; i < NUM_OVR_TILES; i++) {
    get_or_add_ovr_tile(i);
  }
}

void reset_ovr_tiles() {
  for (unsigned int i = 0; i < NUM_OVR_TILES; i++) {
    OVR_TILE_LOOKUP[i].id = NO_OVR_TILE.id;
  }
}

