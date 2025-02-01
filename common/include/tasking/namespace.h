#pragma once

#include "khg_thd/concurrent.h"
#include <stdbool.h>

typedef struct resource_thread {
  bool enabled;
  thd_thread thread;
  unsigned int progress;
  unsigned int max;
  bool loading_started;
  bool loaded;
} resource_thread;

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

typedef enum ovr_tile_id {
  FOREACH_OVR_TILE(GENERATE_OVR_TILE_ENUM)
} ovr_tile_id;

#define OVR_TILE_STRINGS (char *[]){ FOREACH_OVR_TILE(GENERATE_OVR_TILE_STRING) }
#define OVR_TILE_STRINGS_SIZE sizeof(OVR_TILE_STRINGS) / sizeof(OVR_TILE_STRINGS[0])

typedef struct texture_asset {
  char tex_filepath[128];
  int tex_width;
  int tex_height;
} texture_asset;

typedef struct texture_raw_info {
  unsigned char *tex_raw;
  int width;
  int height;
  int channels;
} texture_raw_info;

typedef struct ovr_tile_asset {
  char *ovr_tile_filepath;
} ovr_tile_asset;

typedef struct worker_task {
  void (*function)(void *);
  void *arg;
} worker_task;

#if defined(NAMESPACE_TASKING_IMPL) || defined(NAMESPACE_TASKING_USE)
#include "khg_gfx/texture.h"
#include "khg_utl/config.h"
#include "util/ovr_tile.h"
typedef struct tasking_namespace {
  void (*load_configs)(const char *);
  void (*close_config)(void);
  void (*load_thread_defer)(resource_thread *, int (*)(void *), resource_thread *);
  void (*load_resources_defer)(void);
  const unsigned int (*get_ovr_tile_id_from_string)(const char *);
  const ovr_tile (*get_ovr_tile)(unsigned int);
  const ovr_tile (*get_ovr_tile_from_string)(const char *);
  void (*generate_ovr_tiles)(void);
  int (*load_ovr_tile_tick)(void *);
  void (*generate_tex_defs)(const char *);
  void (*emplace_tex_defs_tick)(void *);
  int (*emplace_tex_defs)(void *);
  gfx_texture (*get_tex_def_by_location)(unsigned int);
  gfx_texture (*get_tex_def)(char *);
  unsigned int (*get_location_tex_str)(const char *);
  void (*free_tex_defs)(void);
  bool RESOURCES_LOADED;
  resource_thread OVR_TILE_THREAD;
  resource_thread TEXTURE_ASSET_THREAD;
  resource_thread TEXTURE_RAW_THREAD;
  resource_thread TEXTURE_THREAD;
} tasking_namespace;
#endif

#ifdef NAMESPACE_TASKING_IMPL
extern tasking_namespace NAMESPACE_TASKING_INTERNAL;
#endif

#ifdef NAMESPACE_TASKING_USE
tasking_namespace *NAMESPACE_TASKING(void);
#endif

