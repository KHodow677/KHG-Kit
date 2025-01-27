#define NAMESPACE_LOADING_IMPL 
#include "loading/namespace.h"

int mod1_mult2 (int x) {
  return x << 1;
}

int mod1_mult3 (int x) {
  return x * 3;
}

loading_namespace *NAMESPACE_LOADING(void) {
  static loading_namespace ret = {
    .mult2 = mod1_mult2,
    .mult3 = mod1_mult3,
  };
  return &ret;
}
