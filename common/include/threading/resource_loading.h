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

extern bool RESOUCES_LOADED;

extern resource_thread OVR_TILE_THREAD;
extern resource_thread TEXTURE_RAW_THREAD;
extern resource_thread TEXTURE_THREAD;

void load_thread_defer(resource_thread *resource, int (*task)(void *));
void load_resources_defer(void);

