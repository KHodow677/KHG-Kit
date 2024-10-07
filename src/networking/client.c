#include "networking/client.h"
#include "networking/net_process.h"
#include "khg_tcp/tcp.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

void hoster_run(const char *room_code) {
  tcp_init();
  tcp_channel *channel = tcp_connect("165.22.176.143", "http");
  tcp_client_create_room(channel, room_code);
  tcp_client_receive(channel);
}

void joiner_run(const char *room_code) {
  tcp_init();
  tcp_channel *channel = tcp_connect("165.22.176.143", "http");
  tcp_client_join_room(channel, room_code);
  const char *message = "Hello from the sender!";
  tcp_client_send(channel, "send_message", message);
  tcp_client_receive(channel);
}

void tcp_client_create_room(tcp_channel *channel, const char *room_code) {
  char data[256];
  snprintf(data, sizeof(data), "{\"command\":\"create_room\", \"room_code\":\"%s\"}", room_code);
  
  const char *request = "POST /send HTTP/1.1\r\nHost: 165.22.176.143\r\nContent-Type: application/json\r\nContent-Length: %zu\r\n\r\n%s";
  char formatted_request[1024];
  snprintf(formatted_request, sizeof(formatted_request), request, strlen(data), data);
  tcp_send(channel, formatted_request, strlen(formatted_request), 500);
}

void tcp_client_join_room(tcp_channel *channel, const char *room_code) {
  char data[256];
  snprintf(data, sizeof(data), "{\"command\":\"join_room\", \"room_code\":\"%s\"}", room_code);
  const char *request = "POST /send HTTP/1.1\r\nHost: 165.22.176.143\r\nContent-Type: application/json\r\nContent-Length: %zu\r\n\r\n%s";
  char formatted_request[1024];
  snprintf(formatted_request, sizeof(formatted_request), request, strlen(data), data);
  tcp_send(channel, formatted_request, strlen(formatted_request), 500);
}

void tcp_client_send(tcp_channel *channel, const char *command, const char *message) {
  char data[256];
  snprintf(data, sizeof(data), "{\"command\":\"%s\", \"message\":\"%s\"}", command, message);
  const char *request = "POST /send HTTP/1.1\r\nHost: 165.22.176.143\r\nContent-Type: application/json\r\nContent-Length: %zu\r\n\r\n%s";
  char formatted_request[1024];
  snprintf(formatted_request, sizeof(formatted_request), request, strlen(data), data);
  tcp_send(channel, formatted_request, strlen(formatted_request), 500);
  tcp_stream_receive(channel, ignore_buffer, NULL, 500);
  tcp_close_channel(channel);
  tcp_term();
}

void tcp_client_receive(tcp_channel *channel) {
  const char *request = "GET /receive HTTP/1.1\r\nHost: 165.22.176.143\r\n\r\n";
  tcp_send(channel, request, strlen(request), 500);
  printf("Listening for messages...\n");
  while (1) {
    tcp_stream_receive_no_timeout(channel, message_buffer, NULL);
  }
  tcp_close_channel(channel);
  tcp_term();
}

