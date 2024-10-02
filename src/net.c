#include "net.h"
#include "khg_tcp/error.h"
#include "khg_tcp/tcp.h"
#include "khg_utl/vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

char BUFFER[TCP_STREAM_BUFFER_SIZE] = "";

static void process_error(tcp_error e, void *user_data) {
	tcp_channel **channel = (tcp_channel **) user_data;
	perror(tcp_error_to_string(e));
	tcp_close_channel(*channel);
	tcp_term();
	exit(-1);
}

static bool print_buffer(const char *buffer, int length, void *user_data) {
	(void) user_data;
  memset(BUFFER, 0, sizeof(BUFFER));
  strncpy(BUFFER, buffer, length);
  printf("%s\n", BUFFER);
	return strlen(buffer) == (size_t)length;
}

void run_server_client() {
  game_server server = {0};
  game_client client = {0};
  server_open(&server);
  client_open(&client);
  sever_accept_client(&server);
  server_send_message(&server, *((tcp_channel **)utl_vector_front(server.client_list)), "Hello World from Server!", 24);
  client_receive_message(&client);
  client_close(&client);
  server_close(&server);
}

void run_server() {
  game_server server = {0};
  server_open(&server);
  while (true) {
    sever_accept_client(&server);
    if (!utl_vector_is_empty(server.client_list)) {
      server_send_message(&server, *((tcp_channel **)utl_vector_front(server.client_list)), "Hello World from Server!\n", 24);
    }
    sleep(1);
  }
  server_close(&server);
}

void run_client() {
  game_client client = {0};
  client_open(&client);
  while (true) {
    client_receive_message(&client);
  }
  client_close(&client);
}

void server_open(game_server *server) {
  printf("Open Server\n");
  tcp_init();
  server->server = tcp_open_server("localhost", "3000", 1);
  server->client_list = utl_vector_create(sizeof(tcp_channel *));
}

void sever_accept_client(game_server *server) {
  tcp_channel *client = tcp_accept(server->server, 0);
  if (client == NULL) {
    return;
  }
  printf("Connect a Client\n");
  utl_vector_push_back(server->client_list, &client);
}

void server_close(game_server *server) {
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

void server_send_message(game_server *server, tcp_channel *client, const char *message, const int length) {
  tcp_channel *found_client;
  if (!utl_vector_is_empty(server->client_list)) {
    for (tcp_channel **it = (tcp_channel **)utl_vector_begin(server->client_list); it != (tcp_channel **)utl_vector_end(server->client_list); it++) { 
      if (*it == client) {
        found_client = *it;
      }
    }
    tcp_send(found_client, message, length, 0);
  }
}

void client_open(game_client *client) {
  printf("Open Client\n");
	tcp_channel *channel = NULL;
	tcp_set_error_callback(process_error, &channel);
  tcp_init();
  client->client = tcp_connect("localhost", "3000");
}

void client_close(game_client *client) {
  printf("Close Client\n");
  tcp_close_channel(client->client);
  tcp_term();
}

void client_receive_message(game_client *client) {
  tcp_stream_receive(client->client, print_buffer, NULL, 0);
}
