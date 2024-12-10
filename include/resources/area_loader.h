#pragma once

#include "tile/area.h"

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

typedef struct area_asset {
  char *tile_filepath;
  char *collider_filepath;
  char *object_filepath;
} area_asset;

const area generate_area(const char *tile_filepath, const char *collider_filepath, const char *object_filepath);

void generate_area_setup(area *a, const char *tile_filepath, const char *collider_filepath, const char *object_filepath);

void generate_area_tiles_from_file(area *a, const char *tile_filepath);
void generate_area_colliders_from_file(area *a, const char *collider_filepath);
void generate_area_objects_from_file(area *a, const char *object_filepath);

const bool check_area_loaded(size_t area_id);
const size_t get_area_id_from_string(const char *area_key);

const area get_or_add_area(size_t area_id);
const area get_or_add_area_from_string(const char *area_key);

void generate_areas(void);
void cleanup_areas(void);
void reset_areas(void);
