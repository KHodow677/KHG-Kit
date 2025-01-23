#include "threading/resource_loading.h"
#include "khg_thd/concurrent.h"
#include "resources/ovr_tile_loader.h"
#include "resources/texture_loader.h"

volatile bool RESOUCES_LOADED = false;
volatile unsigned int TEXTURE_LOAD_PROGRESS = SQUARE + 1;
volatile float OVR_TILE_LOAD_PROGRESS = 0.0f; 

static bool LOADING_STARTED = false;

static thd_thread OVR_TILE_THREAD;

static int load_ovr_tiles_task(void *arg) {
  for (unsigned int i = 1; i < NUM_OVR_TILES; i++) {
    OVR_TILE_LOAD_PROGRESS = (float)i / (float)NUM_OVR_TILES;
    // printf("%f\n", OVR_TILE_LOAD_PROGRESS);
  }
  OVR_TILE_LOAD_PROGRESS = 1.0f;
  // printf("%f\n", OVR_TILE_LOAD_PROGRESS);
  return 0; 
}

void load_resources_defer(const unsigned int batch_size) {
  // if (!LOADING_STARTED) {
  //   LOADING_STARTED = true;
  //   thd_thread_create(&OVR_TILE_THREAD, (thd_thread_start)load_ovr_tiles_task, NULL);
  // }
  // if (OVR_TILE_LOAD_PROGRESS >= NUM_OVR_TILES) {
  //   printf("JOINED\n");
  //   thd_thread_join(OVR_TILE_THREAD, NULL);
  // }
  load_texture_tick(batch_size);
  if (TEXTURE_LOAD_PROGRESS == NUM_TEXTURES) {
    RESOUCES_LOADED = true;
  }
}
