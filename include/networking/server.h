#pragma once

#include "khg_tcp/tcp.h"
#include <stdbool.h>
#include <assert.h>

#define MAX_CLIENTS 10

typedef struct {
  char *id;
  tcp_channel *sock;
} client_t;

extern client_t clients[MAX_CLIENTS];
extern int client_count;
extern unsigned int client_id_counter;

void handle_client(tcp_channel *client_sock);
int server_start();

