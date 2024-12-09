#include "resources/area_loader.h"
#include "khg_utl/config.h"
#include "tile/area.h"
#include <string.h>

void generate_area_from_files(area *a, const char *tile_filepath, const char *collider_filepath, const char *object_filepath) {
  generate_area_setup(a, tile_filepath, collider_filepath);
  generate_area_tiles_from_file(a, tile_filepath);
  generate_area_colliders_from_file(a, collider_filepath);
  generate_area_objects_from_file(a, object_filepath);
}

void generate_area_setup(area *a, const char *tile_filepath, const char *object_filepath) {
  utl_config_file *tile_config = utl_config_create(tile_filepath);
  utl_config_file *collider_config = utl_config_create(tile_filepath);
  int num_tiles = utl_config_get_int(tile_config, "area_tiles", "num_tiles", 0);
  int num_colliders = utl_config_get_int(collider_config, "area_colliders", "num_colliders", 0);
  create_area(a, num_tiles,  num_colliders);
}

void generate_area_tiles_from_file(area *a, const char *tile_filepath) {
  utl_config_file *config = utl_config_create(tile_filepath);
  utl_config_iterator iterator = utl_config_get_iterator(config);
  const char *section, *key, *value;
  while (utl_config_next_entry(&iterator, &section, &key, &value)) {
    if (strcmp(section, "area_tiles")) {
      continue;
    }
    size_t len = strlen(key);
    char generic_key[len];
    strncpy(generic_key, key, len - 1);
    generic_key[len - 1] = '\0'; 
    if (!strcmp(generic_key, "tile_tex")) {
      continue;
    }
    else if (!strcmp(generic_key, "tile_pos")) {
      continue;
    }
  }
  utl_config_deallocate(config);
}

void generate_area_colliders_from_file(area *a, const char *object_filepath) {

}

void generate_area_objects_from_file(area *a, const char *object_filepath) {

}
