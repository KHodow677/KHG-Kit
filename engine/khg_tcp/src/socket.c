#include "khg_tcp/socket.h"
#include "khg_tcp/native/native.h"
#include "khg_tcp/error.h"

#include <assert.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

typedef struct timeval timeval;
typedef struct sockaddr sockaddr;
typedef struct addrinfo addrinfo;

static bool init = false;

void tcp_socket_init() {
  if (!init) {
#if defined(_WIN32) || defined(_WIN64)
    WSADATA data;
    int err = WSAStartup(MAKEWORD(2, 2), &data);
    if (err != 0) {
      TCP_FAIL(err);
    }
#endif
    init = true;
  }
}

void tcp_socket_term() {
  if (init) {
#ifdef _WIN32
    int err = WSACleanup();
    if (err != 0) {
      TCP_FAIL(err);
    }
#endif
    init = false;
  }
}

static timeval make_timeout(int timeout_ms) {
  timeval timeout;
  if (timeout_ms < 0) {
    timeout.tv_sec = LONG_MAX;
    timeout.tv_usec = LONG_MAX;
  }
  else {
    timeout.tv_sec = timeout_ms / 1000;
    timeout_ms -= timeout.tv_sec * 1000;
    timeout.tv_usec = timeout_ms * 1000;
  }
  return timeout;
}

static fd_set make_socket_set(const socket_t *sockets, int n) {
  assert(sockets);
  assert(n < FD_SETSIZE);
  fd_set socket_set;
  FD_ZERO(&socket_set);
  for (int i = 0; i < n; ++i) {
    assert(sockets[i] != TCP_INVALID_SOCKET);
    FD_SET(sockets[i], &socket_set);
  }
  return socket_set;
}

static sockaddr init_address(const char *name, const char *protocol) {
  assert(name || protocol);
  addrinfo hints;
  memset(&hints, 0, sizeof(addrinfo));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  addrinfo* info = NULL;
  sockaddr ret;
  if (0 == getaddrinfo(name, protocol, &hints, &info)) {
    ret = *info->ai_addr;
    freeaddrinfo(info);
  }
  else {
    freeaddrinfo(info);
    TCP_FAIL_LAST_ERROR();
  }
  return ret;
}

socket_t tcp_socket_create() {
  socket_t s = socket(AF_INET, SOCK_STREAM, 0);
  if (s != TCP_INVALID_SOCKET) {
    unsigned long int mode = 1;
    if (0 != TCP_SET_NON_BLOCKING(s, &mode)) {
      TCP_FAIL_LAST_ERROR();
    }
  }
  else {
    TCP_FAIL_LAST_ERROR();
  }
  return s;
}

socket_t tcp_socket_accept(const socket_t *server) {
  assert(server);
  socket_t s = accept(*server, NULL, NULL);
  if (s != TCP_INVALID_SOCKET) {
    unsigned long int mode = 1;
    if (0 != TCP_SET_NON_BLOCKING(s, &mode)) {
      TCP_FAIL_LAST_ERROR();
    }
  }
  else {
    TCP_FAIL_LAST_ERROR();
  }
  return s;
}

void tcp_socket_connect(const socket_t *s, const char *address, const char *protocol) {
  assert(s);
  sockaddr addr = init_address(address, protocol);
  if (0 != connect(*s, &addr, sizeof(addr))) {
    tcp_error err = tcp_get_last_error();
    if (err != TCP_EWOULDBLOCK && err != TCP_EINPROGRESS) {
      TCP_FAIL_LAST_ERROR();
    }
  }
  else {
    TCP_FAIL_LAST_ERROR();
  }
}

void tcp_socket_bind(const socket_t *s, const char *address, const char *protocol) {
  assert(s);
  sockaddr addr = init_address(address, protocol);
  if (0 != bind(*s, &addr, sizeof(addr))) {
    TCP_FAIL_LAST_ERROR();
  }
}

void tcp_socket_listen(const socket_t *s, int max_pending_channels) {
  assert(s);
  assert(max_pending_channels > 0);
  if (0 != listen(*s, max_pending_channels)) {
    TCP_FAIL_LAST_ERROR();
  }
}

void tcp_socket_shutdown(const socket_t *s) {
  assert(s);
  if (0 != TCP_SHUTDOWN_SOCKET(*s)) {
    TCP_FAIL_LAST_ERROR();
  }
}

bool tcp_socket_poll_write(const socket_t *socket, int timeout_ms) {
  return tcp_socket_poll_write_n(socket, 1, timeout_ms);
}

bool tcp_socket_poll_write_n(const socket_t *sockets, int n, int timeout_ms) {
  assert(sockets);
  assert(n > 0);
  fd_set socket_set = make_socket_set(sockets, n);
  timeval timeout = make_timeout(timeout_ms);
  int sockets_ready = select(FD_SETSIZE, NULL, &socket_set, NULL, &timeout);
  if (sockets_ready == n) {
    return true;
  }
  else if (sockets_ready == -1) {
    TCP_FAIL_LAST_ERROR();
    return false;
  }
  else {
    if (timeout_ms != 0) {
      tcp_raise_error(TCP_ETIMEDOUT);
    }
    return false;
  }
}

bool tcp_socket_poll_read(const socket_t *socket, int timeout_ms) {
  return tcp_socket_poll_read_n(socket, 1, timeout_ms);
}

bool tcp_socket_poll_read_no_timeout(const socket_t *socket) {
  return tcp_socket_poll_read_n_no_timeout(socket, 1);
}

bool tcp_socket_poll_read_n(const socket_t *sockets, int n, int timeout_ms) {
  assert(sockets);
  assert(n > 0);
  fd_set socket_set = make_socket_set(sockets, n);
  timeval timeout = make_timeout(timeout_ms);
  int sockets_ready = select(FD_SETSIZE, &socket_set, NULL, NULL, &timeout);
  if (sockets_ready == n) {
    return true;
  }
  else if (sockets_ready == -1) {
    TCP_FAIL_LAST_ERROR();
    return false;
  }
  else {
    if (timeout_ms != 0) {
      tcp_raise_error(TCP_ETIMEDOUT);
    }
    return false;
  }
}

bool tcp_socket_poll_read_n_no_timeout(const socket_t *sockets, int n) {
  assert(sockets);
  assert(n > 0);
  fd_set socket_set = make_socket_set(sockets, n);
  timeval timeout = make_timeout(0);
  int sockets_ready = select(FD_SETSIZE, &socket_set, NULL, NULL, &timeout);
  if (sockets_ready == n) {
    return true;
  }
  else if (sockets_ready == -1) {
    TCP_FAIL_LAST_ERROR();
    return false;
  }
  else {
    return false;
  }
}

int tcp_socket_write(const socket_t *s, const char *buffer, int n) {
  assert(s);
  int bytes_sent = send(*s, buffer, n, 0);
  if (bytes_sent == -1) {
    tcp_raise_error(tcp_get_last_error());
    return 0;
  }
  return bytes_sent;
}

int tcp_socket_read(const socket_t *s, char *buffer, int n) {
  int bytes_received = recv(*s, buffer, n, 0);
  if (bytes_received == -1) {
    tcp_raise_error(tcp_get_last_error());
    return 0;
  }
  return bytes_received;
}

void tcp_socket_close(socket_t *s) {
  assert(s);
  if (*s != TCP_INVALID_SOCKET) {
    TCP_CLOSE_SOCKET(*s);
    *s = -1;
  }
}

