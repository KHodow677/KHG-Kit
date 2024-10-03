#pragma once

#include "khg_tcp/tcp.h"

typedef struct {
  tcp_channel *server;
  const char *ip;
  const char *port;
  char buffer[TCP_STREAM_BUFFER_SIZE];
} game_client;

typedef struct {
  int id;
  int linked_id;
} game_client_client;

int client_run(void);

void client_open(game_client *client, const char *ip, const char *port);
void client_close(const game_client *client);
void client_receive_message(const game_client *client);
void client_send_message(const game_client *client, const char *message);
