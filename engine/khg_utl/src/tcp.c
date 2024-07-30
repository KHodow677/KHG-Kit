#include "khg_utl/tcp.h"
#include "khg_utl/error_func.h"
#include "openssl/ssl.h"
#include "openssl/err.h"
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#define TCP_INVALID_SOCKET (~(TcpSocket)0)

static SocketSSLMapping sslMappings[MAX_SSL_CONNECTIONS];
static SSL_CTX *ssl_ctx = NULL;

static void initialize_ssl_mappings() {
  for (int i = 0; i < MAX_SSL_CONNECTIONS; i++) {
    sslMappings[i].socket = -1;
    sslMappings[i].ssl = NULL;
  }
}

static SocketSSLMapping *get_ssl_mapping(TcpSocket socket, bool allocate) {
  SocketSSLMapping *emptySlot = NULL;
  for (int i = 0; i < MAX_SSL_CONNECTIONS; i++) {
    if (sslMappings[i].socket == socket) {
      return &sslMappings[i];
    } 
    else if (!emptySlot && sslMappings[i].socket == TCP_INVALID_SOCKET && allocate) {
      emptySlot = &sslMappings[i];
    }
  }
  if (allocate && emptySlot) {
    emptySlot->socket = socket;
    return emptySlot;
  }
  return NULL;
}

static void tcp_format_error_message(TcpStatusInfo *status_info) {
  if (!status_info) {
    error_func("Status infor is null and invalid in tcp_format_error_message", user_defined_data);
  }
#if defined(_WIN32) || defined(_WIN64)
  FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, status_info->sys_errno, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)status_info->message, sizeof(status_info->message), NULL);
#else
  strerror_r(status_info->sys_errno, status_info->message, sizeof(status_info->message));
#endif
}

void tcp_get_last_error(TcpStatusInfo *status_info) {
  if (!status_info) {
    error_func("Status info param is null and invalid in tcp_get_last_error", user_defined_data);
    return;
  }
#if defined(_WIN32) || defined(_WIN64)
  status_info->sys_errno = WSAGetLastError();
#else
  status_info->sys_errno = errno;
#endif
  tcp_format_error_message(status_info);
}

bool tcp_is_valid_address(const char *address) {
  struct sockaddr_in sa;
  struct sockaddr_in6 sa6;
  if (inet_pton(AF_INET, address, &(sa.sin_addr)) == 1) {
    return true;
  }
  if (inet_pton(AF_INET6, address, &(sa6.sin6_addr)) == 1) {
    return true;
  }
  error_func("Address is not a valid IPv4 or IPv6 address", user_defined_data);
  return false;
}

TcpStatus tcp_socket_create(TcpSocket *sock) {
  if (!sock) {
    error_func("Socket parameter is null in tcp_socket_create", user_defined_data);
    return TCP_ERR_SOCKET;
  }
#if defined(_WIN32) || defined(_WIN64)
  *socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
  if (*socket == INVALID_SOCKET) {
    error_func("Socket creation failed", user_defined_data);
    return TCP_ERR_SOCKET;
  }
#else
  *sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (*sock < 0) {
    error_func("Socket creation failed", user_defined_data);
    return TCP_ERR_SOCKET;
  }
#endif
  return TCP_SUCCESS;
}

