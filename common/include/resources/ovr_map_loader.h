#pragma once

#include "area/ovr_map.h"

#define FOREACH_OVR_MAP(OVR_MAP)\
  OVR_MAP(NULL_OVR_MAP)\
  OVR_MAP(PLAYER_OVR_MAP)\
  OVR_MAP(NUM_OVR_MAPS)\

#define GENERATE_OVR_MAP_ENUM(ENUM) ENUM,
#define GENERATE_OVR_MAP_STRING(STRING) #STRING,

typedef enum {
  FOREACH_OVR_MAP(GENERATE_OVR_MAP_ENUM)
} ovr_map_id;

#define OVR_MAP_STRINGS (char *[]){ FOREACH_OVR_MAP(GENERATE_OVR_MAP_STRING) }
#define OVR_MAP_STRINGS_SIZE sizeof(OVR_MAP_STRINGS) / sizeof(OVR_MAP_STRINGS[0])

#define NO_OVR_MAP_ID NUM_OVR_MAPS

typedef struct ovr_map_asset {
  char *ovr_map_filepath;
} ovr_map_asset;

const ovr_map generate_ovr_map(const char *filepath);
void generate_ovr_map_from_file(ovr_map *r, const char *filepath);

const unsigned int get_ovr_map_id_from_string(const char *area_key);

const ovr_map get_ovr_map(unsigned int ovr_map_id);
const ovr_map get_ovr_map_from_string(const char *ovr_map_key);

void generate_ovr_maps(void);

