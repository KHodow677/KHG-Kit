#pragma once

#if defined(_WIN32) || defined(_WIN64)
#include "khg_net/win32.h"
#else
#include "khg_net/unix.h"
#endif

typedef struct net_callbacks {
  void *(NET_CALLBACK * malloc) (size_t size);
  void (NET_CALLBACK * free) (void * memory);
  void (NET_CALLBACK * no_memory) (void);
} net_callbacks;

extern void *net_malloc(size_t);
extern void net_free(void *);
