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
  printf("CREATE TABLE\n");
  dbm_interactive(&PLAYER_DB, "Create Table EmployeeDemographics (EmployeeID int, FirstName varchar(50), LastName varchar(50), Age int, Gender varchar(50))");
  dbm_interactive(&PLAYER_DB, "Insert into EmployeeDemographics VALUES (1001, 'Jim', 'Halpert', 30, 'Male'), (1002, 'Pam', 'Beasley', 30, 'Female'), (1003, 'Dwight', 'Schrute', 29, 'Male'), (1004, 'Angela', 'Martin', 31, 'Female'), (1005, 'Toby', 'Flenderson', 32, 'Male'), (1006, 'Michael', 'Scott', 35, 'Male'), (1007, 'Meredith', 'Palmer', 32, 'Female'), (1008, 'Stanley', 'Hudson', 38, 'Male'), (1009, 'Kevin', 'Malone', 31, 'Male')");
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
