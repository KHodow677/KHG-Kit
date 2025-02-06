#pragma once

#include "khg_gfx/texture.h"
#include "util/ovr_tile.h"
#include <stdbool.h>

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

typedef struct texture_object {
  bool loaded, fetching, uploaded;
  unsigned int id, width, height, channels;
  char name[32], path[128];
  unsigned char *texture_raw;
  gfx_texture texture;
} texture_object;

typedef struct tile_object {
  bool loaded, fetching;
  unsigned int id;
  char name[32], path[128];
  ovr_tile tile;
} tile_object;

#if defined(NAMESPACE_TASKING_IMPL) || defined(NAMESPACE_TASKING_USE)
#include "khg_gfx/texture.h"
#include "khg_utl/config.h"
#include "util/ovr_tile.h"
typedef struct tasking_namespace {
  void (*task_enqueue)(void (*)(void *), void *);
  worker_task (*task_dequeue)(void);
  int (*task_worker)(void *);
  void (*initialize_thread_pool)(void);
  void (*shutdown_thread_pool)(void);
  void (*populate_texture_data)(const char *);
  void (*load_texture_data)(void *);
  gfx_texture (*get_texture_data)(const unsigned int);
  const unsigned int (*get_texture_id)(const char *);
  void (*clear_texture_data)(void);
  void (*populate_tile_data)(const char *);
  void (*load_tile_data)(void *);
  const ovr_tile (*get_tile_data)(const unsigned int);
  const unsigned int (*get_tile_id)(const char *);
  const char *(*get_random_tile_name)(void);
  void (*clear_tile_data)(void);
} tasking_namespace;
#endif

#ifdef NAMESPACE_TASKING_IMPL
extern tasking_namespace NAMESPACE_TASKING_INTERNAL;
#endif

#ifdef NAMESPACE_TASKING_USE
tasking_namespace *NAMESPACE_TASKING(void);
#endif

