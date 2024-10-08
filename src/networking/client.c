#include "networking/client.h"
#include "khg_tcp/error.h"
#include "khg_tcp/tcp.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

char BUFFER[TCP_STREAM_BUFFER_SIZE] = "";

void process_error(tcp_error e, void *user_data) {
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

bool print_buffer(const char *buffer, int length, void *user_data) {
	(void) user_data;
  memset(BUFFER, 0, sizeof(BUFFER));
  strncpy(BUFFER, buffer, length);
  printf("%s\n", BUFFER);
	return strlen(buffer) == (size_t) length;
}

bool ignore_buffer(const char *buffer, int length, void *user_data) {
	(void) user_data;
	return strlen(buffer) == (size_t) length;
}

bool message_buffer(const char *buffer, int length, void *user_data) {
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
