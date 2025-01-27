#include "khg_thd/concurrent.h"
#include "loading/resource_loading.h"
#include "resources/ovr_tile_loader.h"
#include "resources/texture_loader.h"
#include "scene/scene_loader.h"

bool RESOUCES_LOADED = false;
resource_thread OVR_TILE_THREAD = { .enabled = true, .max = NUM_OVR_TILES };
resource_thread TEXTURE_RAW_THREAD = { .enabled = true, .max = NUM_TEXTURES };
resource_thread TEXTURE_THREAD = { .enabled = false, .max = NUM_TEXTURES };

static int load_ovr_tiles_task(void *arg) {
  for (unsigned int i = EMPTY_OVR_TILE; i < NUM_OVR_TILES; i++) {
    load_ovr_tile_tick(arg);
  }
  return 0; 
}

static int load_textures_raw_task(void *arg) {
  for (unsigned int i = EMPTY_TEXTURE; i < NUM_TEXTURES; i++) {
    load_texture_raw_tick(arg);
  }
  return 0; 
}

bool check_thread_maxed(resource_thread *resource) {
  return resource->max == resource->progress;
}

void load_thread_defer(resource_thread *resource, int (*task)(void *)) {
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
  if (RESOUCES_LOADED) {
    return;
  }
  else if (check_thread_maxed(&OVR_TILE_THREAD) && check_thread_maxed(&TEXTURE_RAW_THREAD) && check_thread_maxed(&TEXTURE_THREAD)) {
    RESOUCES_LOADED = true;
    setup_scenes();
    return;
  }
  load_thread_defer(&OVR_TILE_THREAD, load_ovr_tiles_task);
  load_thread_defer(&TEXTURE_RAW_THREAD, load_textures_raw_task);
  if (check_thread_maxed(&OVR_TILE_THREAD) && check_thread_maxed(&TEXTURE_RAW_THREAD)) {
    load_thread_defer(&TEXTURE_THREAD, load_texture_tick);
  }
}

