#include "entity/map.h"
#include "game_manager.h"
#include "generators/components/texture_generator.h"
#include "physics/physics_setup.h"
#include "khg_gfx/elements.h"
#include "khg_phy/phy_types.h"
#include "khg_phy/vect.h"
#include "khg_utl/vector.h"

void render_map(utl_vector *map) {
  int map_rows = utl_vector_size(map);
  for (int i = 0; i < map_rows; i++) {
    utl_vector **row =  utl_vector_at(map, i);
    int map_cols = utl_vector_size(*row);
    for (int j = 0; j < map_cols; j++) { 
      int *item = utl_vector_at(*row, j);
      if (*item == -1) {
        continue;
      }
      gfx_texture *tex = get_or_add_texture(*item);
      int half_map_width = 0.5 * GAME_MAP_WIDTH;
      int half_map_height = 0.5 * GAME_MAP_HEIGHT;
      int tile_pos_x = (j + 0.5f - half_map_width) * GAME_MAP_TILE_SIZE - 4;
      int tile_pos_y = (i + 0.5f - half_map_height) * GAME_MAP_TILE_SIZE - 4;
      gfx_image_no_block(tile_pos_x, tile_pos_y, *tex, 0.0f, 0.0f, CAMERA.position.x, CAMERA.position.y, CAMERA.zoom, true);
    }
  }
}

void add_map_collision_segments(utl_vector *map, utl_vector **segments) {
  *segments = utl_vector_create(sizeof(phy_shape *));
  int map_rows = utl_vector_size(map);
  for (int i = 0; i < map_rows; i++) {
    utl_vector **row =  utl_vector_at(map, i);
    int map_cols = utl_vector_size(*row);
    for (int j = 0; j < map_cols; j++) { 
      int *item = utl_vector_at(*row, j);
      if (*item == -1) {
        continue;
      }
      gfx_texture *tex = get_or_add_texture(*item);
      int collision_dir = TEXTURE_ASSET_REF[*item].collision_direction;
      int half_map_width = 0.5 * GAME_MAP_WIDTH;
      int half_map_height = 0.5 * GAME_MAP_HEIGHT;
      phy_vect tile_pos_top_left = { (j - half_map_width) * GAME_MAP_TILE_SIZE, (i - half_map_height) * GAME_MAP_TILE_SIZE};
      phy_vect tile_pos_top_right = phy_v_add(tile_pos_top_left, phy_v(GAME_MAP_TILE_SIZE, 0.0f));
      phy_vect tile_pos_bottom_left = phy_v_add(tile_pos_top_left, phy_v(0.0f, GAME_MAP_TILE_SIZE));
      phy_vect tile_pos_bottom_right = phy_v_add(tile_pos_top_left, phy_v(GAME_MAP_TILE_SIZE, GAME_MAP_TILE_SIZE));
      if ((collision_dir == SEGMENT_TOP_LEFT || collision_dir == SEGMENT_TOP || collision_dir == SEGMENT_TOP_RIGHT)) {
        phy_shape *seg = physics_add_static_segment_shape(SPACE, tile_pos_top_left, tile_pos_top_right);
        utl_vector_push_back(*segments, &seg);
      }
      if ((collision_dir == SEGMENT_TOP_LEFT || collision_dir == SEGMENT_LEFT || collision_dir == SEGMENT_BOTTOM_LEFT)) {
        phy_shape *seg = physics_add_static_segment_shape(SPACE, tile_pos_top_left, tile_pos_bottom_left);
        utl_vector_push_back(*segments, &seg);
      }
      if ((collision_dir == SEGMENT_TOP_RIGHT || collision_dir == SEGMENT_RIGHT || collision_dir == SEGMENT_BOTTOM_RIGHT)) {
        phy_shape *seg = physics_add_static_segment_shape(SPACE, tile_pos_top_right, tile_pos_bottom_right);
        utl_vector_push_back(*segments, &seg);
      }
      if ((collision_dir == SEGMENT_BOTTOM_LEFT || collision_dir == SEGMENT_BOTTOM || collision_dir == SEGMENT_BOTTOM_RIGHT)) {
        phy_shape *seg = physics_add_static_segment_shape(SPACE, tile_pos_bottom_left, tile_pos_bottom_right);
        utl_vector_push_back(*segments, &seg);
      }
    }
  }
}

void free_map_collision_segments(utl_vector **segments) {
  for (int i = 0; i < utl_vector_size(*segments); i++) {
    phy_shape **seg = utl_vector_at(*segments, i);
    physics_remove_static_segment_shape(SPACE, *seg);
  }
  utl_vector_deallocate(*segments);
}

