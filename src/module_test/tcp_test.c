#include "tcp_test.h"
#include "khg_dbm/util.h"
#include "khg_tcp/error.h"
#include "khg_tcp/tcp.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h> 
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

int tcp_test() {
	tcp_channel *channel = NULL;
	tcp_set_error_callback(process_error, &channel);
	tcp_init();
	const char *request = "HEAD / HTTP/1.2\r\n\r\n";
	channel = tcp_connect("www.google.com", "http");
	tcp_send(channel, request, strlen(request), 500);
	tcp_stream_receive(channel, print_buffer, NULL, 500);
	tcp_close_channel(channel);
	tcp_term();
	return 0;
}

int tcp_client_send() {
  tcp_channel *channel = NULL;
  tcp_set_error_callback(process_error, &channel);
  tcp_init();
  const char *data = "{\"message\":\"FFFFFF:Hello from Client 1!\"}";
  const char *request = "POST /send HTTP/1.1\r\nHost: khgsvr.fly.dev\r\nContent-Type: application/json\r\nContent-Length: %zu\r\n\r\n%s";
  char formatted_request[1024];
  snprintf(formatted_request, sizeof(formatted_request), request, strlen(data), data);
  channel = tcp_connect("khgsvr.fly.dev", "http");
  tcp_send(channel, formatted_request, strlen(formatted_request), 500);
  tcp_stream_receive(channel, ignore_buffer, NULL, 500);
  tcp_close_channel(channel);
  tcp_term();
  return 0;
}

int tcp_client_receive() {
  tcp_channel *channel = NULL;
  tcp_set_error_callback(process_error, &channel);
  tcp_init();
  const char *request = "GET /receive HTTP/1.1\r\nHost: khgsvr.fly.dev\r\n\r\n";
  channel = tcp_connect("khgsvr.fly.dev", "http");
  tcp_send(channel, request, strlen(request), 500);
  tcp_stream_receive_no_timeout(channel, ignore_buffer, NULL);
  while (1) {
    tcp_stream_receive_no_timeout(channel, message_buffer, NULL);
  }
  tcp_close_channel(channel);
  tcp_term();
  return 0;
}

int tcp_local_test(int argc, char *argv[]) {
	tcp_channel *channel = NULL;
	tcp_set_error_callback(process_error, &channel);
  if (streq(argv[1], "host")) {
    tcp_init();
    tcp_server *serv = tcp_open_server("localhost", "3000", 1);
    printf("Started Server\n");
    tcp_channel *client = tcp_accept(serv, 5000);
    printf("Connected a Client\n");
    printf("Started HTTP Packet Stream\n");
    usleep(16000);
    tcp_send(client, "Hello World from Server!", 24, 0);
    usleep(16000);
    tcp_send(client, "Hello World from Server!", 24, 0);
    usleep(16000);
    tcp_send(client, "Hello World from Server!", 24, 0);
    usleep(16000);
    tcp_send(client, "exit                    ", 24, 0);
    tcp_close_channel(client);
    tcp_close_server(serv);
	  tcp_close_channel(channel);
    tcp_term();
    return 0;
  }
  else if (streq(argv[1], "client")) {
    tcp_init();
    tcp_channel *server = tcp_connect("localhost", "3000");
    printf("Connected to Server\n");
    printf("Started HTTP Packet Stream\n");
    while (strstr(BUFFER, "exit") == NULL) {
      tcp_stream_receive(server, print_buffer, NULL, 0);
    }
    tcp_close_channel(server);
	  tcp_close_channel(channel);
    tcp_term();
    return 0;
  }
  return 0;
}
