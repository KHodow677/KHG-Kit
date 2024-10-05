#pragma once

#include "khg_tcp/tcp.h"

typedef struct {
    tcp_channel *server;
    const char *ip;
    const char *port;
    char buffer[TCP_STREAM_BUFFER_SIZE];
} game_client;

void client_connect(game_client *client, const char *ip, const char *port);
void client_disconnect(game_client *client);
void client_send_message(game_client *client, const char *message);
void client_receive_message(game_client *client);
void client_run(void);

