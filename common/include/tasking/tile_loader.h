#pragma once

#include "util/ovr_tile.h"

void populate_tile_data(const char *filename);
void load_tile_data(void *arg);

const ovr_tile get_tile_data(const unsigned int tile_id);
const unsigned int get_tile_id(const char *tile_name);

void clear_tile_data(void);
