#include "threading/resource_loading.h"
#include "khg_thd/concurrent.h"
#include "resources/texture_loader.h"
#include <stdio.h>

volatile bool RESOUCES_LOADED = false;
volatile unsigned int TEXTURE_LOAD_PROGRESS = 1;
volatile float OVR_TILE_LOAD_PROGRESS = 0.0f; 

static thd_mutex MUTEX = { 0 };
static bool LOADING_STARTED = false;

static int load_ovr_tiles_task(void *arg) {
  for (unsigned int i = 1; i < NUM_TEXTURES; i++) {
    OVR_TILE_LOAD_PROGRESS = (float)i / (float)NUM_TEXTURES;
    printf("%f\n", OVR_TILE_LOAD_PROGRESS);
  }
  OVR_TILE_LOAD_PROGRESS = 1.0f;
  printf("%f\n", OVR_TILE_LOAD_PROGRESS);
  return 0; 
}

void load_resources_defer(const unsigned int batch_size) {
  if (!LOADING_STARTED) {
    LOADING_STARTED = true;
    /*thd_thread texture_thread;*/
    /*thd_mutex_init(&MUTEX, THD_MUTEX_PLAIN);*/
    /*thd_thread_create(&texture_thread, (thd_thread_start)load_ovr_tiles_task, NULL);*/
    /*thd_thread_join(texture_thread, NULL);*/
    /*thd_mutex_destroy(&MUTEX);*/
  }
  load_texture_tick(batch_size);
}

