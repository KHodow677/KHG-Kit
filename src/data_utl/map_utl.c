#include "data_utl/map_utl.h"
#include "entity/entity.h"
#include "generators/elements/particle_generator.h"
#include <stdio.h>
#include <stdlib.h>

int compare_ints(const key_type a, const key_type b) {
  const int* ia = (const int*)a;
  const int* ib = (const int*)b;
  return (*ia - *ib);
}

void no_deallocator(void *data) {
  (void)0;
}

void free_deallocator(void *data) {
  free(data);
}

void free_entity_deallocator(void *data) {
  generic_entity *ge = data;
  if (generic_entity_is_particle(ge)) {
    free_particle(ge->particle);
  }
  free(data);
}

