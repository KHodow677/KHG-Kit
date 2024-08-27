#include "khg_tcp/error.h"
#include "khg_tcp/tcp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
