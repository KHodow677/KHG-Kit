#pragma once

#include "khg_tcp/tcp.h"
#include "khg_utl/vector.h"

typedef struct game_server {
  tcp_server *server;
  utl_vector *client_list;
} game_server;

typedef struct game_client {
  tcp_channel *client;
} game_client;

void run_server_client(void);
void run_server(void);
void run_client(void);

void server_open(game_server *server);
void sever_accept_client(game_server *server);
void server_send_message(game_server *server, tcp_channel *client, const char *message, const int length);
void server_close(game_server *server);

void client_open(game_client *client);
void client_close(game_client *client);
void client_receive_message(game_client *client);