TcpStatus tcp_bind(TcpSocket socket, const char *host, unsigned short port) {
  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  if (host == NULL || strcmp(host, "") == 0 || strcmp(host, "0.0.0.0") == 0) {
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
  } 
  else {
    if (!inet_pton(AF_INET, host, &addr.sin_addr)) {
      error_func("Invalid host address", user_defined_data);
      return TCP_ERR_RESOLVE;
    }
  }
  if (bind(socket, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    error_func("Binding failed", user_defined_data);
    return TCP_ERR_BIND;
  }
  return TCP_SUCCESS;
}

TcpStatus tcp_listen(TcpSocket socket, int backlog) {
  if (listen(socket, backlog) < 0) {
    error_func("Listening on socket failed", user_defined_data);
    return TCP_ERR_LISTEN;
  }
  return TCP_SUCCESS;
}

TcpStatus tcp_accept(TcpSocket socket, TcpSocket *client_socket) {
    if (!client_socket) {
      error_func("Client socket parameter is null", user_defined_data);
      return TCP_ERR_ACCEPT;
    }
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    memset(&client_addr, 0, sizeof(client_addr));
    *client_socket = accept(socket, (struct sockaddr *)&client_addr, &client_addr_len);
    if (*client_socket == TCP_INVALID_SOCKET) {
      TcpStatusInfo status_info;
      tcp_get_last_error(&status_info);
#if defined(_WIN32) || defined(_WIN64)
      if (status_info.sys_errno == WSAEWOULDBLOCK) {
        error_func("Non-blocking socket operation could not be completed immediately", user_defined_data);
        return TCP_ERR_WOULD_BLOCK; // Assuming you define TCP_ERR_WOULD_BLOCK
      }
#else
      if (status_info.sys_errno == EAGAIN || status_info.sys_errno == EWOULDBLOCK) {
        error_func("Non-blocking socket operation could not be completed immediately", user_defined_data);
        return TCP_ERR_WOULD_BLOCK; // Assuming you define TCP_ERR_WOULD_BLOCK
      }
#endif
      error_func("Accepting connection failed", user_defined_data);
      return TCP_ERR_ACCEPT;
    }
    return TCP_SUCCESS;
}

TcpStatus tcp_connect(TcpSocket socket, const char* host, unsigned short port) {
  if (host == NULL || host[0] == '\0') {
    error_func("Host parameter is null or empty in tcp_connect", user_defined_data);
    return TCP_ERR_RESOLVE;
  }
  struct sockaddr_in server_addr;
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  if (inet_pton(AF_INET, host, &server_addr.sin_addr) <= 0) {
    error_func("Invalid server address", user_defined_data);
    return TCP_ERR_RESOLVE;
  }
  if (connect(socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
    error_func("Connecting to server failed", user_defined_data);
    return TCP_ERR_CONNECT;
  }
  return TCP_SUCCESS;
}

TcpStatus tcp_init(void) {
#if defined(_WIN32) || defined(_WIN64)
  WSADATA wsaData;
  int result = WSAStartup(MAKEWORD(2,2), &wsaData);
  if (result != 0) {
    error_func("WSAStartup failed", user_defined_data);
    return TCP_ERR_SETUP;
  }
#endif
  return TCP_SUCCESS;
}

TcpStatus tcp_cleanup(void) {
#if defined(_WIN32) || defined(_WIN64)
  WSACleanup();
#endif
  return TCP_SUCCESS;
}

TcpStatus tcp_send(TcpSocket socket, const void *buf, size_t len, size_t *sent) {
  if (!buf || len == 0) {
    error_func("Buffer is null or length is 0", user_defined_data);
    return TCP_ERR_SEND;
  }
  size_t bytes_sent = 0;
  while (len > 0) {
#if defined(_WIN32) || defined(_WIN64)
    WSABUF wsaBuf;
    wsaBuf.buf = (CHAR *)buf;
    wsaBuf.len = (ULONG)len;
    DWORD flags = 0;
    DWORD bytesSent = 0;
    int result = WSASend(socket, &wsaBuf, 1, &bytesSent, flags, NULL, NULL);
    if (result == SOCKET_ERROR) {
      if (sent) {
        *sent = (size_t)bytes_sent;
      }
      error_func("Sending data failed", user_defined_data);
      return TCP_ERR_SEND;
    }
    bytes_sent += bytesSent;
#else
    bytes_sent = send(socket, buf, len, 0);
    if (bytes_sent <= 0) {
      if (bytes_sent == 0) {
        return TCP_ERR_CLOSE;
      }
      if (sent) {
        *sent = (size_t)bytes_sent;
      }
      error_func("Sending data failed", user_defined_data);
      return TCP_ERR_SEND;
    }
#endif
    buf = (const char *)buf + bytes_sent;
    len -= bytes_sent;
  }
  if (sent) {
    *sent = (size_t)bytes_sent;
  }
  return TCP_SUCCESS;
}

TcpStatus tcp_recv(TcpSocket socket, void *buf, size_t len, size_t *received) {
  if (!buf || len == 0) {
    error_func("Buffer is null or length is 0", user_defined_data);
    return TCP_ERR_RECV;
  }
  size_t bytes_received = 0;
#if defined(_WIN32) || defined(_WIN64)
  WSABUF wsaBuf;
  wsaBuf.buf = (CHAR*)buf;
  wsaBuf.len = (ULONG)len;
  DWORD flags = 0;
  DWORD bytesRecvd = 0;
  int result = WSARecv(socket, &wsaBuf, 1, &bytesRecvd, &flags, NULL, NULL);
  if (result == SOCKET_ERROR) {
      bytes_received = SOCKET_ERROR;
  } else {
      bytes_received = (size_t)bytesRecvd;
  }
#else
  bytes_received = recv(socket, buf, len, 0);
#endif
  if (bytes_received > 0) {
    if (received) {
      *received = (size_t)bytes_received;
    }
    return TCP_SUCCESS;
  } 
  else if (bytes_received == 0) {
    error_func("Connection closed by peer", user_defined_data);
    return TCP_ERR_CLOSE;
  } 
  else {
    error_func("Receiving data failed", user_defined_data);
    return TCP_ERR_RECV;
  }
}

TcpStatus tcp_close(TcpSocket socket) {
  int result = 0;
#if defined(_WIN32) || defined(_WIN64)
  result = closesocket(socket);
  if (result == SOCKET_ERROR) {
    error_func("Closing socket failed", user_defined_data);
    return TCP_ERR_CLOSE;
  }
#else
  result = close(socket);
  if (result < 0) {
    error_func("Closing socket failed", user_defined_data);
    return TCP_ERR_CLOSE;
  }
#endif
  return TCP_SUCCESS;
}

TcpStatus tcp_shutdown(TcpSocket socket, TcpShutdownHow how) {
  int shutdownHow;
#if defined(_WIN32) || defined(_WIN64)
  switch (how) {
    case TCP_SHUTDOWN_RECEIVE:
      shutdownHow = SD_RECEIVE;
      break;
    case TCP_SHUTDOWN_SEND:
      shutdownHow = SD_SEND;
      break;
    case TCP_SHUTDOWN_BOTH:
      shutdownHow = SD_BOTH;
      break;
    default:
      error_func("Invalid shutdown operation specified", user_defined_data);
      return TCP_ERR_GENERIC;
  }
#else
  switch (how) {
    case TCP_SHUTDOWN_RECEIVE:
      shutdownHow = SHUT_RD;
      break;
    case TCP_SHUTDOWN_SEND:
      shutdownHow = SHUT_WR;
      break;
    case TCP_SHUTDOWN_BOTH:
      shutdownHow = SHUT_RDWR;
      break;
    default:
      error_func("Invalid shutdown operation specified in tcp_shutdown", user_defined_data);
      return TCP_ERR_GENERIC;
  }
#endif
  if (shutdown(socket, shutdownHow) == -1) {
    error_func("Shutting down socket failed", user_defined_data);
    return TCP_ERR_GENERIC;
  }
  return TCP_SUCCESS;
}

TcpStatus tcp_set_timeout(TcpSocket socket, TcpTimeoutOperation operation, long timeout_ms) {
  int result;
#ifdef _WIN32
  DWORD timeout = (DWORD)timeout_ms;
#else
  struct timeval timeout;
  timeout.tv_sec = timeout_ms / 1000;
  timeout.tv_usec = (timeout_ms % 1000) * 1000;
#endif
  if (operation == TCP_TIMEOUT_RECV || operation == TCP_TIMEOUT_BOTH) {
    result = setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
    if (result < 0) {
      error_func("Error: Setting receive timeout failed", user_defined_data);
      return TCP_ERR_GENERIC;
    }
  }
  if (operation == TCP_TIMEOUT_SEND || operation == TCP_TIMEOUT_BOTH) {
    result = setsockopt(socket, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout, sizeof(timeout));
    if (result < 0) {
      error_func("Setting send timeout failed", user_defined_data);
      return TCP_ERR_GENERIC;
    }
  }
  return TCP_SUCCESS;
}

TcpStatus tcp_resolve_hostname(const char *hostname, char *ip_address, size_t ip_address_len) {
  struct addrinfo hints, *res, *p;
  int status;
  void *addr;
  if (!hostname || !ip_address) {
    error_func("Null parameter provided", user_defined_data);
    return TCP_ERR_GENERIC;
  }
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  if ((status = getaddrinfo(hostname, NULL, &hints, &res)) != 0) {
    error_func("Error in getaddrinfo", user_defined_data);
    return TCP_ERR_RESOLVE;
  }
  for(p = res; p != NULL; p = p->ai_next) {
    if (p->ai_family == AF_INET) {
      struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
      addr = &(ipv4->sin_addr);
    } 
    else {
      struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
      addr = &(ipv6->sin6_addr);
    }
    if (inet_ntop(p->ai_family, addr, ip_address, ip_address_len) == NULL) {
      error_func("Error converting IP address to string", user_defined_data);
      freeaddrinfo(res); 
      return TCP_ERR_GENERIC;
    }
    break;
  }
  freeaddrinfo(res); 
  if (p == NULL) { 
    error_func("No addresses found", user_defined_data);
    return TCP_ERR_RESOLVE;
  }
  return TCP_SUCCESS;
}

TcpStatus tcp_set_non_blocking(TcpSocket socket, bool enable) {
#if defined(_WIN32) || defined(_WIN64)
  u_long mode = enable ? 1 : 0; // Non-zero value for non-blocking mode
  if (ioctlsocket(socket, FIONBIO, &mode) != NO_ERROR) {
    error_func("Setting non-blocking mode failed", user_defined_data);
    return TCP_ERR_GENERIC;
  }
#else
  int flags = fcntl(socket, F_GETFL, 0);
  if (flags == -1) {
    error_func("Getting socket flags failed", user_defined_data);
    return TCP_ERR_GENERIC;
  }
  flags = enable ? (flags | O_NONBLOCK) : (flags & ~O_NONBLOCK);
  if (fcntl(socket, F_SETFL, flags) == -1) {
    error_func("Setting non-blocking mode failed", user_defined_data);
    return TCP_ERR_GENERIC;
  }
#endif
  return TCP_SUCCESS;
}

TcpStatus tcp_get_local_address(TcpSocket socket, char *address, size_t address_len, unsigned short *port) {
  struct sockaddr_storage addr;
  socklen_t len = sizeof(addr);
  if (!address || address_len == 0 || !port) {
    error_func("Invalid parameter provided to tcp_get_local_address", user_defined_data);
    return TCP_ERR_GENERIC;
  }
  memset(&addr, 0, sizeof(addr));
  if (getsockname(socket, (struct sockaddr*)&addr, &len) == -1) {
    error_func("Getsockname() failed", user_defined_data);
    return TCP_ERR_GENERIC;
  }
  if (addr.ss_family == AF_INET) {
    struct sockaddr_in *ipv4 = (struct sockaddr_in *)&addr;
    if (inet_ntop(AF_INET, &ipv4->sin_addr, address, address_len) == NULL) {
      error_func("Inet_ntop() failed for IPv4 address", user_defined_data);
      return TCP_ERR_GENERIC;
    }
    *port = ntohs(ipv4->sin_port);
  } 
  else if (addr.ss_family == AF_INET6) {
      struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)&addr;
      if (inet_ntop(AF_INET6, &ipv6->sin6_addr, address, address_len) == NULL) {
        error_func("Inet_ntop() failed for IPv6 address", user_defined_data);
        return TCP_ERR_GENERIC;
      }
      *port = ntohs(ipv6->sin6_port);
  } 
  else {
    error_func("Unknown socket family", user_defined_data);
    return TCP_ERR_GENERIC;
  }
  return TCP_SUCCESS;
}

TcpStatus tcp_get_remote_address(TcpSocket socket, char *address, size_t address_len, unsigned short *port) {
  struct sockaddr_storage addr;
  socklen_t len = sizeof(addr);
  if (!address || address_len == 0 || !port) {
    error_func("Error: Invalid parameter provided to tcp_get_remote_address", user_defined_data);
    return TCP_ERR_GENERIC;
  }
  memset(&addr, 0, sizeof(addr));
  if (getpeername(socket, (struct sockaddr*)&addr, &len) == -1) {
    error_func("Getpeername() failed", user_defined_data);
    return TCP_ERR_GENERIC;
  }
  if (addr.ss_family == AF_INET) {
    struct sockaddr_in *ipv4 = (struct sockaddr_in *)&addr;
    if (inet_ntop(AF_INET, &ipv4->sin_addr, address, address_len) == NULL) {
      error_func("Inet_ntop() failed for IPv4 address", user_defined_data);
      return TCP_ERR_GENERIC;
    }
    *port = ntohs(ipv4->sin_port);
  } 
  else if (addr.ss_family == AF_INET6) {
    struct sockaddr_in6 *ipv6 = (struct sockaddr_in6*)&addr;
    if (inet_ntop(AF_INET6, &ipv6->sin6_addr, address, address_len) == NULL) {
      error_func("Inet_ntop() failed for IPv6 address", user_defined_data);
      return TCP_ERR_GENERIC;
    }
    *port = ntohs(ipv6->sin6_port);
  } 
  else {
    error_func("Unknown socket family", user_defined_data);
    return TCP_ERR_GENERIC;
  }
  return TCP_SUCCESS;
}

TcpStatus tcp_set_reuse_addr(TcpSocket socket, bool enabled) {
  int optval = enabled ? 1 : 0;
  int result;
#if defined(_WIN32) || defined(_WIN64)
  result = setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, (char *)&optval, sizeof(optval));
#else
  result = setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
#endif
  if (result < 0) {
    error_func("Setting SO_REUSEADDR failed", user_defined_data);
    return TCP_ERR_GENERIC;
  }
  return TCP_SUCCESS;
}

