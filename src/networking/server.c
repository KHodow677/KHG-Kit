#include "networking/server.h"
#include "networking/processing.h"
#include "khg_dbm/dbm.h"
#include "khg_tcp/tcp.h"
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
  tcp_init();
  tcp_server *server = tcp_open_server("localhost", "3000", 10);
  while (1) {
    tcp_channel *channel = tcp_accept(server, 0);
    if (channel) {
      while (1) {
        char request_buffer[1024];
        if (tcp_receive(channel, request_buffer, 1024, 500)) {
          char *response_buffer = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n";
          tcp_send(channel, response_buffer, strlen(response_buffer), 500);
          break;
        }
      }
      tcp_close_channel(channel);
    }
  }
  tcp_term();
  /*dbm_db *db;*/
  /*create_or_open_db("server_player", &db, sizeof(player)); */
  /*int64_t key = 1;*/
  /*player result = {"John Doe", 20 };*/
  /*dbm_insert(db, key, &result);*/
  /*player select_result;*/
  /*dbm_select(db, 1, &select_result);*/
  /*printf("HUMAN:\nname: %s\nage: %i \n", select_result.name, select_result.age);*/
  /*dbm_close(&db);*/
}

dbm_db_state create_or_open_db(const char *asset_name, dbm_db **db, size_t size) {
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
    return dbm_open(db, path);
  } 
  else {
    return dbm_create(db, path, size);
  }
}

