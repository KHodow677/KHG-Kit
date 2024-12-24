#pragma once

#include <stdbool.h>

#define TCP_STREAM_BUFFER_SIZE 2048

typedef struct tcp_channel tcp_channel;
typedef struct tcp_server tcp_server;

typedef bool (*stream_output_fn)(const char *buffer, int length, void *user_data);

bool tcp_init(void);
void tcp_term(void);

tcp_server *tcp_open_server(const char *address, const char *protocol, int max_pending_channels);
tcp_channel *tcp_accept(tcp_server *server, int timeout_ms);
void tcp_close_server(tcp_server *server);

tcp_channel *tcp_connect(const char *address, const char *protocol);
bool tcp_send(tcp_channel *channel, const char *buffer, int length, int timeout_ms);
int tcp_receive(tcp_channel *channel, char *buffer, int length, int timeout_ms);
bool tcp_stream_receive(tcp_channel *channel, stream_output_fn stream_output, void *user_data, int timeout_ms);

void tcp_close_channel(tcp_channel *channel);