TcpStatus tcp_get_peer_name(TcpSocket socket, char *host, size_t host_len, unsigned short *port) {
  struct sockaddr_storage addr;
  socklen_t len = sizeof(addr);
  if (getpeername(socket, (struct sockaddr *)&addr, &len) != 0) {
    error_func("Getpeername() failed", user_defined_data);
    return TCP_ERR_GENERIC;
  }
  if (addr.ss_family == AF_INET) {
    struct sockaddr_in *s = (struct sockaddr_in *)&addr;
    if (inet_ntop(AF_INET, &s->sin_addr, host, host_len) == NULL) {
      error_func("Inet_ntop() failed for IPv4 address", user_defined_data);
      return TCP_ERR_GENERIC;
    }
    *port = ntohs(s->sin_port);
  } 
  else if (addr.ss_family == AF_INET6) {
    struct sockaddr_in6* s = (struct sockaddr_in6*)&addr;
    if (inet_ntop(AF_INET6, &s->sin6_addr, host, host_len) == NULL) {
      error_func("Inet_ntop() failed for IPv6 address", user_defined_data);
      return TCP_ERR_GENERIC;
    }
    *port = ntohs(s->sin6_port);
  } 
  else {
    error_func("Unknown socket family", user_defined_data);
    return TCP_ERR_GENERIC;
  }
  return TCP_SUCCESS;
}

