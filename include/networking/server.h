#pragma once

#include "khg_tcp/tcp.h"

typedef struct {
    tcp_server *server;
    const char *address;
    const char *port;
} game_server;

typedef struct {
    tcp_channel *channel;
    int client_id;
} game_server_client;

void server_start(game_server *server, const char *address, const char *port, int max_clients);
void server_run(game_server *server);
void server_shutdown(game_server *server);

void handle_client(game_server_client *client);

