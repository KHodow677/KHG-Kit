#include "networking/client.h"
#include "networking/processing.h"
#include "khg_tcp/error.h"
#include "khg_tcp/tcp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

void hoster_run(const char *room_code) {
  tcp_init();
  tcp_channel *channel = tcp_connect("165.22.176.143", "http");
  tcp_client_receive(channel);
  tcp_close_channel(channel);
  tcp_term();
}

void joiner_run(const char *room_code) {
  tcp_init();
  tcp_channel *channel = tcp_connect("165.22.176.143", "http");
  tcp_client_send(channel);
  tcp_close_channel(channel);
  tcp_term();
}

void tcp_client_send(tcp_channel *channel) {
  tcp_init();
  const char *data = "{\"message\":\"FFFFFF:Hello from Client 1!\"}";
  const char *request = "POST /send HTTP/1.1\r\nHost: 165.22.176.143\r\nContent-Type: application/json\r\nContent-Length: %zu\r\n\r\n%s";
  char formatted_request[1024];
  snprintf(formatted_request, sizeof(formatted_request), request, strlen(data), data);
  tcp_send(channel, formatted_request, strlen(formatted_request), 500);
  tcp_stream_receive(channel, ignore_buffer, NULL, 500);
}

void tcp_client_receive(tcp_channel *channel) {
  tcp_init();
  const char *request = "GET /receive HTTP/1.1\r\nHost: 165.22.176.143\r\n\r\n";
  tcp_send(channel, request, strlen(request), 500);
  tcp_stream_receive_no_timeout(channel, ignore_buffer, NULL);
  while (1) {
    tcp_stream_receive_no_timeout(channel, message_buffer, NULL);
  }
}

int test_tcp_client_send() {
  tcp_channel *channel = NULL;
  tcp_set_error_callback(process_error, &channel);
  tcp_init();
  const char *data = "{\"message\":\"FFFFFF:Hello from Client 1!\"}";
  const char *request = "POST /send HTTP/1.1\r\nHost: 165.22.176.143\r\nContent-Type: application/json\r\nContent-Length: %zu\r\n\r\n%s";
  char formatted_request[1024];
  snprintf(formatted_request, sizeof(formatted_request), request, 41, data);
  channel = tcp_connect("165.22.176.143", "http");
  printf("Formatted request:\n%s\n", formatted_request);
  tcp_send(channel, formatted_request, strlen(formatted_request), 500);
  tcp_stream_receive(channel, message_buffer, NULL, 500);
  tcp_close_channel(channel);
  tcp_term();
  return 0;
}

int test_tcp_client_receive() {
  tcp_channel *channel = NULL;
  tcp_set_error_callback(process_error, &channel);
  tcp_init();
  const char *request = "GET /receive HTTP/1.1\r\nHost: 165.22.176.143\r\n\r\n";
  channel = tcp_connect("165.22.176.143", "http");
  tcp_send(channel, request, strlen(request), 500);
  tcp_stream_receive_no_timeout(channel, ignore_buffer, NULL);
  tcp_close_channel(channel);
  tcp_term();
  return 0;
}