TcpStatus tcp_get_sock_name(TcpSocket socket, char *host, size_t host_len, unsigned short *port) {
  struct sockaddr_storage addr;
  socklen_t len = sizeof(addr);
  if (getsockname(socket, (struct sockaddr *)&addr, &len) != 0) {
    error_func("Getsockname() failed", user_defined_data);
    return TCP_ERR_GENERIC;
  }
  if (addr.ss_family == AF_INET) {
    struct sockaddr_in *s = (struct sockaddr_in *)&addr;
    if (inet_ntop(AF_INET, &s->sin_addr, host, host_len) == NULL) {
      error_func("Inet_ntop() failed for IPv4 address", user_defined_data);
      return TCP_ERR_GENERIC;
    }
    *port = ntohs(s->sin_port);
  } 
  else if (addr.ss_family == AF_INET6) {
    struct sockaddr_in6 *s = (struct sockaddr_in6 *)&addr;
    if (inet_ntop(AF_INET6, &s->sin6_addr, host, host_len) == NULL) {
      error_func("Inet_ntop() failed for IPv6 address", user_defined_data);
      return TCP_ERR_GENERIC;
    }
    *port = ntohs(s->sin6_port);
  } 
  else {
    error_func("Unknown socket family", user_defined_data);
    return TCP_ERR_GENERIC;
  }
  return TCP_SUCCESS;
}

