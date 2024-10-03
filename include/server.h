#pragma once

#include "khg_tcp/tcp.h"
#include "khg_utl/map.h"

typedef struct {
  tcp_server *server;
  utl_map *client_lookup;
  const char *ip;
  const char *port;
} game_server;

typedef struct {
  tcp_channel *client;
  int id;
  int linked_id;
} game_server_client;

int server_run(void);

void server_open(game_server *server, const char *ip, const char *port);
void server_close(const game_server *server);
void server_accept_client(game_server *server);
void server_send_message(const game_server *server, const int reciever_id, const char *message);
void server_receive_message(const game_server *server, const int sender_id);
