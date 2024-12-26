#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#else
#include <linux/limits.h>
#endif

#include "networking/info.h"
#include "networking/server.h"
#include "networking/processing.h"
#include "khg_dbm/dbm.h"
#include "khg_tcp/tcp.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

typedef struct user {
  char name[50];
  int age;
} user;

static const char *NETWORK_DELIMITER = "::";
static const char *NETWORK_NULL_COMMAND = "NULLC";

void server_run(const char *tag) {
  net_info_setup("res/assets/data/net/info.ini");
  tcp_init();
  tcp_server *server = tcp_open_server("localhost", "http", 10);
  while (1) {
    tcp_channel *channel = tcp_accept(server, 0);
    if (!channel) {
      continue;
    }
    char request_buffer[1024];
    if (tcp_receive(channel, request_buffer, 1024, 500) && strstr(request_buffer, tag)) {
      char *packet_info = strstr(request_buffer, NETWORK_DELIMITER) + 2 * strlen(NETWORK_DELIMITER) + strlen(NETWORK_NULL_COMMAND);
      if (packet_info) {
        const char *data = "%s::RESLT::%s";
        const char *response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n%s";
        char formatted_data[256];
        snprintf(formatted_data, sizeof(formatted_data), data, SERVER_TAG, packet_info);
        char formatted_response[1024];
        snprintf(formatted_response, sizeof(formatted_response), response, formatted_data);
        tcp_send(channel, formatted_response, strlen(formatted_response), 500);
      }
    }
    tcp_close_channel(channel);
  }
  tcp_term();
}

/*  dbm_db *db;*/
/*  create_or_open_db("server_player", &db, sizeof(user)); */
/*  int64_t key = 1;*/
/*  user result = {"John Doe", 20 };*/
/*  dbm_insert(db, key, &result);*/
/*  user select_result;*/
/*  dbm_select(db, 1, &select_result);*/
/*  printf("HUMAN:\nname: %s\nage: %i \n", select_result.name, select_result.age);*/
/*  dbm_close(&db);*/

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

