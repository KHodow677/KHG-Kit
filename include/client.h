#pragma once

#include "khg_tcp/tcp.h"

typedef struct game_client {
  tcp_channel *server;
  const char *ip;
  const char *port;
} game_client;

void run_client(void);

void client_open(game_client *client, const char *ip, const char *port);
void client_close(const game_client *client);
void client_receive_message(const game_client *client);
void client_send_message(const game_client *client, const char *message);

