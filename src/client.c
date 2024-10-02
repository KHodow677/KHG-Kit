#include "client.h"
#include "net_process.h"
#include "khg_tcp/tcp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void run_client() {
  game_client client = {0};
  client_open(&client, "165.22.176.143", "http");
  client_send_message(&client, "");
  while(true) {
    (void)0;
  }
  client_close(&client);
}

void client_open(game_client *client, const char *ip, const char *port) {
  printf("Open Client\n");
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
  tcp_stream_receive_no_timeout(client->server, print_buffer, NULL);
  const char *request = "GET /receive HTTP/1.1\r\nHost: localhost\r\n\r\n";
  tcp_send(client->server, request, strlen(request), 500);
  tcp_stream_receive_no_timeout(client->server, ignore_buffer, NULL);
  while (1) {
    tcp_stream_receive_no_timeout(client->server, print_buffer, NULL);
  }
}

void client_send_message(const game_client *client, const char *message) {
  const char *data = "{\"message\":\"Hello from Client!\"}";
  const char *request = "POST /send HTTP/1.1\r\nHost: 165.22.176.143\r\nContent-Type: application/json\r\nContent-Length: %zu\r\n\r\n%s";
  char formatted_request[1024];
  snprintf(formatted_request, sizeof(formatted_request), request, strlen(data), data);
  tcp_send(client->server, formatted_request, sizeof(formatted_request), 500);
  tcp_stream_receive(client->server, print_buffer, NULL, 500);
}

