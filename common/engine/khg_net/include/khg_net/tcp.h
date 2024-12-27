#pragma once

#if defined(_WIN32) || defined(_WIN64)
#include <winsock2.h>
#include <ws2tcpip.h>
typedef SOCKET net_tcp_socket;
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
typedef int32_t net_tcp_socket;
#endif

#include "openssl/crypto.h"
#include <stddef.h> 
#include <stdbool.h>

#define NET_MAX_SSL_CONNECTIONS 256
#define NET_TCP_INVALID_SOCKET (~(net_tcp_socket)0)

typedef enum net_tcp_status {
  NET_TCP_SUCCESS,
  NET_TCP_ERR_SOCKET,
  NET_TCP_ERR_BIND,
  NET_TCP_ERR_LISTEN,
  NET_TCP_ERR_ACCEPT,
  NET_TCP_ERR_CONNECT,
  NET_TCP_ERR_SEND,
  NET_TCP_ERR_RECV,
  NET_TCP_ERR_CLOSE,
  NET_TCP_ERR_SETUP,
  NET_TCP_ERR_RESOLVE,
  NET_TCP_ERR_GENERIC,
  NET_TCP_ERR_SSL,
  NET_TCP_ERR_SSL_HANDSHAKE,
  NET_TCP_ERR_TRY_AGAIN,
  NET_TCP_ERR_NO_SSL,
  NET_TCP_ERR_UNSUPPORTED, 
  NET_TCP_ERR_WOULD_BLOCK,
} net_tcp_status;

typedef enum net_tcp_shutdown_how {
  NET_TCP_SHUTDOWN_RECEIVE,
  NET_TCP_SHUTDOWN_SEND,
  NET_TCP_SHUTDOWN_BOTH
} net_tcp_shutdown_how;

typedef enum net_tcp_timeout_operation {
  NET_TCP_TIMEOUT_RECV = 1,
  NET_TCP_TIMEOUT_SEND = 2,
  NET_TCP_TIMEOUT_BOTH = 3
} net_tcp_timeout_operation;

typedef struct net_tcp_status_info {
  net_tcp_status code;
  int sys_errno;
  char message[256];
} net_tcp_status_info;

typedef struct net_socket_ssl_mapping {
  net_tcp_socket socket;
  SSL* ssl;
} net_socket_ssl_mapping;

net_tcp_status net_tcp_init(void); 
net_tcp_status net_tcp_cleanup(void);
net_tcp_status net_tcp_socket_create(net_tcp_socket *socket);
net_tcp_status net_tcp_bind(net_tcp_socket socket, const char *host, uint16_t port);
net_tcp_status net_tcp_listen(net_tcp_socket socket, int32_t backlog);
net_tcp_status net_tcp_accept(net_tcp_socket socket, net_tcp_socket *client_socket);
net_tcp_status net_tcp_connect(net_tcp_socket socket, const char *host, uint16_t port);
net_tcp_status net_tcp_send(net_tcp_socket socket, const void *buf, size_t len, size_t *sent);
net_tcp_status net_tcp_recv(net_tcp_socket socket, void *buf, size_t len, size_t *received);
net_tcp_status net_tcp_close(net_tcp_socket socket);
net_tcp_status net_tcp_set_non_blocking(net_tcp_socket socket, bool enable);
net_tcp_status net_tcp_set_timeout(net_tcp_socket socket, net_tcp_timeout_operation operation, uint32_t timeout_ms);
net_tcp_status net_tcp_resolve_hostname(const char *hostname, char *ip_address, size_t ip_address_len);
net_tcp_status net_tcp_shutdown(net_tcp_socket socket, net_tcp_shutdown_how how);
net_tcp_status net_tcp_get_local_address(net_tcp_socket socket, char *address, size_t address_len, uint16_t *port);
net_tcp_status net_tcp_get_remote_address(net_tcp_socket socket, char *address, size_t address_len, uint16_t *port);
net_tcp_status net_tcp_set_reuse_address(net_tcp_socket socket, bool enabled);
net_tcp_status net_tcp_get_peer_name(net_tcp_socket socket, char *host, size_t host_len, uint16_t *port);
net_tcp_status net_tcp_get_sock_name(net_tcp_socket socket, char *host, size_t host_len, uint16_t *port);
net_tcp_status net_tcp_enable_ssl(net_tcp_socket socket);
net_tcp_status net_tcp_disable_ssl(net_tcp_socket socket);
net_tcp_status net_tcp_async_send(net_tcp_socket socket, const void *buf, size_t len);
net_tcp_status net_tcp_async_recv(net_tcp_socket socket, void* buf, size_t len);
net_tcp_status net_tcp_get_connection_quality(net_tcp_socket socket, float *rtt, float *variance);
net_tcp_status net_tcp_ssl_init(const char *cert, const char *key);
net_tcp_status net_tcp_ssl_cleanup(void);
net_tcp_status net_tcp_ssl_connect(net_tcp_socket socket, const char *host);
net_tcp_status net_tcp_ssl_accept(net_tcp_socket socket);
net_tcp_status net_tcp_ssl_send(net_tcp_socket socket, const void *buf, size_t len, size_t *sent);
net_tcp_status net_tcp_ssl_recv(net_tcp_socket socket, void *buf, size_t len, size_t *received);
net_tcp_status net_tcp_ssl_close(net_tcp_socket socket);

void net_tcp_get_last_error(net_tcp_status_info *status_info);
void net_tcp_set_ssl(net_tcp_socket socket, SSL *ssl);

SSL *net_tcp_get_ssl(net_tcp_socket socket);
bool net_tcp_is_valid_address(const char *address);

