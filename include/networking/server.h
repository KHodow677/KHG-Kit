#pragma once

#include "khg_tcp/tcp.h"
#include <stdbool.h>
#include <assert.h>

#define MAX_CLIENTS 100
#define BUFFER_SIZE 2048

typedef struct {
  char id[7];
  tcp_channel *sock;
} client_t;

extern client_t clients[MAX_CLIENTS];
extern int client_count;
extern unsigned int client_id_counter;
extern char clientData[BUFFER_SIZE];

void assign_client_id(client_t *client);

client_t *find_client_by_sock(tcp_channel *sock);

client_t *find_client_by_id(const char *id);

void handle_post_request(tcp_channel *client_sock, const char *body);

void handle_get_request(tcp_channel *client_sock);

void handle_client_request(tcp_channel *client_sock, const char *request);

void handle_new_client(tcp_channel *client_sock);
void close_client(tcp_channel *client_sock);

bool process_client_message(const char *buffer, int length, void *user_data);

int server_start();

