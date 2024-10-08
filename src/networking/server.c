#include "networking/server.h"
#include "khg_dbm/database.h"
#include "khg_dbm/parser.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

dbm_database *PLAYER_DB = NULL;

void server_run() {
  PLAYER_DB = dbm_create_database("player_db");
  load_db("player_db");
  dbm_interactive(&PLAYER_DB, "CREATE TABLE abc FIELDS (Field1=int, Field2=str)");
  dbm_interactive(&PLAYER_DB, "INSERT INTO abc (Field1=19,Field2='Hello World')");
  dbm_interactive(&PLAYER_DB, "INSERT INTO abc (Field1=20,Field2='Hello World')");
  dbm_interactive(&PLAYER_DB, "INSERT INTO abc (Field1=21,Field2='Hello World')");
  dbm_interactive(&PLAYER_DB, "SELECT * FROM abc WHERE Field1=19");
  dbm_interactive(&PLAYER_DB, "UPDATE abc SET Field1=20 WHERE Field2='Hello World'");
  dbm_interactive(&PLAYER_DB, "SELECT * FROM abc WHERE Field1=20");
  dbm_interactive(&PLAYER_DB, "DELETE FROM abc where Field1=19");
  save_db("player_db");
}

void load_db(const char *asset_name) {
#if defined(_WIN32) || defined(_WIN64)
  char cwd[MAX_PATH];
  _getcwd(cwd, sizeof(cwd));
  size_t cwd_len = strlen(cwd);
  size_t asset_dir_len = cwd_len + strlen("\\res") + 1;
  size_t meta_path_len = asset_dir_len + strlen("\\assets\\db\\") + strlen(asset_name) + strlen(".meta") + 1;
  size_t db_path_len = asset_dir_len + strlen("\\assets\\db\\") + strlen(asset_name) + strlen(".db") + 1;
  char asset_dir[asset_dir_len];
  char meta_path[meta_path_len];
  char db_path[db_path_len];
  snprintf(asset_dir, sizeof(asset_dir), "%s\\res", cwd);
  snprintf(meta_path, sizeof(meta_path), "%s\\assets\\db\\%s.%s", asset_dir, asset_name, "meta");
  snprintf(db_path, sizeof(db_path), "%s\\assets\\db\\%s.%s", asset_dir, asset_name, "db");
  dbm_read_files(&PLAYER_DB, meta_path, db_path);
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
  dbm_read_files(&PLAYER_DB, meta_path, db_path);
#endif
}

void save_db(const char *asset_name) {
#if defined(_WIN32) || defined(_WIN64)
  char cwd[MAX_PATH];
  _getcwd(cwd, sizeof(cwd));
  size_t cwd_len = strlen(cwd);
  size_t asset_dir_len = cwd_len + strlen("\\res") + 1;
  size_t meta_path_len = asset_dir_len + strlen("\\assets\\db\\") + strlen(asset_name) + strlen(".meta") + 1;
  size_t db_path_len = asset_dir_len + strlen("\\assets\\db\\") + strlen(asset_name) + strlen(".db") + 1;
  char asset_dir[asset_dir_len];
  char meta_path[meta_path_len];
  char db_path[db_path_len];
  snprintf(asset_dir, sizeof(asset_dir), "%s\\res", cwd);
  snprintf(meta_path, sizeof(meta_path), "%s\\assets\\db\\%s.%s", asset_dir, asset_name, "meta");
  snprintf(db_path, sizeof(db_path), "%s\\assets\\db\\%s.%s", asset_dir, asset_name, "db");
  dbm_write_files(&PLAYER_DB, meta_path, db_path);
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
