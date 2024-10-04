#include "client.h"
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
  client_open(&client, "165.22.176.143", "http");
  client_send_message(&client, "");
  client_receive_message(&client);
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
  bool bytes_received = tcp_stream_receive_no_timeout(client->server, print_buffer, NULL);
  /*if (bytes_received) {*/
  /*  const char *response =*/
  /*    "HTTP/1.1 200 OK\r\n"*/
  /*    "Content-Type: application/json\r\n"*/
  /*    "Content-Length: 16\r\n"*/
  /*    "Connection: keep-alive\r\n\r\n"*/
  /*    "{\"status\":\"ok\"}";*/
  /*  tcp_send(client->server, response, strlen(response), TIMEOUT);*/
  /*} */
}

void client_send_message(const game_client *client, const char *message) {
  const char *data = message;
  const char *request = "POST /send HTTP/1.1\r\nHost: %s\r\nContent-Type: application/json\r\nContent-Length: %zu\r\n\r\n%s";
  char formatted_request[1024];
  snprintf(formatted_request, sizeof(formatted_request), request, client->ip, strlen(data), data);
  tcp_send(client->server, formatted_request, sizeof(formatted_request), TIMEOUT);
  bool received = tcp_stream_receive(client->server, ignore_buffer, NULL, TIMEOUT);
  while (!received) {
    received = tcp_stream_receive(client->server, ignore_buffer, NULL, TIMEOUT);
  }
  printf("Hello\n");
}

