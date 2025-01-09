#pragma once

#include "rig/rig.h"

#define FOREACH_RIG(RIG)\
  RIG(PLAYER_RIG)\
  RIG(NUM_RIGS)\

#define GENERATE_RIG_ENUM(ENUM) ENUM,
#define GENERATE_RIG_STRING(STRING) #STRING,

typedef enum {
  FOREACH_RIG(GENERATE_RIG_ENUM)
} rig_id;

#define RIG_STRINGS (char *[]){ FOREACH_RIG(GENERATE_RIG_STRING) }
#define RIG_STRINGS_SIZE sizeof(RIG_STRINGS) / sizeof(RIG_STRINGS[0])

#define NO_RIG_ID NUM_RIGS

typedef struct rig_asset {
  char *rig_filepath;
  char *rb_section;
  char *rig_section;
  unsigned int num_anim;
} rig_asset;

const rig generate_rig(const char *filepath, const char *rb_section, const char *rig_section, const unsigned int num_anim);

rig_builder generate_rig_builder_from_file(const char *filepath, const char *section, const int num_anim);
void generate_rig_from_file(rig *r, const char *filepath, const char *rig_section);

const unsigned int get_rig_id_from_string(const char *area_key);

const rig get_rig(unsigned int rig_id);
const rig get_rig_from_string(const char *rig_key);

void generate_rigs(void);

