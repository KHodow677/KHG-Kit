#include "khg_tcp/stcp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void process_error(stcp_error e, void *user_data) {
	stcp_channel **channel = (stcp_channel **) user_data;
	perror(stcp_error_to_string(e));
	stcp_close_channel(*channel);
	stcp_terminate();
	exit(-1);
}

bool print_buffer(const char *buffer, int length, void *user_data) {
	(void) user_data;
	return fwrite(buffer, sizeof(char), length, stdout) == (size_t) length;
}

int tcp_test() {
	stcp_channel *channel = NULL;
	stcp_set_error_callback(process_error, &channel);
	stcp_initialize();
	const char *request = "HEAD / HTTP/1.2\r\n\r\n";
	channel = stcp_connect("www.google.com", "http");
	stcp_send(channel, request, strlen(request), 500);
	stcp_stream_receive(channel, print_buffer, NULL, 500);
	stcp_close_channel(channel);
	stcp_terminate();
	return 0;
}