void tcp_set_ssl(TcpSocket socket, SSL *ssl) {
  SocketSSLMapping *mapping = get_ssl_mapping(socket, true);
  if (mapping) {
    mapping->ssl = ssl;
  }
}

SSL *tcp_get_ssl(TcpSocket socket) {
  SocketSSLMapping *mapping = get_ssl_mapping(socket, false);
  return mapping ? mapping->ssl : NULL;
}

TcpStatus tcp_enable_ssl(TcpSocket socket) {
  if (ssl_ctx == NULL) {
    error_func("SSL context is not initialized", user_defined_data);
    return TCP_ERR_SETUP;
  }
  SSL *ssl = SSL_new(ssl_ctx);
  if (!ssl) {
    error_func("Failed to create SSL object", user_defined_data);
    return TCP_ERR_GENERIC;
  }
  if (SSL_set_fd(ssl, socket) == 0) {
    error_func("Failed to associate socket with SSL", user_defined_data);
    SSL_free(ssl);
    return TCP_ERR_GENERIC;
  }
  tcp_set_ssl(socket, ssl);
  return TCP_SUCCESS;
}

TcpStatus tcp_disable_ssl(TcpSocket socket) {
  SSL *ssl = tcp_get_ssl(socket);
  if (ssl == NULL) {
    error_func("No SSL object associated with the socket", user_defined_data);
    return TCP_ERR_NO_SSL;
  }
  int shutdownResult = SSL_shutdown(ssl);
  if (shutdownResult == 0) {
    SSL_shutdown(ssl);
  }
  SSL_free(ssl);
  tcp_set_ssl(socket, NULL);
  return TCP_SUCCESS;
}

