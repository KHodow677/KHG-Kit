#pragma once

#include "khg_tcp/native/native_types.h"
#include <stdbool.h>

void tcp_socket_init(void);
void tcp_socket_term(void);

socket_t tcp_socket_create(void);
socket_t tcp_socket_accept(const socket_t *server);

void tcp_socket_connect(const socket_t *s, const char *address, const char *protocol);
void tcp_socket_bind(const socket_t *s, const char *address, const char *protocol);
void tcp_socket_listen(const socket_t *s, int max_pending_channels);
void tcp_socket_shutdown(const socket_t *s);

bool tcp_socket_poll_write(const socket_t *socket, int timeout_ms);
bool tcp_socket_poll_write_n(const socket_t *sockets, int n, int timeout_ms);
bool tcp_socket_poll_read(const socket_t *s, int timeout_ms);
bool tcp_socket_poll_read_n(const socket_t *sockets, int n, int timeout_ms);

int tcp_socket_write(const socket_t *s, const char *buffer, int n);
int tcp_socket_read(const socket_t *s, char* buffer, int n);

void tcp_socket_close(socket_t *s);

