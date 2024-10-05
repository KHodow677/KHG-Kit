#include "networking/server.h"
#include "khg_tcp/tcp.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

static int MAX_CLIENTS = 10;

void server_start(game_server *server, const char *address, const char *port, int max_clients) {
  tcp_init();
  server->server = tcp_open_server(address, port, max_clients);
  if (server->server) {
    printf("Server started at %s:%s\n", address, port);
  } 
  else {
    fprintf(stderr, "Failed to start server at %s:%s\n", address, port);
    exit(EXIT_FAILURE);
  }
}

void server_run(game_server *server) {
  printf("Waiting for clients to connect...\n");
  while (true) {
    tcp_channel *client_channel = tcp_accept(server->server, 0);
    if (client_channel) {
      game_server_client client = { client_channel, 0 };
      printf("Client connected!\n");
      handle_client(&client);
      tcp_close_channel(client_channel);
    }
  }
}

void handle_client(game_server_client *client) {
  char buffer[TCP_STREAM_BUFFER_SIZE];
  while (true) {
    int bytes_received = tcp_receive(client->channel, buffer, TCP_STREAM_BUFFER_SIZE, 1000);
    if (bytes_received > 0) {
      printf("Received: %.*s\n", bytes_received, buffer);
      tcp_send(client->channel, buffer, bytes_received, 1000);
    } 
    else {
      printf("Client disconnected or timeout.\n");
      break;
    }
  }
}

void server_shutdown(game_server *server) {
  tcp_close_server(server->server);
  tcp_term();
  printf("Server shut down.\n");
}
