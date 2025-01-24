#pragma once

#include "area/ovr_tile.h"

#define FOREACH_OVR_TILE(TEXTURE)\
  TEXTURE(EMPTY_OVR_TILE)\
  TEXTURE(PLAINS_CLEARING_0)\
  TEXTURE(PLAINS_CLEARING_1)\
  TEXTURE(PLAINS_CLEARING_2)\
  TEXTURE(PLAINS_CLEARING_3)\
  TEXTURE(PLAINS_CLEARING_4)\
  TEXTURE(PLAINS_CLEARING_5)\
  TEXTURE(PLAINS_CLEARING_6)\
  TEXTURE(PLAINS_CLEARING_7)\
  TEXTURE(PLAINS_CLEARING_8)\
  TEXTURE(PLAINS_CLEARING_9)\
  TEXTURE(PLAINS_DENSE_0)\
  TEXTURE(PLAINS_DENSE_1)\
  TEXTURE(PLAINS_DENSE_2)\
  TEXTURE(PLAINS_DENSE_3)\
  TEXTURE(PLAINS_DENSE_4)\
  TEXTURE(PLAINS_DENSE_5)\
  TEXTURE(PLAINS_DENSE_6)\
  TEXTURE(PLAINS_DENSE_7)\
  TEXTURE(PLAINS_DENSE_8)\
  TEXTURE(PLAINS_DENSE_9)\
  TEXTURE(NUM_OVR_TILES)\

#define GENERATE_OVR_TILE_ENUM(ENUM) ENUM,
#define GENERATE_OVR_TILE_STRING(STRING) #STRING,

typedef enum {
  FOREACH_OVR_TILE(GENERATE_OVR_TILE_ENUM)
} ovr_tile_id;

#define OVR_TILE_STRINGS (char *[]){ FOREACH_OVR_TILE(GENERATE_OVR_TILE_STRING) }
#define OVR_TILE_STRINGS_SIZE sizeof(OVR_TILE_STRINGS) / sizeof(OVR_TILE_STRINGS[0])

typedef struct ovr_tile_asset {
  char *ovr_tile_filepath;
} ovr_tile_asset;

const ovr_tile generate_ovr_tile(char *filepath, const unsigned int id);
const unsigned int get_ovr_tile_id_from_string(const char *ovr_tile_key);

void add_ovr_tile(void);
const ovr_tile get_ovr_tile(unsigned int ovr_tile_id);
const ovr_tile get_ovr_tile_from_string(const char *ovr_tile_key);

void generate_ovr_tiles(void);
void reset_ovr_tiles(void);
void load_ovr_tile_tick(const unsigned int count);

