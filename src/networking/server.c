#include "networking/server.h"
#include "khg_dbm/dbm.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#else
#include <linux/limits.h>
#endif

typedef struct Human
{
    char name[50];
    int age;
} Human;

bool OPEN = false;

void server_run() {
  MiniDb *db;
  create_or_open_db("player_db", db, sizeof(Human)); 

  int64_t key = 1;
  Human result;
  strncpy(result.name, "John Doe", sizeof(result.name));
  result.age = 20;
  MiniDbState i_state = minidb_insert(db, key, &result);
  if (i_state != MINIDB_OK) {
      printf("Error: %s\n", minidb_error_get_str(i_state));
  }

  minidb_close(&db);
}

MiniDbState create_or_open_db(const char *asset_name, MiniDb *db, size_t size) {
#if defined(_WIN32) || defined(_WIN64)
  char cwd[MAX_PATH];
  _getcwd(cwd, sizeof(cwd));
  size_t cwd_len = strlen(cwd);
  size_t asset_dir_len = cwd_len + strlen("\\res") + 1;
  size_t path_len = asset_dir_len + strlen("\\assets\\db\\") + strlen(asset_name) + strlen(".db") + 1;
  char asset_dir[asset_dir_len];
  char path[path_len];
  snprintf(asset_dir, sizeof(asset_dir), "%s\\res", cwd);
  snprintf(path, sizeof(path), "%s\\assets\\db\\%s.db", asset_dir, asset_name);
#else 
  char cwd[PATH_MAX];
  getcwd(cwd, sizeof(cwd));
  size_t cwd_len = strlen(cwd);
  size_t asset_dir_len = cwd_len + strlen("/res") + 1;
  size_t path_len = asset_dir_len + strlen("/assets/db/") + strlen(asset_name) + strlen(".db") + 1;
  char asset_dir[asset_dir_len];
  char path[path_len];
  snprintf(asset_dir, sizeof(asset_dir), "%s/res", cwd);
  snprintf(path, sizeof(path), "%s/assets/db/%s.db", asset_dir, asset_name);
#endif
  if (access(path, F_OK) == 0) {
    OPEN = true;
    printf("OPEN\n");
    return minidb_open(&db, path);
  } 
  else {
    OPEN = false;
    printf("CREATE\n");
    return minidb_create(&db, path, size);
  }
}

