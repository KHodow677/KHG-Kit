#pragma once

#include "tile/area.h"

void generate_area_from_files(area *a, const char *tile_filepath, const char *collider_filepath, const char *object_filepath);

void generate_area_setup(area *a, const char *tile_filepath, const char *object_filepath);

void generate_area_tiles_from_file(area *a, const char *tile_filepath);
void generate_area_colliders_from_file(area *a, const char *collider_filepath);
void generate_area_objects_from_file(area *a, const char *object_filepath);
