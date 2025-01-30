#include "khg_utl/string.h"
#define NAMESPACE_LOADING_IMPL

#include "khg_thd/concurrent.h"
#include "loading/namespace.h"
#include "scene/scene_loader.h"

static char *TEX_CONFIG_FILENAME;

static int load_ovr_tiles_task(void *arg) {
  for (unsigned int i = EMPTY_OVR_TILE; i < NUM_OVR_TILES; i++) {
    NAMESPACE_LOADING_INTERNAL.load_ovr_tile_tick(arg);
  }
  return 0; 
}

static int load_textures_asset_task(void *arg) {
  NAMESPACE_LOADING_INTERNAL.generate_tex_defs(TEX_CONFIG_FILENAME);
  NAMESPACE_LOADING_INTERNAL.TEXTURE_ASSET_THREAD.progress = NAMESPACE_LOADING_INTERNAL.TEXTURE_ASSET_THREAD.max;
  return 0; 
}

static int load_textures_raw_task(void *arg) {
  for (unsigned int i = 0; i < NAMESPACE_LOADING_INTERNAL.TEXTURE_RAW_THREAD.max; i++) {
    NAMESPACE_LOADING_INTERNAL.emplace_tex_defs_tick(arg);
  }
  return 0; 
}

static bool resource_thread_maxed(loading_resource_thread *resource) {
  return resource->max == resource->progress;
}

void load_configs(const char *tex_filename) {
  TEX_CONFIG_FILENAME = utl_string_strdup(tex_filename);
}

void close_configs() {
  NAMESPACE_LOADING_INTERNAL.free_tex_defs();
  free(TEX_CONFIG_FILENAME);
}

void load_thread_defer(loading_resource_thread *resource, int (*task)(void *), loading_resource_thread *await) {
  if (await && !resource_thread_maxed(await)) {
    return;
  }
  if (!resource->enabled) {
    task(resource);
    resource->loading_started = true;
  }
  if (!resource->loading_started) {
    resource->loading_started = true;
    thd_thread_create(&resource->thread, (thd_thread_start)task, resource);
  }
  if (!resource->loaded && resource->progress >= resource->max) {
    resource->loaded = true;
    if (resource->enabled) {
      thd_thread_join(resource->thread, NULL);
    }
  }
}

void load_resources_defer() {
  if (NAMESPACE_LOADING_INTERNAL.RESOURCES_LOADED) {
    return;
  }
  else if (resource_thread_maxed(&NAMESPACE_LOADING_INTERNAL.OVR_TILE_THREAD) && resource_thread_maxed(&NAMESPACE_LOADING_INTERNAL.TEXTURE_THREAD)) {
    NAMESPACE_LOADING_INTERNAL.RESOURCES_LOADED = true;
    setup_scenes();
    return;
  }
  load_thread_defer(&NAMESPACE_LOADING_INTERNAL.TEXTURE_ASSET_THREAD, load_textures_asset_task, NULL);
  load_thread_defer(&NAMESPACE_LOADING_INTERNAL.OVR_TILE_THREAD, load_ovr_tiles_task, &NAMESPACE_LOADING_INTERNAL.TEXTURE_ASSET_THREAD);
  load_thread_defer(&NAMESPACE_LOADING_INTERNAL.TEXTURE_RAW_THREAD, load_textures_raw_task, &NAMESPACE_LOADING_INTERNAL.TEXTURE_ASSET_THREAD);
  load_thread_defer(&NAMESPACE_LOADING_INTERNAL.TEXTURE_THREAD, NAMESPACE_LOADING_INTERNAL.emplace_tex_defs, &NAMESPACE_LOADING_INTERNAL.TEXTURE_RAW_THREAD);
}
