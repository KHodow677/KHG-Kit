#include "tile/area.h"
#include "khg_utl/array.h"
#include "khg_utl/vector.h"
#include <stddef.h>

void create_area(area *a, size_t num_tiles, size_t num_colliders, int tiles_layer, int objects_layer) {
  a->tiles = utl_array_create(sizeof(area_tile), num_tiles);
  a->colliders = utl_array_create(sizeof(area_collider), num_tiles);
  a->objects = utl_vector_create(sizeof(area_object));
  a->tiles_layer = tiles_layer;
  a->objects_layer = objects_layer;
  a->loaded = true;
}

void free_area(area *a) {
  utl_array_deallocate(a->tiles);
  utl_array_deallocate(a->colliders);
  utl_vector_deallocate(a->objects);
}

