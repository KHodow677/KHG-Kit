#include "server.h"
#include "net_process.h"
#include "khg_tcp/tcp.h"
#include "khg_utl/map.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int TIMEOUT = 500;

static int compare_clients(const key_type a, const key_type b) {
  return *(const int*)a - *(const int*)b;
}

static void int_deallocator(void* data) {
  free(data);
}

static void gsc_deallocator(void* data) {
  game_server_client *gsc = data;
  tcp_close_channel(gsc->client);
  free(gsc);
}

int server_run() {
  game_server server = { 0 };
  tcp_init();
  server_open(&server, "localhost", "3000");
  while (true) {
    server_accept_client(&server);
    if (!utl_map_empty(server.client_lookup)) {
      for (utl_map_iterator it = utl_map_begin(server.client_lookup); it.node != utl_map_end(server.client_lookup).node; utl_map_iterator_increment(&it)) {
        server_receive_message(&server, *((int *)utl_map_node_get_key(it.node)));
      }
    }
  }
  server_close(&server);
  return 0;
}

void server_open(game_server *server, const char *ip, const char *port) {
  printf("Open Server\n");
  tcp_init();
  server->ip = ip;
  server->port = port;
  server->server = tcp_open_server(ip, port, 1);
  server->client_lookup = utl_map_create(compare_clients, int_deallocator, gsc_deallocator);
}

void server_close(const game_server *server) {
  printf("Close Server\n");
  utl_map_deallocate(server->client_lookup);
  tcp_close_server(server->server);
  tcp_term();
}

void server_accept_client(game_server *server) {
  tcp_channel *client = tcp_accept(server->server, TIMEOUT);
  if (client == NULL) {
    return;
  }
  printf("Connect a Client\n");
  int* key = malloc(sizeof(int));
  game_server_client* value = malloc(sizeof(game_server_client));
  *key = 1000;
  *value = (game_server_client){ .client = client, .id = *key, .linked_id = -1 };
  utl_map_insert(server->client_lookup, key, value);
}

void server_send_message(const game_server *server, const int reciever_id, const char *message) {
  int lookup = reciever_id;
  const char *data = message;
  const char *response = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: %zu\r\nConnection: keep-alive\r\n\r\n%s";
  char formatted_response[1024];
  snprintf(formatted_response, sizeof(formatted_response), response, strlen(data), data);
  tcp_send(((game_server_client *)utl_map_at(server->client_lookup, &lookup))->client, formatted_response, strlen(formatted_response), TIMEOUT);
}

void server_receive_message(const game_server *server, const int sender_id) {
  int lookup = sender_id;
  char buffer[1024] = {0};
  bool bytes_received = tcp_stream_receive(((game_server_client *)utl_map_at(server->client_lookup, &lookup))->client, print_buffer, NULL, TIMEOUT);
  if (bytes_received) {
    printf("Received from client (%d):\n%s\n", sender_id, buffer);
    const char *response =
      "HTTP/1.1 200 OK\r\n"
      "Content-Type: application/json\r\n"
      "Content-Length: 16\r\n"
      "Connection: keep-alive\r\n\r\n"
      "{\"status\":\"ok\"}";
    tcp_send(((game_server_client *)utl_map_at(server->client_lookup, &lookup))->client, response, strlen(response), TIMEOUT);
  } 
}

