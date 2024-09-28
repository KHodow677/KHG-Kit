#include "generators/components/map_generator.h"
#include "game_manager.h"
#include "khg_csv/csv.h"
#include "khg_utl/vector.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int get_csv_rows(char *path) {
  int rows = 0;
  char *row;
  csv_handle handle = csv_open(path);
  while ((row = csv_read_next_row(handle))) {
    rows++;
  }
  csv_close(handle);
  return rows;
}

int get_csv_cols(char *path) {
  int cols = 0;
  char *row, *col;
  csv_handle handle = csv_open(path);
  row = csv_read_next_row(handle);
  while ((col = csv_read_next_col(row, handle))) {
    cols++;
  }
  csv_close(handle);
  return cols;
}

void load_map(const char *filepath, utl_vector **map) {
#if defined(_WIN32) || defined(_WIN64)
  char cwd[MAX_PATH];
  _getcwd(cwd, sizeof(cwd));
  size_t cwd_len = strlen(cwd);
  size_t asset_dir_len = cwd_len + strlen("\\res") + 1;
  size_t path_len = asset_dir_len + strlen("\\assets\\maps\\") + strlen(filepath) + strlen(".csv") + 1;
  char asset_dir[asset_dir_len];
  char path[path_len];
  snprintf(asset_dir, sizeof(asset_dir), "%s\\res", cwd);
  snprintf(path, sizeof(path), "%s\\assets\\maps\\%s.csv", asset_dir, filepath);
#else
  char cwd[MAX_PATH];
  getcwd(cwd, sizeof(cwd));
  size_t cwd_len = strlen(cwd);
  size_t asset_dir_len = cwd_len + strlen("/res") + 1;
  size_t path_len = asset_dir_len + strlen("/assets/maps/") + strlen(filepath) + strlen(".csv") + 1;
  char asset_dir[asset_dir_len];
  char path[path_len];
  snprintf(asset_dir, sizeof(asset_dir), "%s/res", cwd);
  snprintf(path, sizeof(path), "%s/assets/maps/%s.csv", asset_dir, filepath);
#endif
  char *row;
  csv_handle handle = csv_open(path);
  GAME_MAP_HEIGHT = get_csv_rows(path);
  GAME_MAP_WIDTH = get_csv_cols(path);
  *map = utl_vector_create(sizeof(utl_vector *));
  while ((row = csv_read_next_row(handle))) {
    char *col;
    utl_vector *int_row = utl_vector_create(sizeof(int));
    utl_vector_push_back(*map, &int_row);
    while ((col = csv_read_next_col(row, handle))) {
      int val = atoi(col);
      utl_vector_push_back(int_row, &val);
    }
  }
  csv_close(handle);
}

void free_map(utl_vector **map) {
  for (int i = 0; i < utl_vector_size(*map); i++) {
    utl_vector **row =  utl_vector_at(*map, i);
    utl_vector_deallocate(*row);
  }
  utl_vector_deallocate(*map);
}

