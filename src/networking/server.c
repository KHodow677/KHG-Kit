#include "networking/server.h"
#include "khg_dbm/database.h"
#include <string.h>
#include <unistd.h>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#else 
#include <linux/limits.h>
#include <sys/stat.h>
#endif

dbm_database *PLAYER_DB = NULL;

void server_run() {
  PLAYER_DB = dbm_create_database("player_db");
  load_db("player_db");
  save_db("player_db");
}

int file_exists (char *filename) {
  struct stat buffer;
  return (stat(filename, &buffer) == 0);
}

void load_db(const char *asset_name) {
#if defined(_WIN32) || defined(_WIN64)
  char cwd[MAX_PATH];
  _getcwd(cwd, sizeof(cwd));
  size_t cwd_len = strlen(cwd);
  size_t asset_dir_len = cwd_len + strlen("\\res") + 1;
  size_t path_len = asset_dir_len + strlen("\\assets\\fonts\\") + strlen(asset_name) + strlen(".") + strlen(file_extension) + 1;
  char asset_dir[asset_dir_len];
  char path[path_len];
  snprintf(asset_dir, sizeof(asset_dir), "%s\\res", cwd);
  snprintf(path, sizeof(path), "%s\\assets\\fonts\\%s.%s", asset_dir, asset_name, file_extension);
  return gfx_load_font(path, font_size);
#else 
  char cwd[PATH_MAX];
  getcwd(cwd, sizeof(cwd));
  size_t cwd_len = strlen(cwd);
  size_t asset_dir_len = cwd_len + strlen("/res") + 1;
  size_t meta_path_len = asset_dir_len + strlen("/assets/db/") + strlen(asset_name) + strlen(".meta") + 1;
  size_t db_path_len = asset_dir_len + strlen("/assets/db/") + strlen(asset_name) + strlen(".db") + 1;
  char asset_dir[asset_dir_len];
  char meta_path[meta_path_len];
  char db_path[db_path_len];
  snprintf(asset_dir, sizeof(asset_dir), "%s/res", cwd);
  snprintf(meta_path, sizeof(meta_path), "%s/assets/db/%s.%s", asset_dir, asset_name, "meta");
  snprintf(db_path, sizeof(db_path), "%s/assets/db/%s.%s", asset_dir, asset_name, "db");
  if (file_exists(meta_path) && file_exists(db_path)) {
    dbm_read_files(&PLAYER_DB, meta_path, db_path);
  }
#endif
}

void save_db(const char *asset_name) {
#if defined(_WIN32) || defined(_WIN64)
  char cwd[MAX_PATH];
  _getcwd(cwd, sizeof(cwd));
  size_t cwd_len = strlen(cwd);
  size_t asset_dir_len = cwd_len + strlen("\\res") + 1;
  size_t path_len = asset_dir_len + strlen("\\assets\\fonts\\") + strlen(asset_name) + strlen(".") + strlen(file_extension) + 1;
  char asset_dir[asset_dir_len];
  char path[path_len];
  snprintf(asset_dir, sizeof(asset_dir), "%s\\res", cwd);
  snprintf(path, sizeof(path), "%s\\assets\\fonts\\%s.%s", asset_dir, asset_name, file_extension);
  return gfx_load_font(path, font_size);
#else 
  char cwd[PATH_MAX];
  getcwd(cwd, sizeof(cwd));
  size_t cwd_len = strlen(cwd);
  size_t asset_dir_len = cwd_len + strlen("/res") + 1;
  size_t meta_path_len = asset_dir_len + strlen("/assets/db/") + strlen(asset_name) + strlen(".meta") + 1;
  size_t db_path_len = asset_dir_len + strlen("/assets/db/") + strlen(asset_name) + strlen(".db") + 1;
  char asset_dir[asset_dir_len];
  char meta_path[meta_path_len];
  char db_path[db_path_len];
  snprintf(asset_dir, sizeof(asset_dir), "%s/res", cwd);
  snprintf(meta_path, sizeof(meta_path), "%s/assets/db/%s.%s", asset_dir, asset_name, "meta");
  snprintf(db_path, sizeof(db_path), "%s/assets/db/%s.%s", asset_dir, asset_name, "db");
  dbm_write_files(&PLAYER_DB, meta_path, db_path);
#endif
}

