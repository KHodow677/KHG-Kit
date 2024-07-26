#include "khg_net/net.h"
#include <stdlib.h>

static net_callbacks callbacks = { malloc, free, abort };

int enet_initialize_with_callbacks(ENetVersion version, const net_callbacks *inits) {
  if (version < ENET_VERSION_CREATE(1, 3, 0)) {
    return -1;
  }
  if (inits -> malloc != NULL || inits -> free != NULL) {
    if (inits -> malloc == NULL || inits -> free == NULL) {
      return -1;
    }
    callbacks.malloc = inits -> malloc;
    callbacks.free = inits -> free;
  }
  if (inits -> no_memory != NULL) {
    callbacks.no_memory = inits -> no_memory;
  }
  return enet_initialize();
}

ENetVersion enet_linked_version() {
  return ENET_VERSION;
}
           
void *net_malloc(size_t size) {
  void *memory = callbacks.malloc (size);
  if (memory == NULL) {
    callbacks.no_memory();
  }
  return memory;
}

void net_free(void *memory) {
  callbacks.free(memory);
}
