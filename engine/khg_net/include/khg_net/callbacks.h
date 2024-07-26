#pragma once

#if defined(_WIN32) || defined(_WIN64)
#include "khg_net/win32.h"
#else
#include "khg_net/unix.h"
#endif

typedef struct _ENetCallbacks {
  void *(ENET_CALLBACK * malloc) (size_t size);
  void (ENET_CALLBACK * free) (void * memory);
  void (ENET_CALLBACK * no_memory) (void);
} ENetCallbacks;

extern void *enet_malloc(size_t);
extern void enet_free(void *);
