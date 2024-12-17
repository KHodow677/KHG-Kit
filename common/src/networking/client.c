#include "networking/client.h"
#include "khg_tcp/error.h"
#include "khg_tcp/tcp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

static void process_error(tcp_error e, void *user_data) {
  tcp_channel **channel = (tcp_channel **) user_data;
  perror(tcp_error_to_string(e));
  tcp_close_channel(*channel);
  tcp_term();
  exit(-1);
}

void hoster_run() {
  tcp_init();
  tcp_channel *err_channel = NULL;
  tcp_set_error_callback(process_error, &err_channel);
  tcp_channel *channel = tcp_connect("165.22.176.143", "http");
  tcp_client_receive(channel);
  tcp_close_channel(channel);
  tcp_close_channel(err_channel);
  tcp_term();
}

void joiner_run() {
  tcp_init();
  tcp_channel *channel = tcp_connect("165.22.176.143", "http");
  tcp_client_send(channel);
  tcp_close_channel(channel);
  tcp_term();
}

void tcp_client_send(tcp_channel *channel) {
  tcp_init();
  const char *data = "000000000000000000000000000000000000000000000000000000000000000000000000";
  const char *request = "POST /send HTTP/1.1\r\nHost: 165.22.176.143\r\nContent-Type: application/json\r\nContent-Length: %zu\r\n\r\n%s";
  char formatted_request[1024];
  snprintf(formatted_request, sizeof(formatted_request), request, strlen(data), data);
  tcp_send(channel, formatted_request, strlen(formatted_request), 500);
  char formatted_response[1024];
  tcp_receive(channel, formatted_response, 1024, 500);
  printf("%s\n", formatted_response);
}

void tcp_client_receive(tcp_channel *channel) {
  tcp_init();
  const char *data = "000000000000000000000000000000000000000000000000000000000000000000000000";
  const char *request = "GET /receive HTTP/1.1\r\nHost: 165.22.176.143\r\n\r\n";
  char formatted_request[1024];
  snprintf(formatted_request, sizeof(formatted_request), request, strlen(data), data);
  tcp_send(channel, formatted_request, strlen(formatted_request), 500);
  char formatted_response[1024];
  tcp_receive(channel, formatted_response, 1024, 500);
  printf("%s\n", formatted_response);
}

