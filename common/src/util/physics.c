#define NAMESPACE_KIN_USE

#include "khg_kin/namespace.h"
#include "util/physics.h"
#include <stddef.h>

kin_engine ENGINE = NULL;

void physics_setup() {
  ENGINE = NAMESPACE_KIN()->engine_create();
}

void physics_cleanup() {
  NAMESPACE_KIN()->engine_deallocate(&ENGINE);
}

