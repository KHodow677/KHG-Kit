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

typedef struct player {
  char name[50];
  int age;
} player;

void server_run() {
  MiniDb *db;
  create_or_open_db("server_player", &db, sizeof(player)); 
  int64_t key = 1;
  player result = {"John Doe", 20 };
  minidb_insert(db, key, &result);
  player select_result;
  minidb_select(db, 1, &select_result);
  printf("HUMAN:\nname: %s\nage: %i \n", select_result.name, select_result.age);
  minidb_close(&db);
}

MiniDbState create_or_open_db(const char *asset_name, MiniDb **db, size_t size) {
#if defined(_WIN32) || defined(_WIN64)
  size_t path_len = strlen(".\\res\\assets\\db\\") + strlen(asset_name) + strlen(".db") + 1;
  char path[path_len];
  snprintf(path, sizeof(path), ".\\res\\assets\\db\\%s.db", asset_name);
#else 
  size_t path_len = strlen("./res/assets/db/") + strlen(asset_name) + strlen(".db") + 1;
  char path[path_len];
  snprintf(path, sizeof(path), "./res/assets/db/%s.db", asset_name);
#endif
  if (access(path, F_OK) == 0) {
    return minidb_open(db, path);
  } 
  else {
    return minidb_create(db, path, size);
  }
}

