#define NAMESPACE_LOADING_IMPL

#include "khg_thd/concurrent.h"
#include "loading/namespace.h"
#include "scene/scene_loader.h"
#include <stdio.h>

static int load_ovr_tiles_task(void *arg) {
  for (unsigned int i = EMPTY_OVR_TILE; i < NUM_OVR_TILES; i++) {
    NAMESPACE_LOADING_INTERNAL.load_ovr_tile_tick(arg);
  }
  return 0; 
}

static int load_textures_raw_task(void *arg) {
  for (unsigned int i = 0; i < NAMESPACE_LOADING_INTERNAL.TEXTURE_RAW_THREAD.max; i++) {
    NAMESPACE_LOADING_INTERNAL.emplace_tex_defs_tick(arg);
  }
  printf("Part2\n");
  return 0; 
}

static bool resource_thread_maxed(loading_resource_thread *resource) {
  return resource->max == resource->progress;
}

void load_configs(const char *tex_filename) {
  NAMESPACE_LOADING_INTERNAL.generate_tex_defs(tex_filename);
  printf("Part1\n");
}

void close_configs() {
  NAMESPACE_LOADING_INTERNAL.free_tex_defs();
}

void load_thread_defer(loading_resource_thread *resource, int (*task)(void *)) {
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
  else if (resource_thread_maxed(&NAMESPACE_LOADING_INTERNAL.OVR_TILE_THREAD) && resource_thread_maxed(&NAMESPACE_LOADING_INTERNAL.TEXTURE_RAW_THREAD) && resource_thread_maxed(&NAMESPACE_LOADING_INTERNAL.TEXTURE_THREAD)) {
    NAMESPACE_LOADING_INTERNAL.RESOURCES_LOADED = true;
    setup_scenes();
    return;
  }
  load_thread_defer(&NAMESPACE_LOADING_INTERNAL.OVR_TILE_THREAD, load_ovr_tiles_task);
  load_thread_defer(&NAMESPACE_LOADING_INTERNAL.TEXTURE_RAW_THREAD, load_textures_raw_task);
  if (resource_thread_maxed(&NAMESPACE_LOADING_INTERNAL.OVR_TILE_THREAD) && resource_thread_maxed(&NAMESPACE_LOADING_INTERNAL.TEXTURE_RAW_THREAD)) {
    load_thread_defer(&NAMESPACE_LOADING_INTERNAL.TEXTURE_THREAD, NAMESPACE_LOADING_INTERNAL.emplace_tex_defs);
  }
}

