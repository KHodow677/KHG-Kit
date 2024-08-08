#include "data_utl/map_utl.h"

int compare_ints(const key_type a, const key_type b) {
  const int* ia = (const int*)a;
  const int* ib = (const int*)b;
  return (*ia - *ib);
}

void no_deallocator(void *data) {
  (void)0;
}
