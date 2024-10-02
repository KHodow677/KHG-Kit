#include "server.h"
#include "net_process.h"
#include "khg_tcp/tcp.h"
#include "khg_utl/vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void run_server() {
  game_server server = {0};
  server_open(&server, "localhost", "3000");
  while (true) {
    server_accept_client(&server);
    if (!utl_vector_is_empty(server.client_list)) {
      server_receive_message(&server, *((tcp_channel **)utl_vector_front(server.client_list)));
    }
  }
  server_close(&server);
}

void server_open(game_server *server, const char *ip, const char *port) {
  printf("Open Server\n");
  tcp_init();
  server->ip = ip;
  server->port = port;
  server->server = tcp_open_server(ip, port, 1);
  server->client_list = utl_vector_create(sizeof(tcp_channel *));
}

void server_close(const game_server *server) {
  printf("Close Server\n");
  if (!utl_vector_is_empty(server->client_list)) {
    for (tcp_channel **it = (tcp_channel **)utl_vector_begin(server->client_list); it != (tcp_channel **)utl_vector_end(server->client_list); it++) { 
      tcp_close_channel(*it);
    }
  }
  utl_vector_deallocate(server->client_list);
  tcp_close_server(server->server);
  tcp_term();
}

void server_accept_client(game_server *server) {
  tcp_channel *client = tcp_accept(server->server, 0);
  if (client == NULL) {
    return;
  }
  printf("Connect a Client\n");
  utl_vector_push_back(server->client_list, &client);
}

void server_send_message(const game_server *server, const tcp_channel *client, const char *message, const int length) {
  tcp_channel *found_client;
  if (!utl_vector_is_empty(server->client_list)) {
    for (tcp_channel **it = (tcp_channel **)utl_vector_begin(server->client_list); it != (tcp_channel **)utl_vector_end(server->client_list); it++) { 
      if (*it == client) {
        found_client = *it;
      }
    }
    tcp_send(found_client, message, length, 500);
  }
}

void server_receive_message(const game_server *server, tcp_channel *client) {
  tcp_stream_receive_no_timeout(client, print_buffer, NULL);
}

