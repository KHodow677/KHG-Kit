#pragma once

#include "khg_tcp/tcp.h"
#include "khg_utl/vector.h"

typedef struct game_server {
  tcp_server *server;
  utl_vector *client_list;
  const char *ip;
  const char *port;
} game_server;

typedef struct game_client {
  tcp_channel *server;
  const char *ip;
  const char *port;
} game_client;

void run_server(void);
void run_client(void);

void server_open(game_server *server, const char *ip, const char *port);
void server_close(const game_server *server);
void server_accept_client(game_server *server);
void server_send_message(const game_server *server, const tcp_channel *client, const char *message, const int length);
void server_receive_message(const game_server *server, tcp_channel *client);

void client_open(game_client *client, const char *ip, const char *port);
void client_close(const game_client *client);
void client_receive_message(const game_client *client);
void client_send_message(const game_client *client, const char *message);

