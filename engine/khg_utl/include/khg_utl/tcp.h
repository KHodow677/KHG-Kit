#pragma once

#include "openssl/types.h"
#include <stddef.h> 
#include <stdbool.h>

#define MAX_SSL_CONNECTIONS 256

#if defined(_WIN32) || defined(_WIN64)
  #include <winsock2.h>
  #include <ws2tcpip.h>
  typedef SOCKET TcpSocket;
#else
  #include <sys/types.h>
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
  #include <netinet/tcp.h>
  #include <unistd.h>
  #include <netdb.h>
  typedef int TcpSocket;
#endif

typedef enum {
  TCP_SUCCESS = 0,
  TCP_ERR_SOCKET,
  TCP_ERR_BIND,
  TCP_ERR_LISTEN,
  TCP_ERR_ACCEPT,
  TCP_ERR_CONNECT,
  TCP_ERR_SEND,
  TCP_ERR_RECV,
  TCP_ERR_CLOSE,
  TCP_ERR_SETUP,
  TCP_ERR_RESOLVE,
  TCP_ERR_GENERIC,
  TCP_ERR_SSL, 
  TCP_ERR_SSL_HANDSHAKE, 
  TCP_ERR_TRY_AGAIN, 
  TCP_ERR_NO_SSL, 
  TCP_ERR_UNSUPPORTED,       
  TCP_ERR_WOULD_BLOCK,
} TcpStatus;

typedef enum {
  TCP_SHUTDOWN_RECEIVE = 0,
  TCP_SHUTDOWN_SEND,
  TCP_SHUTDOWN_BOTH
} TcpShutdownHow;

typedef enum {
  TCP_TIMEOUT_RECV = 1,
  TCP_TIMEOUT_SEND = 2,
  TCP_TIMEOUT_BOTH = 3
} TcpTimeoutOperation;

typedef struct {
  TcpStatus code;
  int sys_errno;
  char message[256];
} TcpStatusInfo;

typedef struct {
  TcpSocket socket;
  SSL *ssl;
} SocketSSLMapping;

TcpStatus tcp_init(void); 
TcpStatus tcp_cleanup(void);
TcpStatus tcp_socket_create(TcpSocket *socket);
TcpStatus tcp_bind(TcpSocket socket, const char *host, unsigned short port);
TcpStatus tcp_listen(TcpSocket socket, int backlog);
TcpStatus tcp_accept(TcpSocket socket, TcpSocket *client_socket);
TcpStatus tcp_connect(TcpSocket socket, const char *host, unsigned short port);
TcpStatus tcp_send(TcpSocket socket, const void *buf, size_t len, size_t *sent);
TcpStatus tcp_recv(TcpSocket socket, void *buf, size_t len, size_t* received);
TcpStatus tcp_close(TcpSocket socket);
TcpStatus tcp_set_non_blocking(TcpSocket socket, bool enable);
TcpStatus tcp_set_timeout(TcpSocket socket, TcpTimeoutOperation operation, long timeout_ms);
TcpStatus tcp_resolve_hostname(const char *hostname, char *ip_address, size_t ip_address_len);
TcpStatus tcp_shutdown(TcpSocket socket, TcpShutdownHow how);
TcpStatus tcp_get_local_address(TcpSocket socket, char *address, size_t address_len, unsigned short *port);
TcpStatus tcp_get_remote_address(TcpSocket socket, char *address, size_t address_len, unsigned short *port);
TcpStatus tcp_set_reuse_addr(TcpSocket socket, bool enabled);
TcpStatus tcp_get_peer_name(TcpSocket socket, char *host, size_t host_len, unsigned short *port);
TcpStatus tcp_get_sock_name(TcpSocket socket, char *host, size_t host_len, unsigned short *port);
TcpStatus tcp_enable_ssl(TcpSocket socket);
TcpStatus tcp_disable_ssl(TcpSocket socket);
TcpStatus tcp_async_send(TcpSocket socket, const void *buf, size_t len);
TcpStatus tcp_async_recv(TcpSocket socket, void* buf, size_t len);
TcpStatus tcp_get_connection_quality(TcpSocket socket, float *rtt, float *variance);
TcpStatus tcp_ssl_init(const char *cert, const char *key);
TcpStatus tcp_ssl_cleanup(void);
TcpStatus tcp_ssl_connect(TcpSocket socket, const char *host);
TcpStatus tcp_ssl_accept(TcpSocket socket);
TcpStatus tcp_ssl_send(TcpSocket socket, const void *buf, size_t len, size_t *sent);
TcpStatus tcp_ssl_recv(TcpSocket socket, void *buf, size_t len, size_t *received);
TcpStatus tcp_ssl_close(TcpSocket socket);

void tcp_get_last_error(TcpStatusInfo *status_info);
void tcp_set_ssl(TcpSocket socket, SSL *ssl);

SSL *tcp_get_ssl(TcpSocket socket);
bool tcp_is_valid_address(const char *address);