TcpStatus tcp_ssl_init(const char *cert, const char *key) {
  if (ssl_ctx != NULL) {
    error_func("SSL context is already initialized", user_defined_data);
    return TCP_SUCCESS; 
  }
  SSL_load_error_strings();   
  OpenSSL_add_ssl_algorithms();
  ssl_ctx = SSL_CTX_new(TLS_server_method()); 
  if (!ssl_ctx) {
    error_func("Error creating SSL context", user_defined_data);
    return TCP_ERR_SETUP; 
  }
  if (SSL_CTX_use_certificate_file(ssl_ctx, cert, SSL_FILETYPE_PEM) <= 0) {
    error_func("Error loading certificate from file", user_defined_data);
    SSL_CTX_free(ssl_ctx);
    ssl_ctx = NULL; 
    return TCP_ERR_SSL;
  }
  if (SSL_CTX_use_PrivateKey_file(ssl_ctx, key, SSL_FILETYPE_PEM) <= 0) {
    error_func("Error loading private key from file", user_defined_data);
    SSL_CTX_free(ssl_ctx);
    ssl_ctx = NULL; 
    return TCP_ERR_SSL;
  }
  if (!SSL_CTX_check_private_key(ssl_ctx)) {
    error_func("Private key does not match the public certificate", user_defined_data);
    SSL_CTX_free(ssl_ctx);
    ssl_ctx = NULL; 
    return TCP_ERR_SSL;
  }
  initialize_ssl_mappings();
  return TCP_SUCCESS;
}

TcpStatus tcp_ssl_cleanup(void) {
  if (ssl_ctx != NULL) {
    SSL_CTX_free(ssl_ctx);
    ssl_ctx = NULL;
  }
  EVP_cleanup();
  ERR_free_strings();
  return TCP_SUCCESS;
}

TcpStatus tcp_ssl_connect(TcpSocket socket, const char *host) {
  if (ssl_ctx == NULL) {
    error_func("SSL context is not initialized", user_defined_data);
    return TCP_ERR_SETUP;
  }
  if (host == NULL || host[0] == '\0') {
    error_func("Host parameter is null or empty", user_defined_data);
    return TCP_ERR_RESOLVE;
  }
  SSL *ssl = SSL_new(ssl_ctx);
  if (ssl == NULL) {
    error_func("Failed to create SSL object", user_defined_data);
    return TCP_ERR_SSL;
  }
  if (SSL_set_fd(ssl, socket) == 0) {
    error_func("Failed to set file descriptor for SSL", user_defined_data);
    SSL_free(ssl); 
    return TCP_ERR_SSL; 
  }
  if (SSL_set_tlsext_host_name(ssl, host) == 0) {
    error_func("Failed to set SNI Hostname", user_defined_data);
    SSL_free(ssl); 
    return TCP_ERR_SSL; 
  }
  if (SSL_connect(ssl) != 1) {
    error_func("SSL handshake failed", user_defined_data);
    SSL_free(ssl);
    return TCP_ERR_SSL_HANDSHAKE;
  }
  SocketSSLMapping *mapping = get_ssl_mapping(socket, true);
  if (!mapping) {
    error_func("Failed to map SSL object with socket", user_defined_data);
    SSL_free(ssl);
    return TCP_ERR_SSL;
  }
  mapping->ssl = ssl;
  return TCP_SUCCESS;
}

