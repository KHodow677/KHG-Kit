#include "client.h"
#include "game.h"
#include "net_process.h"
#include "khg_tcp/tcp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int TIMEOUT = 500;

int client_run() {
  game_client client = {0};
  tcp_init();
  client_open(&client, "localhost", "3000");
  client_send_message(&client, "");
  game_run();
  client_close(&client);
  return 0;
}

void client_open(game_client *client, const char *ip, const char *port) {
  printf("Open Client\n");
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
  tcp_stream_receive_no_timeout(client->server, set_buffer, NULL);
  strncpy(client->buffer, BUFFER, TCP_STREAM_BUFFER_SIZE);
  printf("%s\n", client->buffer);
}

void client_send_message(const game_client *client, const char *message) {
  const char *data = message;
  const char *request = "POST /send HTTP/1.1\r\nHost: %s\r\nContent-Type: application/json\r\nContent-Length: %zu\r\n\r\n%s";
  char formatted_request[1024];
  snprintf(formatted_request, sizeof(formatted_request), request, client->ip, strlen(data), data);
  tcp_send(client->server, formatted_request, sizeof(formatted_request), TIMEOUT);
}
