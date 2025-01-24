#include "threading/resource_loading.h"
#include "khg_thd/concurrent.h"
#include "resources/ovr_tile_loader.h"
#include "resources/texture_loader.h"
#include "scene/scene_loader.h"
#include <stdio.h>

volatile bool RESOUCES_LOADED = false;
volatile unsigned int TEXTURE_LOAD_PROGRESS = EMPTY_TEXTURE;
volatile unsigned int OVR_TILE_LOAD_PROGRESS = EMPTY_OVR_TILE;

static thd_thread OVR_TILE_THREAD = { 0 };
static bool OVR_TILE_LOADING_STARTED = false;
static bool OVR_TILES_LOADED = false;

static int load_ovr_tiles_task(void *arg) {
  for (unsigned int i = EMPTY_OVR_TILE; i < NUM_OVR_TILES; i++) {
    load_ovr_tile_tick(1);
  }
  return 0; 
}

void load_resources_defer(const unsigned int batch_size) {
  if (!OVR_TILE_LOADING_STARTED) {
    OVR_TILE_LOADING_STARTED = true;
    unsigned int batch = batch_size;
    thd_thread_create(&OVR_TILE_THREAD, (thd_thread_start)load_ovr_tiles_task, &batch);
  }
  if (!OVR_TILES_LOADED && OVR_TILE_LOAD_PROGRESS >= NUM_OVR_TILES) {
    OVR_TILES_LOADED = true;
    thd_thread_join(OVR_TILE_THREAD, NULL);
    printf("JOINED\n");
  }
  load_texture_tick(batch_size);
  if (!RESOUCES_LOADED && TEXTURE_LOAD_PROGRESS == NUM_TEXTURES && OVR_TILE_LOAD_PROGRESS == NUM_OVR_TILES) {
    RESOUCES_LOADED = true;
    setup_scenes();
    printf("DONE\n");
  }
}

