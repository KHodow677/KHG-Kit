#include "tcp_test.h"
#include "khg_dbm/util.h"
#include "khg_tcp/error.h"
#include "khg_tcp/tcp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char BUFFER[TCP_STREAM_BUFFER_SIZE] = "";

void process_error(tcp_error e, void *user_data) {
	tcp_channel **channel = (tcp_channel **) user_data;
	perror(tcp_error_to_string(e));
	tcp_close_channel(*channel);
	tcp_term();
	exit(-1);
}

bool print_buffer(const char *buffer, int length, void *user_data) {
	(void) user_data;
  strncpy(BUFFER, buffer, length);
  printf("%s\n", BUFFER);
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

int tcp_remote_test() {
	tcp_channel *channel = NULL;
	tcp_set_error_callback(process_error, &channel);
	tcp_init();
	const char *request = "GET /something HTTP/1.1\r\nHost: khgsvr.fly.dev\r\n\r\n";
	channel = tcp_connect("khgsvr.fly.dev", "http");
	tcp_send(channel, request, strlen(request), 500);
	tcp_stream_receive(channel, print_buffer, NULL, 500);
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
