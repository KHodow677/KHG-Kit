#include "networking/client.h"
#include "networking/net_process.h"
#include "khg_tcp/tcp.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

void sender_run(const char *room_code) {
  tcp_client_create_room(room_code);
  sleep(1);
  const char *message = "Hello from the sender in ROOM!";
  tcp_client_send(message);
}

void receiver_run(const char *room_code) {
  tcp_client_join_room(room_code);
  printf("Listening for messages in room %s...\n", room_code);
  tcp_client_receive();
}

int tcp_client_create_room(const char *room_code) {
  tcp_channel *channel = NULL;
  tcp_init();
  char data[256];
  snprintf(data, sizeof(data), "{\"roomCode\":\"%s\"}", room_code);
  const char *request = "POST /room/create HTTP/1.1\r\nHost: 165.22.176.143\r\nContent-Type: application/json\r\nContent-Length: %zu\r\n\r\n%s";
  char formatted_request[1024];
  snprintf(formatted_request, sizeof(formatted_request), request, strlen(data), data);
  channel = tcp_connect("165.22.176.143", "http");
  tcp_send(channel, formatted_request, strlen(formatted_request), 500);
  tcp_stream_receive(channel, ignore_buffer, NULL, 500);
  tcp_close_channel(channel);
  tcp_term();
  return 0;
}

int tcp_client_join_room(const char *room_code) {
  tcp_channel *channel = NULL;
  tcp_init();
  char data[256];
  snprintf(data, sizeof(data), "{\"roomCode\":\"%s\"}", room_code);
  const char *request = "POST /room/join HTTP/1.1\r\nHost: 165.22.176.143\r\nContent-Type: application/json\r\nContent-Length: %zu\r\n\r\n%s";
  char formatted_request[1024];
  snprintf(formatted_request, sizeof(formatted_request), request, strlen(data), data);
  channel = tcp_connect("165.22.176.143", "http");
  tcp_send(channel, formatted_request, strlen(formatted_request), 500);
  tcp_stream_receive(channel, ignore_buffer, NULL, 500);
  tcp_close_channel(channel);
  tcp_term();
  return 0;
}

int tcp_client_send(const char *message) {
  tcp_channel *channel = NULL;
  tcp_init();
  char data[256];
  snprintf(data, sizeof(data), "{\"message\":\"%s\"}", message);
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
  while (1) {
    tcp_stream_receive_no_timeout(channel, message_buffer, NULL);
    printf("Message: %s\n", message_buffer);  // Print received message
  }
  tcp_close_channel(channel);
  tcp_term();
  return 0;
}