TcpStatus tcp_ssl_accept(TcpSocket socket) {
  if (ssl_ctx == NULL) {
    error_func("SSL context is not initialized", user_defined_data);
    return TCP_ERR_SETUP;
  }
  SSL *ssl = SSL_new(ssl_ctx);
  if (ssl == NULL) {
    error_func("Failed to create SSL object", user_defined_data);
    return TCP_ERR_SSL;
  }
  if (SSL_set_fd(ssl, socket) == 0) {
    error_func("Failed to set file descriptor for SSL", user_defined_data);
    SSL_free(ssl);
    return TCP_ERR_SSL;
  }
  int acceptResult = SSL_accept(ssl);
  if (acceptResult <= 0) {
    int sslError = SSL_get_error(ssl, acceptResult);
    error_func("SSL_accept failed with SSL error", user_defined_data);
    if (sslError == SSL_ERROR_SYSCALL) {
      unsigned long err;
      while ((err = ERR_get_error()) != 0) {
        error_func("OpenSSL Error", user_defined_data);
      }
      if (errno != 0) {
        error_func("Syscall error", user_defined_data);
      }
    }
    SSL_free(ssl);
    return TCP_ERR_SSL_HANDSHAKE;
  }
  SocketSSLMapping *mapping = get_ssl_mapping(socket, true);
  if (!mapping) {
    error_func("Failed to map SSL object with socket", user_defined_data);
    SSL_free(ssl); 
    return TCP_ERR_SSL; 
  }
  mapping->ssl = ssl;
  return TCP_SUCCESS;
}

TcpStatus tcp_ssl_close(TcpSocket socket) {
  SSL *ssl = tcp_get_ssl(socket);
  if (!ssl) {
    error_func("No SSL object associated with the socket. Closing socket without SSL shutdown", user_defined_data);
    tcp_close(socket);
    return TCP_ERR_NO_SSL;
  }
  int shutdownResult = SSL_shutdown(ssl);
  if (shutdownResult == 0) {
    shutdownResult = SSL_shutdown(ssl);
    if (shutdownResult != 1) {
      error_func("SSL shutdown did not complete cleanly", user_defined_data);
    }
  } 
  else if (shutdownResult < 0) {
    error_func("SSL shutdown failed", user_defined_data);
  }
  SSL_free(ssl);
  tcp_set_ssl(socket, NULL);
  TcpStatus closeStatus = tcp_close(socket);
  if (closeStatus != TCP_SUCCESS) {
    error_func("Socket close failed", user_defined_data);
    return closeStatus;
  }
  return TCP_SUCCESS;
}

TcpStatus tcp_ssl_send(TcpSocket socket, const void *buf, size_t len, size_t *sent) {
  if (!buf || len == 0) {
    error_func("Invalid buffer or length for SSL send", user_defined_data);
    return TCP_ERR_SEND;
  }
  SSL *ssl = tcp_get_ssl(socket);
  if (!ssl) {
    error_func("No SSL object associated with the socket. Cannot send data", user_defined_data);
    return TCP_ERR_NO_SSL;
  }
  int totalSent = 0;
  int result = 0;
  const char *dataPtr = (const char *)buf;
  while (len > 0) {
    result = SSL_write(ssl, dataPtr, len);
    if (result <= 0) {
      int sslError = SSL_get_error(ssl, result);
      switch (sslError) {
        case SSL_ERROR_WANT_WRITE:
        case SSL_ERROR_WANT_READ:
          error_func("SSL_write needs to be called again", user_defined_data);
          if (sent) {
            *sent = totalSent;
          }
          return TCP_SUCCESS;
        case SSL_ERROR_ZERO_RETURN:
          error_func("SSL connection closed by peer", user_defined_data);
          return TCP_ERR_CLOSE;
        case SSL_ERROR_SYSCALL:
          error_func("SSL write syscall error", user_defined_data);
          return TCP_ERR_SEND;
        default:
          error_func("SSL write error", user_defined_data);
          return TCP_ERR_SEND;
      }
    }
    totalSent += result;
    dataPtr += result;
    len -= result;
  }
  if (sent) {
    *sent = totalSent;
  }
  return TCP_SUCCESS;
}

