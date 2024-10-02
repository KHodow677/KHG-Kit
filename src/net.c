#include "net.h"
#include "khg_tcp/error.h"
#include "khg_tcp/tcp.h"
#include "khg_utl/vector.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <winscard.h>

char BUFFER[TCP_STREAM_BUFFER_SIZE] = "";

static void process_error(tcp_error e, void *user_data) {
	tcp_channel **channel = (tcp_channel **) user_data;
	perror(tcp_error_to_string(e));
	tcp_close_channel(*channel);
	tcp_term();
	exit(-1);
}

bool is_number(const char *str) { 
  char *endptr; 
  errno = 0;
  long val = strtol(str, &endptr, 10); 
  if (errno == ERANGE || (endptr == str)) { 
    return false;
  } 
  return true;
} 

static bool print_buffer(const char *buffer, const int length, void *user_data) {
	(void) user_data;
  memset(BUFFER, 0, sizeof(BUFFER));
  strncpy(BUFFER, buffer, length);
  printf("%s\n", BUFFER);
	return strlen(buffer) == (size_t)length;
}

static bool ignore_buffer(const char *buffer, int length, void *user_data) {
	(void) user_data;
	return strlen(buffer) == (size_t) length;
}

static bool message_buffer(const char *buffer, int length, void *user_data) {
  (void) user_data;
  memset(BUFFER, 0, sizeof(BUFFER));
  strncpy(BUFFER, buffer, length);
  char *body = strstr(BUFFER, "\r\n\r\n");
  if (body) {
    body += 4;
    char *newline = strchr(body, '\n');
    if (newline) {
      *newline = '\0';
    }
    if (!is_number(body)) {
      printf("%s\n", body);
    }
  } 
  else {
    printf("No valid response body found.\n");
  }
  return strlen(buffer) == (size_t) length;
}

void run_server_client() {
  game_server server = {0};
  game_client client = {0};
  server_open(&server, "localhost", "http");
  client_open(&client, "localhost", "http");
  server_accept_client(&server);
  server_send_message(&server, *((tcp_channel **)utl_vector_front(server.client_list)), "Hello World from Server!", 24);
  client_receive_message(&client);
  client_close(&client);
  server_close(&server);
}

void run_server() {
  game_server server = {0};
  server_open(&server, "localhost", "http");
  while (true) {
    server_accept_client(&server);
    if (!utl_vector_is_empty(server.client_list)) {
      server_receive_message(&server, *((tcp_channel **)utl_vector_front(server.client_list)));
    }
  }
  server_close(&server);
}

void run_client() {
  game_client client = {0};
  client_open(&client, "localhost", "http");
  while (true) {
    client_receive_message(&client);
  }
  client_close(&client);
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
  tcp_stream_receive(client, print_buffer, NULL, 500);
}

void client_open(game_client *client, const char *ip, const char *port) {
  printf("Open Client\n");
	tcp_channel *channel = NULL;
	tcp_set_error_callback(process_error, &channel);
  tcp_init();
  client->ip = ip;
  client->port = port;
  client->server = tcp_connect(ip, port);
}

void client_close(const game_client *client) {
  printf("Close Client\n");
  tcp_close_channel(client->server);
  tcp_term();
}

void client_receive_message(const game_client *client) {
  const char *request = "GET /receive HTTP/1.1\r\nHost: localhost\r\n\r\n";
  tcp_send(client->server, request, strlen(request), 500);
  tcp_stream_receive_no_timeout(client->server, ignore_buffer, NULL);
  while (1) {
    tcp_stream_receive_no_timeout(client->server, message_buffer, NULL);
  }
}

void client_send_message(const game_client *client, const char *message, const int length) {
  const char *data = "{\"message\":\"Hello from Client!\"}";
  const char *request = "POST /send HTTP/1.1\r\nHost: localhost\r\nContent-Type: application/json\r\nContent-Length: %zu\r\n\r\n%s";
  char formatted_request[1024];
  snprintf(formatted_request, sizeof(formatted_request), request, strlen(data), data);
  tcp_send(client->server, request, length, 500);
  tcp_stream_receive(client->server, ignore_buffer, NULL, 500);
}
