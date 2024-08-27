#include "khg_tcp/tcp.h"
#include "khg_tcp/native/native_types.h"
#include "khg_tcp/socket.h"

#include <assert.h>
#include <string.h>
#include <stdlib.h>

typedef struct tcp_channel {
	socket_t socket;
} tcp_channel;

typedef struct tcp_server {
	socket_t socket;
} tcp_server;

static bool init = false;

bool tcp_init() {
	if (!init) {
		tcp_socket_init();
		init = true;
	}
	return true;
}

void tcp_term() {
	if (init) {
		tcp_socket_term();
		init = false;
	}
}

tcp_server *tcp_open_server(const char *address, const char *protocol, int max_pending_channels) {
	tcp_server *server = (tcp_server *)malloc(sizeof(tcp_server));
	assert(server);
	assert(address);
	assert(max_pending_channels > 0);
	server->socket = tcp_socket_create();
	tcp_socket_bind(&server->socket, address, protocol);
	tcp_socket_listen(&server->socket, max_pending_channels);
	return server;
}

tcp_channel *tcp_accept(tcp_server *server, int timeout_ms) {
	assert(server);
	if (!tcp_socket_poll_read(&server->socket, timeout_ms)) {
		return NULL;
  }
	tcp_channel *channel = (tcp_channel *)malloc(sizeof(tcp_channel));
	assert(channel);
	channel->socket = tcp_socket_accept(&server->socket);
	return channel;
}

void tcp_close_server(tcp_server *server) {
	if (server) {
		tcp_socket_close(&server->socket);
		free(server);
	}
}

tcp_channel *tcp_connect(const char *address, const char *protocol) {
	tcp_channel *channel = (tcp_channel *)malloc(sizeof(tcp_channel));
	assert(channel);
	assert(address);
	assert(protocol);
	channel->socket = tcp_socket_create();
	tcp_socket_connect(&channel->socket, address, protocol);
	return channel;
}

bool tcp_send(tcp_channel *channel, const char *buffer, int length, int timeout_ms) {
	assert(channel);
	assert(buffer);
	assert(length > 0);
	if (!tcp_socket_poll_write(&channel->socket, timeout_ms)) {
		return false;
  }
	int bytes_sent = 0;
	while (bytes_sent < length) {
		int ret = tcp_socket_write(&channel->socket, buffer + bytes_sent, length - bytes_sent);
		if (ret == 0) {
			return false;
    }
		bytes_sent += ret;
	}
	return true;
}

int tcp_receive(tcp_channel *channel, char *buffer, int length, int timeout_ms) {
	assert(channel);
	assert(buffer);
	assert(length > 0);
	if (!tcp_socket_poll_read(&channel->socket, timeout_ms)) {
		return 0;
  }
	return tcp_socket_read(&channel->socket, buffer, length);
}

bool tcp_stream_receive(tcp_channel *channel, stream_output_fn stream_output, void *user_data, int timeout_ms) {
	assert(channel);
	assert(stream_output);
	if (!tcp_socket_poll_read(&channel->socket, timeout_ms)) {
		return false;
  }
	char buffer[TCP_STREAM_BUFFER_SIZE];
	const int length = TCP_STREAM_BUFFER_SIZE;
	do {
		int bytes_received = tcp_socket_read(&channel->socket, buffer, length);
		if (bytes_received == 0) {
			return false;
    }
		if (!stream_output(buffer, bytes_received, user_data)) {
			return false;
    }
	} while (tcp_socket_poll_read(&channel->socket, 0));
	return true;
}

void tcp_close_channel(tcp_channel *channel) {
	if (channel) {
		tcp_socket_close(&channel->socket);
		free(channel);
	}
}

