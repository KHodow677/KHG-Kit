#include "tcp_test.h"
#include "khg_dbm/util.h"
#include "khg_tcp/error.h"
#include "khg_tcp/tcp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SERVER "localhost"
#define PORT "12345"

void process_error(tcp_error e, void *user_data) {
	tcp_channel **channel = (tcp_channel **) user_data;
	perror(tcp_error_to_string(e));
	tcp_close_channel(*channel);
	tcp_term();
	exit(-1);
}

bool print_buffer(const char *buffer, int length, void *user_data) {
	(void) user_data;
	return fwrite(buffer, sizeof(char), length, stdout) == (size_t) length;
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

int tcp_server_test(int argc, char *argv[]) {
  if (streq(argv[1], "host")) {
    tcp_channel *channel = NULL;
    tcp_set_error_callback(process_error, &channel);
    tcp_init();
    tcp_server *serv = tcp_open_server("localhost", "https", 1);
    printf("Started Server\n");
    tcp_channel *client = tcp_accept(serv, 5000);
    printf("Connected a Client\n");
    tcp_send(client, "Hello World!", strlen("Hello World!"), 5000);
    printf("Sent Message to Client\n");
    tcp_close_channel(channel);
    tcp_term();
    return 0;
  }
  else if (streq(argv[1], "client")) {
    tcp_channel *channel = NULL;
    tcp_set_error_callback(process_error, &channel);
    tcp_init();
    tcp_channel *server = tcp_connect("localhost", "https");
    printf("Connected to Server\n");
    char buffer[TCP_STREAM_BUFFER_SIZE];
    tcp_receive(server, buffer, strlen("Hello World!"), 5000);
    printf("Received Message from Server: %s\n", buffer);
    tcp_close_channel(channel);
    tcp_term();
    return 0;
  }
  return 0;
}