TcpStatus tcp_ssl_recv(TcpSocket socket, void *buf, size_t len, size_t *received) {
  if (!buf || len == 0) {
    error_func("Invalid buffer or length for SSL receive", user_defined_data);
    return TCP_ERR_RECV;
  }
  SSL *ssl = tcp_get_ssl(socket);
  if (!ssl) {
    error_func("No SSL object associated with the socket. Cannot receive data", user_defined_data);
    return TCP_ERR_NO_SSL;
  }
  int result = SSL_read(ssl, buf, len);
  if (result > 0) {
    if (received) {
      *received = result;
    }
    return TCP_SUCCESS;
  } 
  else {
    int sslError = SSL_get_error(ssl, result);
    switch (sslError) {
      case SSL_ERROR_WANT_READ:
      case SSL_ERROR_WANT_WRITE:
        if (received) {
          *received = 0;
        }
        error_func("SSL_read needs to be called again", user_defined_data);
        return TCP_SUCCESS;
      case SSL_ERROR_ZERO_RETURN:
        error_func("SSL connection closed by peer", user_defined_data);
        return TCP_ERR_CLOSE;
      case SSL_ERROR_SYSCALL: {
        if (ERR_peek_error() == 0) {
          if (result == 0 || errno == 0) {
            error_func("SSL connection closed by peer or EOF reached", user_defined_data);
            SSL_free(ssl);
            tcp_set_ssl(socket, NULL);
            tcp_close(socket);
            return TCP_ERR_CLOSE;
          } 
          else {
            error_func("SSL syscall error", user_defined_data);
          }
        }
        error_func("SSL read syscall error", user_defined_data);
        return TCP_ERR_RECV;
      }
      default:
        error_func("SSL read error", user_defined_data);
        return TCP_ERR_RECV;
    }
  }
}

TcpStatus tcp_get_connection_quality(TcpSocket socket, float *rtt, float *variance) {
#if defined(__linux__)
  struct tcp_info info;
  socklen_t len = sizeof(info);
  memset(&info, 0, sizeof(info));
  if (getsockopt(socket, IPPROTO_TCP, TCP_INFO, (void *)&info, &len) == 0) {
    *rtt = info.tcpi_rtt / 1000.0;
    *variance = info.tcpi_rttvar / 1000.0;
    return TCP_SUCCESS;
  } 
  else {
    error_func("Failed to get TCP connection quality", user_defined_data);
    return TCP_ERR_GENERIC;
  }
#elif defined(_WIN32)
  (void)socket;
  (void)rtt;
  (void)variance;
  error_func("Direct RTT measurement not supported on Windows", user_defined_data);
  return TCP_ERR_UNSUPPORTED;
#else
  error_func("Your platform does not support direct RTT measurement via socket options", user_defined_data);
  return TCP_ERR_UNSUPPORTED;
#endif
}

TcpStatus tcp_async_send(TcpSocket socket, const void *buf, size_t len) {
  size_t result = send(socket, buf, len, 0);
  if (result == TCP_INVALID_SOCKET) {
#ifdef _WIN32
    int lastError = WSAGetLastError();
    if (lastError == WSAEWOULDBLOCK) {
      error_func("Connection is blocked", user_defined_data);
      return TCP_ERR_WOULD_BLOCK;
    }
#else
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      error_func("Connection is blocked", user_defined_data);
      return TCP_ERR_WOULD_BLOCK;
    }
#endif
    return TCP_ERR_SEND;
  }
  return TCP_SUCCESS;
}

TcpStatus tcp_async_recv(TcpSocket socket, void *buf, size_t len) {
  size_t result = recv(socket, buf, len, 0);
  if (result == TCP_INVALID_SOCKET) {
#ifdef _WIN32
    int lastError = WSAGetLastError();
    if (lastError == WSAEWOULDBLOCK) {
      error_func("No data available to read; non-blocking operation", user_defined_data);
      return TCP_ERR_WOULD_BLOCK;
    }
#else
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      error_func("No data available to read; non-blocking operation", user_defined_data);
      return TCP_ERR_WOULD_BLOCK;
    }
#endif
    return TCP_ERR_RECV;
  } 
  else if (result == 0) {
    return TCP_ERR_RECV;
  }
  return TCP_SUCCESS;
}
