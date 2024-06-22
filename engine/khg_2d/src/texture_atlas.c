#include "khg_2d/texture_atlas.h"

vec4 compute_texture_atlas(int x_count, int y_count, int x, int y, bool flip) {
  float x_size = 1.0f / x_count;
  float y_size = 1.0f / y_count;
  vec4 unflipped_vec = { x * x_size, 1 - (y * y_size), (x + 1) * x_size, 1.f - ((y + 1) * y_size) }; 
  vec4 flipped_vec = { unflipped_vec.z, unflipped_vec.y, unflipped_vec.x, unflipped_vec.w };
  if (flip) {
    return flipped_vec;
  }
  else {
    return unflipped_vec;
  }
}

vec4 compute_texture_atlas_with_padding(int map_x_size, int map_y_size, int x_count, int y_count, int x, int y, bool flip) {
  float x_size = 1.0f / x_count;
  float y_size = 1.0f / y_count;
  float x_padding = 1.0f / map_x_size;
  float y_padding = 1.0f / map_y_size;
  vec4 unflipped_vec = { x * x_size + x_padding, 1 - (y * y_size) - y_padding, (x + 1) * x_size - x_padding, 1.0f - ((y + 1) * y_size) + y_padding };
  vec4 flipped_vec = { unflipped_vec.z, unflipped_vec.y, unflipped_vec.x, unflipped_vec.w };
  if (flip) {
    return flipped_vec;
  }
  else {
    return unflipped_vec;
  }
}

vec4 get_texture_atlas(texture_atlas *ta, int x, int y, bool flip) {
  return compute_texture_atlas(ta->x_count, ta->y_count, x, y, flip);
}
