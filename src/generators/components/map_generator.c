#include "generators/components/map_generator.h"
#include "khg_csv/csv.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int NUM_LINES = 3;
int MAX_LENGTH = 1024;

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
  char cwd[PATH_MAX];
  getcwd(cwd, sizeof(cwd));
  size_t cwd_len = strlen(cwd);
  size_t asset_dir_len = cwd_len + strlen("/res") + 1;
  size_t path_len = asset_dir_len + strlen("/assets/maps/") + strlen(filepath) + strlen(".csv") + 1;
  char asset_dir[asset_dir_len];
  char path[path_len];
  snprintf(asset_dir, sizeof(asset_dir), "%s/res", cwd);
  snprintf(path, sizeof(path), "%s/assets/maps/%s.csv", asset_dir, filepath);
#endif
  FILE *file = fopen(path, "rb");
  char *map[NUM_LINES];
  for (int i = 0; i < NUM_LINES; i++) {
    int done, error;
    char *line = read_csv_line(file, MAX_LENGTH, &done, &error);
    map[i] = line;
  }
  for (int i = 0; i < NUM_LINES; i++) {
    printf("%s\n", map[i]);
  }
  return 1;
}
