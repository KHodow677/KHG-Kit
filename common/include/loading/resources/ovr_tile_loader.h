#pragma once

#include "util/ovr_tile.h"

const unsigned int get_ovr_tile_id_from_string(const char *ovr_tile_key);
const ovr_tile get_ovr_tile(unsigned int ovr_tile_id);
const ovr_tile get_ovr_tile_from_string(const char *ovr_tile_key);

void generate_ovr_tiles(void);
int load_ovr_tile_tick(void *arg);

