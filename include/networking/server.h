#pragma once

#include "khg_tcp/tcp.h"

#define BUFFER_SIZE 2048
#define MAX_CLIENTS 10

typedef struct {
  char id[7];
  tcp_channel *sock;
} Client;

extern Client clients[MAX_CLIENTS];
extern int client_count;
extern int client_id_counter;

void get_client_id(char *client_id);
void handle_client_data(Client *client, const char *data);
int server_start(void);

