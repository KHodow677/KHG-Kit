#define NAMESPACE_TASKING_IMPL

#include "khg_thd/concurrent.h"
#include "scene/scene_loader.h"
#include "tasking/namespace.h"
#include <stdlib.h>

static char *TEX_CONFIG_FILENAME;

static int load_ovr_tiles_task(void *arg) {
  for (unsigned int i = EMPTY_OVR_TILE; i < NUM_OVR_TILES; i++) {
    NAMESPACE_TASKING_INTERNAL.load_ovr_tile_tick(arg);
  }
  return 0; 
}

static bool resource_thread_maxed(resource_thread *resource) {
  return resource->max == resource->progress;
}

void load_thread_defer(resource_thread *resource, int (*task)(void *), resource_thread *await) {
  if (await && !resource_thread_maxed(await)) {
    return;
  }
  if (!resource->enabled && !resource->loaded) {
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
  if (NAMESPACE_TASKING_INTERNAL.RESOURCES_LOADED) {
    return;
  }
  else if (resource_thread_maxed(&NAMESPACE_TASKING_INTERNAL.OVR_TILE_THREAD)) {
    NAMESPACE_TASKING_INTERNAL.RESOURCES_LOADED = true;
    setup_scenes();
    return;
  }
  load_thread_defer(&NAMESPACE_TASKING_INTERNAL.OVR_TILE_THREAD, load_ovr_tiles_task, NULL);
}

