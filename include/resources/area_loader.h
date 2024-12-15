#pragma once

#include "area/collider.h"
#include "area/tile.h"

#define FOREACH_AREA(AREA)\
  AREA(DUNGEON_0)\
  AREA(NUM_AREAS)\

#define GENERATE_AREA_ENUM(ENUM) ENUM,
#define GENERATE_AREA_STRING(STRING) #STRING,

typedef enum {
  FOREACH_AREA(GENERATE_AREA_ENUM)
} area_id;

#define AREA_STRINGS (char *[]){ FOREACH_AREA(GENERATE_AREA_STRING) }
#define AREA_STRINGS_SIZE sizeof(AREA_STRINGS) / sizeof(AREA_STRINGS[0])

#define NO_AREA_ID NUM_AREAS

typedef struct area_asset_info {
  size_t num_tiles;
  size_t num_colliders;
  int tiles_layer;
  int objects_layer;
} area_asset_info;

typedef struct area_asset {
  char *tile_filepath;
  char *collider_filepath;
  char *object_filepath;
  area_asset_info info;
} area_asset;

const area_asset_info generate_area_info(const char *tile_filepath, const char *collider_filepath, const char *object_filepath);

const area_tiles generate_area_tiles_from_file(const char *tile_filepath, size_t num_tiles, int tiles_layer);
const area_colliders generate_area_colliders_from_file(const char *collider_filepath, size_t num_colliders, bool enabled);

const size_t get_area_id_from_string(const char *area_key);

const area_tiles get_area_tiles(size_t rig_id);
const area_tiles get_area_tiles_from_string(const char *area_key);

const area_colliders get_area_colliders(size_t rig_id, bool enabled);
const area_colliders get_area_colliders_from_string(const char *area_key, bool enabled);

void generate_areas();

