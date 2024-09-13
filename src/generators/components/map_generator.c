#include "generators/components/map_generator.h"
#include "khg_csv/csv.h"
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

int load_map(const char *filepath) {
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
  int elements = 0;
  csv_handle handle = csv_open(path);
  while ((row = csv_read_next_row(handle))) {
    char *col;
    while ((col = csv_read_next_col(row, handle))) {
      printf("%s, ", col);
      elements++;
    }
    printf("\n");
  }
  int csv_row = get_csv_rows(path);
  int csv_col = get_csv_cols(path);
  printf("Rows: %i, Cols: %i\n", csv_row, csv_col);
  printf("# of Elements = %i\n", elements);
  csv_close(handle);
  return 1;
}
