#include "entity/map.h"
#include "game_manager.h"
#include "generators/components/texture_generator.h"
#include "khg_gfx/elements.h"
#include "khg_utl/vector.h"

void render_map() {
  int map_rows = utl_vector_size(GAME_MAP);
  for (int i = 0; i < map_rows; i++) {
    utl_vector **row =  utl_vector_at(GAME_MAP, i);
    int map_cols = utl_vector_size(*row);
    for (int j = 0; j < map_cols; j++) { 
      int *item = utl_vector_at(*row, j);
      gfx_texture *tex = get_or_add_texture(*item);
      gfx_image_no_block((j + 0.5f) * GAME_MAP_TILE_SIZE, (i + 0.5f) * GAME_MAP_TILE_SIZE, *tex, 0.0f, 0.0f, CAMERA.position.x, CAMERA.position.y, CAMERA.zoom);
    }
  }
}

