#include "networking/client.h"
#include "networking/net_process.h"
#include "khg_tcp/tcp.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

int tcp_client_send() {
  tcp_channel *channel = NULL;
  tcp_init();
  const char *data = "{\"message\":\"FFFFFF:Hello from Client 1!\"}";
  const char *request = "POST /send HTTP/1.1\r\nHost: 165.22.176.143\r\nContent-Type: application/json\r\nContent-Length: %zu\r\n\r\n%s";
  char formatted_request[1024];
  snprintf(formatted_request, sizeof(formatted_request), request, strlen(data), data);
  channel = tcp_connect("165.22.176.143", "http");
  tcp_send(channel, formatted_request, strlen(formatted_request), 500);
  tcp_stream_receive(channel, ignore_buffer, NULL, 500);
  tcp_close_channel(channel);
  tcp_term();
  return 0;
}

int tcp_client_receive() {
  tcp_channel *channel = NULL;
  tcp_init();
  const char *request = "GET /receive HTTP/1.1\r\nHost: 165.22.176.143\r\n\r\n";
  channel = tcp_connect("165.22.176.143", "http");
  tcp_send(channel, request, strlen(request), 500);
  tcp_stream_receive_no_timeout(channel, message_buffer, NULL);
  while (1) {
    tcp_stream_receive_no_timeout(channel, message_buffer, NULL);
  }
  tcp_close_channel(channel);
  tcp_term();
  return 0;
}

