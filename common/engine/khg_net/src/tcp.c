#include "khg_net/tcp.h"
#include "khg_utl/error_func.h"
#include "openssl/ssl.h"
#include "openssl/err.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>

static net_socket_ssl_mapping SSL_MAPPINGS[NET_MAX_SSL_CONNECTIONS];
static SSL_CTX *SSL_CONTEXT = NULL;

static void net_initialize_ssl_mappings() {
  for (unsigned int i = 0; i < NET_MAX_SSL_CONNECTIONS; i++) {
    SSL_MAPPINGS[i].socket = -1;
    SSL_MAPPINGS[i].ssl = NULL;
  }
}

static net_socket_ssl_mapping *net_get_ssl_mapping(net_tcp_socket socket, bool allocate) {
  net_socket_ssl_mapping *empty_slot = NULL;
  for (unsigned int i = 0; i < NET_MAX_SSL_CONNECTIONS; i++) {
    if (SSL_MAPPINGS[i].socket == socket) {
      return &SSL_MAPPINGS[i];
    } 
    else if (!empty_slot && SSL_MAPPINGS[i].socket == NET_TCP_INVALID_SOCKET && allocate) {
      empty_slot = &SSL_MAPPINGS[i];
    }
  }
  if (allocate && empty_slot) {
    empty_slot->socket = socket;
    return empty_slot;
  }
  return NULL;
}

static void net_tcp_format_error_message(net_tcp_status_info *status_info) {
  if (!status_info) {
    utl_error_func("Status information is null and invalid", utl_user_defined_data);
    return;
  }
#if defined(_WIN32) || defined(_WIN64)
  FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, status_info->sys_errno, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)status_info->message, sizeof(status_info->message), NULL);
#else
  strerror_r(status_info->sys_errno, status_info->message, sizeof(status_info->message));
#endif
}

void net_tcp_get_last_error(net_tcp_status_info *status_info) {
  if (!status_info) {
    utl_error_func("Status info param is null and invalid", utl_user_defined_data);
    return;
  }
#if defined(_WIN32) || defined(_WIN64)
  status_info->sys_errno = WSAGetLastError();
#else
  status_info->sys_errno = errno;
#endif
  net_tcp_format_error_message(status_info);
}

bool net_tcp_is_valid_address(const char *address) {
  struct sockaddr_in sa;
  struct sockaddr_in6 sa6;
  if (inet_pton(AF_INET, address, &(sa.sin_addr)) == 1) {
    return true; 
  }
  if (inet_pton(AF_INET6, address, &(sa6.sin6_addr)) == 1) {
    return true; 
  }
  return false;
}

net_tcp_status net_tcp_socket_create(net_tcp_socket *sock) {
  if (!sock) {
    utl_error_func("Socket parameter is null", utl_user_defined_data);
    return NET_TCP_ERR_SOCKET;
  }
#if defined(_WIN32) || defined(_WIN64)
  *sock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
  if (*sock == NET_TCP_INVALID_SOCKET) {
    utl_error_func("Socket creation failed", utl_user_defined_data);
    return NET_TCP_ERR_SOCKET;
  }
#else
  *sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (*sock < 0) {
    utl_error_func("Socket creation failed", utl_user_defined_data);
    return NET_TCP_ERR_SOCKET;
  }
#endif
  return NET_TCP_SUCCESS;
}

net_tcp_status net_tcp_bind(net_tcp_socket sock, const char *host, unsigned short port) {
  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  if (host == NULL || strcmp(host, "") == 0 || strcmp(host, "0.0.0.0") == 0) {
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
  } 
  else {
    if (!inet_pton(AF_INET, host, &addr.sin_addr)) {
      utl_error_func("Invalid host address", utl_user_defined_data);
      return NET_TCP_ERR_RESOLVE;
    }
  }
  if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    utl_error_func("Binding failed", utl_user_defined_data);
    return NET_TCP_ERR_BIND;
  }
  return NET_TCP_SUCCESS;
}

net_tcp_status net_tcp_listen(net_tcp_socket socket, short backlog) {
  if (listen(socket, backlog) < 0) {
    utl_error_func("Listening on socket failed", utl_user_defined_data);
    return NET_TCP_ERR_LISTEN;
  }
  return NET_TCP_SUCCESS;
}

net_tcp_status net_tcp_accept(net_tcp_socket socket, net_tcp_socket *client_socket) {
  if (!client_socket) {
    utl_error_func("Client socket parameter is null", utl_user_defined_data);
    return NET_TCP_ERR_ACCEPT;
  }
  struct sockaddr_in client_addr;
  socklen_t client_addr_len = sizeof(client_addr);
  memset(&client_addr, 0, sizeof(client_addr));
  *client_socket = accept(socket, (struct sockaddr *)&client_addr, &client_addr_len);
  if (*client_socket == NET_TCP_INVALID_SOCKET) {
    net_tcp_status_info status_info;
    net_tcp_get_last_error(&status_info);
#if defined(_WIN32) || defined(_WIN64)
    if (status_info.sys_errno == WSAEWOULDBLOCK) {
      return NET_TCP_ERR_WOULD_BLOCK;
    }
#else
    if (status_info.sys_errno == EAGAIN || status_info.sys_errno == EWOULDBLOCK) {
      return NET_TCP_ERR_WOULD_BLOCK;
    }
#endif
    utl_error_func("Accepting connection failed", utl_user_defined_data);
    return NET_TCP_ERR_ACCEPT;
  }
  char client_ip[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
  return NET_TCP_SUCCESS;
}

net_tcp_status net_tcp_connect(net_tcp_socket socket, const char *host, unsigned short port) {
  if (host == NULL || host[0] == '\0') {
    utl_error_func("Host parameter is null or empty", utl_user_defined_data);
    return NET_TCP_ERR_RESOLVE; 
  }
  struct sockaddr_in server_addr;
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  if (inet_pton(AF_INET, host, &server_addr.sin_addr) <= 0) {
    utl_error_func("Invalid server address", utl_user_defined_data);
    return NET_TCP_ERR_RESOLVE;
  }
  if (connect(socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
    net_tcp_status_info status_info;
    net_tcp_get_last_error(&status_info);
    utl_error_func("Connecting to server failed", utl_user_defined_data);
    return NET_TCP_ERR_CONNECT;
  }
  return NET_TCP_SUCCESS;
}

net_tcp_status net_tcp_init(void) {
#if defined(_WIN32) || defined(_WIN64)
  WSADATA wsa_data;
  short result = WSAStartup(MAKEWORD(2,2), &wsa_data);
  if (result != 0) {
    utl_error_func("WSAStartup failed", utl_user_defined_data);
    return NET_TCP_ERR_SETUP;
  }
#endif
  return NET_TCP_SUCCESS;
}

net_tcp_status net_tcp_cleanup(void) {
#if defined(_WIN32) || defined(_WIN64)
  WSACleanup();
#endif
  return NET_TCP_SUCCESS;
}

net_tcp_status net_tcp_send(net_tcp_socket socket, const void *buf, unsigned int len, unsigned int *sent) {
  if (!buf || len == 0) {
    utl_error_func("Buffer is null or length is 0", utl_user_defined_data);
    return NET_TCP_ERR_SEND;
  }
  unsigned int bytes_sent = 0;
  while (len > 0) {
#if defined(_WIN32) || defined(_WIN64)
    WSABUF wsaBuf;
    wsaBuf.buf = (CHAR *)buf;
    wsaBuf.len = (ULONG)len;
    DWORD flags = 0;
    DWORD bytesSent = 0;
    short result = WSASend(socket, &wsaBuf, 1, &bytesSent, flags, NULL, NULL);
    if (result == SOCKET_ERROR) {
      if (sent) {
        *sent = (unsigned int)bytes_sent;
      }
      utl_error_func("Sending data failed", utl_user_defined_data);
      return NET_TCP_ERR_SEND;
    }
    bytes_sent += bytesSent;
#else
    bytes_sent = send(socket, buf, len, 0);
    if (bytes_sent <= 0) {
      if (bytes_sent == 0) {
        return NET_TCP_ERR_CLOSE;
      }
      if (sent) {
        *sent = (unsigned int)bytes_sent;
      }
      utl_error_func("Sending data failed", utl_user_defined_data);
      return NET_TCP_ERR_SEND;
    }
#endif
    buf = (const char *)buf + bytes_sent;
    len -= bytes_sent;
  }
  if (sent) {
    *sent = (unsigned int)bytes_sent;
  }
  return NET_TCP_SUCCESS;
}

net_tcp_status net_tcp_recv(net_tcp_socket socket, void *buf, unsigned int len, unsigned int *received) {
  if (!buf || len == 0) {
    utl_error_func("Buffer is null or length is 0", utl_user_defined_data);
    return NET_TCP_ERR_RECV;
  }
  unsigned int bytes_received = 0;
#if defined(_WIN32) || defined(_WIN64)
  WSABUF wsaBuf;
  wsaBuf.buf = (CHAR*)buf;
  wsaBuf.len = (ULONG)len;
  DWORD flags = 0;
  DWORD bytesRecvd = 0;
  short result = WSARecv(socket, &wsaBuf, 1, &bytesRecvd, &flags, NULL, NULL);
  if (result == SOCKET_ERROR) {
    bytes_received = SOCKET_ERROR;
  } 
  else {
    bytes_received = (unsigned int)bytesRecvd;
  }
#else
  bytes_received = recv(socket, buf, len, 0);
#endif
  if (bytes_received > 0) {
    if (received) {
      *received = (unsigned int)bytes_received;
    }
    return NET_TCP_SUCCESS;
  } 
  else if (bytes_received == 0) {
    return NET_TCP_ERR_CLOSE;
  } 
  else {
    utl_error_func("[tcp_recv] Error: Receiving data failed", utl_user_defined_data);
    return NET_TCP_ERR_RECV;
  }
}

net_tcp_status net_tcp_close(net_tcp_socket socket) {
  short result = 0;
#if defined(_WIN32) || defined(_WIN64)
  result = closesocket(socket);
  if (result == SOCKET_ERROR) {
    utl_error_func("Closing socket failed", utl_user_defined_data);
    return NET_TCP_ERR_CLOSE;
  }
#else
  result = close(socket);
  if (result < 0) {
    utl_error_func("Closing socket failed", utl_user_defined_data);
    return NET_TCP_ERR_CLOSE;
  }
#endif
  return NET_TCP_SUCCESS;
}

net_tcp_status net_tcp_shutdown(net_tcp_socket socket, net_tcp_shutdown_how how) {
  short shutdown_how;
#if defined(_WIN32) || defined(_WIN64)
  switch (how) {
    case NET_TCP_SHUTDOWN_RECEIVE:
      shutdown_how = SD_RECEIVE;
      break;
    case NET_TCP_SHUTDOWN_SEND:
      shutdown_how = SD_SEND;
      break;
    case NET_TCP_SHUTDOWN_BOTH:
      shutdown_how = SD_BOTH;
      break;
    default:
      utl_error_func("Invalid shutdown operation specified", utl_user_defined_data);
      return NET_TCP_ERR_GENERIC;
    }
#else
  switch (how) {
    case NET_TCP_SHUTDOWN_RECEIVE:
      shutdown_how = SHUT_RD;
      break;
    case NET_TCP_SHUTDOWN_SEND:
      shutdown_how = SHUT_WR;
      break;
    case NET_TCP_SHUTDOWN_BOTH:
      shutdown_how = SHUT_RDWR;
      break;
    default:
      utl_error_func("Invalid shutdown operation specified", utl_user_defined_data);
      return NET_TCP_ERR_GENERIC;
    }
#endif
  if (shutdown(socket, shutdown_how) == -1) {
    net_tcp_status_info status_info;
    net_tcp_get_last_error(&status_info);
    utl_error_func("Shutting down socket failed", utl_user_defined_data);
    return NET_TCP_ERR_GENERIC;
  }
  return NET_TCP_SUCCESS;
}

net_tcp_status net_tcp_set_timeout(net_tcp_socket socket, net_tcp_timeout_operation operation, unsigned int timeout_ms) {
  short result;
#if defined(_WIN32) || defined(_WIN64)
  DWORD timeout = (DWORD)timeout_ms;
#else
  struct timeval timeout;
  timeout.tv_sec = timeout_ms / 1000;
  timeout.tv_usec = (timeout_ms % 1000) * 1000;
#endif
  if (operation == NET_TCP_TIMEOUT_RECV || operation == NET_TCP_TIMEOUT_BOTH) {
    result = setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof(timeout));
    if (result < 0) {
      utl_error_func("Setting receive timeout failed", utl_user_defined_data);
      return NET_TCP_ERR_GENERIC;
    }
  }
  if (operation == NET_TCP_TIMEOUT_SEND || operation == NET_TCP_TIMEOUT_BOTH) {
    result = setsockopt(socket, SOL_SOCKET, SO_SNDTIMEO, (const char *)&timeout, sizeof(timeout));
    if (result < 0) {
      utl_error_func("Setting send timeout failed", utl_user_defined_data);
      return NET_TCP_ERR_GENERIC;
    }
  }
  return NET_TCP_SUCCESS;
}

net_tcp_status net_tcp_resolve_hostname(const char *hostname, char *ip_address, unsigned int ip_address_len) {
  struct addrinfo hints, *res, *p;
  short status;
  void *addr;
  if (!hostname || !ip_address) {
    utl_error_func("Null parameter provided", utl_user_defined_data);
    return NET_TCP_ERR_GENERIC;
  }
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  if ((status = getaddrinfo(hostname, NULL, &hints, &res)) != 0) {
    utl_error_func("Error getting address info", utl_user_defined_data);
    return NET_TCP_ERR_RESOLVE;
  }
  for (p = res; p != NULL; p = p->ai_next) {
    if (p->ai_family == AF_INET) {
      struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
      addr = &(ipv4->sin_addr);
    } 
    else {
      struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
      addr = &(ipv6->sin6_addr);
    }
    if (inet_ntop(p->ai_family, addr, ip_address, ip_address_len) == NULL) {
      utl_error_func("Error converting IP address to string", utl_user_defined_data);
      freeaddrinfo(res); 
      return NET_TCP_ERR_GENERIC;
    }
    break;
  }
  freeaddrinfo(res); 
  if (p == NULL) { 
    utl_error_func("No addresses found", utl_user_defined_data);
    return NET_TCP_ERR_RESOLVE;
  }
  return NET_TCP_SUCCESS;
}

net_tcp_status net_tcp_set_non_blocking(net_tcp_socket socket, bool enable) {
#if defined(_WIN32) || defined(_WIN64)
  u_long mode = enable ? 1 : 0;
  if (ioctlsocket(socket, FIONBIO, &mode) != NO_ERROR) {
    utl_error_func("Setting non-blocking mode failed", utl_user_defined_data);
    return NET_TCP_ERR_GENERIC;
  }
#else
  short flags = fcntl(socket, F_GETFL, 0);
  if (flags == -1) {
    net_tcp_status_info status_info;
    net_tcp_get_last_error(&status_info);
    utl_error_func("Getting socket flags failed", utl_user_defined_data);
    return NET_TCP_ERR_GENERIC;
  }
  flags = enable ? (flags | O_NONBLOCK) : (flags & ~O_NONBLOCK);
  if (fcntl(socket, F_SETFL, flags) == -1) {
    utl_error_func("Setting non-blocking mode failed", utl_user_defined_data);
    return NET_TCP_ERR_GENERIC;
  }
#endif
  return NET_TCP_SUCCESS;
}

net_tcp_status net_tcp_get_local_address(net_tcp_socket socket, char *address, unsigned int address_len, unsigned short *port) {
  struct sockaddr_storage addr;
  socklen_t len = sizeof(addr);
  if (!address || address_len == 0 || !port) {
    utl_error_func("Invalid parameter provided", utl_user_defined_data);
    return NET_TCP_ERR_GENERIC; 
  }
  memset(&addr, 0, sizeof(addr));
  if (getsockname(socket, (struct sockaddr *)&addr, &len) == -1) {
    utl_error_func("Gettting socket name failed", utl_user_defined_data);
    return NET_TCP_ERR_GENERIC;
  }
  if (addr.ss_family == AF_INET) {
    struct sockaddr_in *ipv4 = (struct sockaddr_in *)&addr;
    if (inet_ntop(AF_INET, &ipv4->sin_addr, address, address_len) == NULL) {
      utl_error_func("Ntop failed for IPv4 address", utl_user_defined_data);
      return NET_TCP_ERR_GENERIC;
    }
    *port = ntohs(ipv4->sin_port);
  } 
  else if (addr.ss_family == AF_INET6) {
    struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)&addr;
    if (inet_ntop(AF_INET6, &ipv6->sin6_addr, address, address_len) == NULL) {
      utl_error_func("Ntop failed for IPv6 address", utl_user_defined_data);
      return NET_TCP_ERR_GENERIC;
    }
    *port = ntohs(ipv6->sin6_port);
  } 
  else {
    utl_error_func("Unknown socket family", utl_user_defined_data);
    return NET_TCP_ERR_GENERIC;
  }
  return NET_TCP_SUCCESS;
}

net_tcp_status net_tcp_get_remote_address(net_tcp_socket socket, char *address, unsigned int address_len, unsigned short *port) {
  struct sockaddr_storage addr;
  socklen_t len = sizeof(addr);
  if (!address || address_len == 0 || !port) {
    utl_error_func("Invalid parameter provided", utl_user_defined_data);
    return NET_TCP_ERR_GENERIC;
  }
  memset(&addr, 0, sizeof(addr));
  if (getpeername(socket, (struct sockaddr *)&addr, &len) == -1) {
    utl_error_func("Getting peer name failed", utl_user_defined_data);
    return NET_TCP_ERR_GENERIC;
  }
  if (addr.ss_family == AF_INET) {
    struct sockaddr_in *ipv4 = (struct sockaddr_in *)&addr;
    if (inet_ntop(AF_INET, &ipv4->sin_addr, address, address_len) == NULL) {
      utl_error_func("Ntop failed for IPv4 address", utl_user_defined_data);
      return NET_TCP_ERR_GENERIC;
    }
    *port = ntohs(ipv4->sin_port);
  } 
  else if (addr.ss_family == AF_INET6) {
    struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)&addr;
    if (inet_ntop(AF_INET6, &ipv6->sin6_addr, address, address_len) == NULL) {
      utl_error_func("Ntop failed for IPv6 address", utl_user_defined_data);
      return NET_TCP_ERR_GENERIC;
    }
    *port = ntohs(ipv6->sin6_port);
  } 
  else {
    utl_error_func("Unknown socket family", utl_user_defined_data);
    return NET_TCP_ERR_GENERIC;
  }
  return NET_TCP_SUCCESS;
}

net_tcp_status net_tcp_set_reuse_address(net_tcp_socket socket, bool enabled) {
  short optval = enabled ? 1 : 0;
  short result;
#if defined(_WIN32) || defined(_WIN64)
  result = setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, (char *)&optval, sizeof(optval));
#else
  result = setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
#endif
  if (result < 0) {
    utl_error_func("Setting reuse address failed", utl_user_defined_data);
    return NET_TCP_ERR_GENERIC;
  }
  return NET_TCP_SUCCESS;
}

net_tcp_status net_tcp_get_peer_name(net_tcp_socket socket, char *host, unsigned int host_len, unsigned short *port) {
  struct sockaddr_storage addr;
  socklen_t len = sizeof(addr);
  if (getpeername(socket, (struct sockaddr *)&addr, &len) != 0) {
    utl_error_func("Gettting peer name failed", utl_user_defined_data);
    return NET_TCP_ERR_GENERIC;
  }
  if (addr.ss_family == AF_INET) {
    struct sockaddr_in *s = (struct sockaddr_in *)&addr;
    if (inet_ntop(AF_INET, &s->sin_addr, host, host_len) == NULL) {
      utl_error_func("Ntop failed for IPv4 address", utl_user_defined_data);
      return NET_TCP_ERR_GENERIC;
    }
    *port = ntohs(s->sin_port);
  } 
  else if (addr.ss_family == AF_INET6) {
    struct sockaddr_in6 *s = (struct sockaddr_in6 *)&addr;
    if (inet_ntop(AF_INET6, &s->sin6_addr, host, host_len) == NULL) {
      utl_error_func("Ntop failed for IPv6 address", utl_user_defined_data);
      return NET_TCP_ERR_GENERIC;
    }
    *port = ntohs(s->sin6_port);
  } 
  else {
    utl_error_func("Unknown socket family", utl_user_defined_data);
    return NET_TCP_ERR_GENERIC;
  }
  return NET_TCP_SUCCESS;
}

net_tcp_status net_tcp_get_sock_name(net_tcp_socket socket, char *host, unsigned int host_len, unsigned short *port) {
  struct sockaddr_storage addr;
  socklen_t len = sizeof(addr);
  if (getsockname(socket, (struct sockaddr *)&addr, &len) != 0) {
    utl_error_func("Gettting socket name failed", utl_user_defined_data);
    return NET_TCP_ERR_GENERIC;
  }
  if (addr.ss_family == AF_INET) {
    struct sockaddr_in *s = (struct sockaddr_in *)&addr;
    if (inet_ntop(AF_INET, &s->sin_addr, host, host_len) == NULL) {
      utl_error_func("Ntop failed for IPv4 address", utl_user_defined_data);
      return NET_TCP_ERR_GENERIC;
    }
    *port = ntohs(s->sin_port);
  } 
  else if (addr.ss_family == AF_INET6) {
    struct sockaddr_in6 *s = (struct sockaddr_in6 *)&addr;
    if (inet_ntop(AF_INET6, &s->sin6_addr, host, host_len) == NULL) {
      utl_error_func("Ntop failed for IPv6 address", utl_user_defined_data);
      return NET_TCP_ERR_GENERIC;
    }
    *port = ntohs(s->sin6_port);
  } 
  else {
    utl_error_func("Unknown socket family", utl_user_defined_data);
    return NET_TCP_ERR_GENERIC;
  }
  return NET_TCP_SUCCESS;
}

void net_tcp_set_ssl(net_tcp_socket socket, SSL *ssl) {
  net_socket_ssl_mapping *mapping = net_get_ssl_mapping(socket, true);
  if (mapping) {
    mapping->ssl = ssl;
  }
}

SSL *net_tcp_get_ssl(net_tcp_socket socket) {
  net_socket_ssl_mapping *mapping = net_get_ssl_mapping(socket, false);
  return mapping ? mapping->ssl : NULL;
}

net_tcp_status net_tcp_enable_ssl(net_tcp_socket socket) {
  if (SSL_CONTEXT == NULL) {
    utl_error_func("SSL context is not initialized", utl_user_defined_data);
    return NET_TCP_ERR_SETUP;
  }
  SSL *ssl = SSL_new(SSL_CONTEXT);
  if (!ssl) {
    utl_error_func("Failed to create SSL object", utl_user_defined_data);
    return NET_TCP_ERR_GENERIC;
  }
  if (SSL_set_fd(ssl, socket) == 0) {
    utl_error_func("Failed to associate socket with SSL", utl_user_defined_data);
    SSL_free(ssl);
    return NET_TCP_ERR_GENERIC;
  }
  net_tcp_set_ssl(socket, ssl);
  return NET_TCP_SUCCESS;
}

net_tcp_status net_tcp_disable_ssl(net_tcp_socket socket) {
  SSL *ssl = net_tcp_get_ssl(socket);
  if (ssl == NULL) {
    utl_error_func("No SSL object associated with the socket", utl_user_defined_data);
    return NET_TCP_ERR_NO_SSL;
  }
  short shutdown_result = SSL_shutdown(ssl);
  if (shutdown_result == 0) {
    SSL_shutdown(ssl);
  }
  SSL_free(ssl);
  net_tcp_set_ssl(socket, NULL);
  return NET_TCP_SUCCESS;
}

net_tcp_status net_tcp_ssl_init(const char *cert, const char *key) {
  if (SSL_CONTEXT != NULL) {
    return NET_TCP_SUCCESS; 
  }
  SSL_load_error_strings();   
  OpenSSL_add_ssl_algorithms();
  SSL_CONTEXT = SSL_CTX_new(TLS_server_method()); 
  if (!SSL_CONTEXT) {
    utl_error_func("Error creating SSL context", utl_user_defined_data);
    return NET_TCP_ERR_SETUP; 
  }
  if (SSL_CTX_use_certificate_file(SSL_CONTEXT, cert, SSL_FILETYPE_PEM) <= 0) {
    utl_error_func("Error loading certificate from file", utl_user_defined_data);
    SSL_CTX_free(SSL_CONTEXT);
    SSL_CONTEXT = NULL; 
    return NET_TCP_ERR_SSL;
  }
  if (SSL_CTX_use_PrivateKey_file(SSL_CONTEXT, key, SSL_FILETYPE_PEM) <= 0) {
    utl_error_func("Error loading private key from file", utl_user_defined_data);
    SSL_CTX_free(SSL_CONTEXT);
    SSL_CONTEXT = NULL; 
    return NET_TCP_ERR_SSL;
  }
  if (!SSL_CTX_check_private_key(SSL_CONTEXT)) {
    utl_error_func("Private key does not match the public certificate", utl_user_defined_data);
    SSL_CTX_free(SSL_CONTEXT);
    SSL_CONTEXT = NULL; 
    return NET_TCP_ERR_SSL;
  }
  net_initialize_ssl_mappings();
  return NET_TCP_SUCCESS;
}

net_tcp_status net_tcp_ssl_cleanup(void) {
  if (SSL_CONTEXT != NULL) {
    SSL_CTX_free(SSL_CONTEXT);
    SSL_CONTEXT = NULL;
  }
  EVP_cleanup();
  ERR_free_strings();
  return NET_TCP_SUCCESS;
}

net_tcp_status net_tcp_ssl_connect(net_tcp_socket socket, const char *host) {
  if (SSL_CONTEXT == NULL) {
    utl_error_func("SSL context is not initialized", utl_user_defined_data);
    return NET_TCP_ERR_SETUP;
  }
  if (host == NULL || host[0] == '\0') {
    utl_error_func("Host parameter is null or empty", utl_user_defined_data);
    return NET_TCP_ERR_RESOLVE;
  }
  SSL *ssl = SSL_new(SSL_CONTEXT);
  if (ssl == NULL) {
    utl_error_func("Failed to create SSL object", utl_user_defined_data);
    return NET_TCP_ERR_SSL;
  }
  if (SSL_set_fd(ssl, socket) == 0) {
    utl_error_func("Failed to set file descriptor for SSL", utl_user_defined_data);
    SSL_free(ssl); 
    return NET_TCP_ERR_SSL; 
  }
  if (SSL_set_tlsext_host_name(ssl, host) == 0) {
    utl_error_func("Failed to set SNI Hostname", utl_user_defined_data);
    SSL_free(ssl); 
    return NET_TCP_ERR_SSL; 
  }
  if (SSL_connect(ssl) != 1) {
    utl_error_func("SSL handshake failed", utl_user_defined_data);
    SSL_free(ssl);
    return NET_TCP_ERR_SSL_HANDSHAKE;
  }
  net_socket_ssl_mapping *mapping = net_get_ssl_mapping(socket, true);
  if (!mapping) {
    utl_error_func("Failed to map SSL object with socket", utl_user_defined_data);
    SSL_free(ssl);
    return NET_TCP_ERR_SSL;
  }
  mapping->ssl = ssl;
  return NET_TCP_SUCCESS;
}

net_tcp_status net_tcp_ssl_accept(net_tcp_socket socket) {
  if (SSL_CONTEXT == NULL) {
    utl_error_func("SSL context is not initialized", utl_user_defined_data);
    return NET_TCP_ERR_SETUP;
  }
  SSL *ssl = SSL_new(SSL_CONTEXT);
  if (ssl == NULL) {
    utl_error_func("Failed to create SSL object", utl_user_defined_data);
    return NET_TCP_ERR_SSL;
  }
  if (SSL_set_fd(ssl, socket) == 0) {
    utl_error_func("Failed to set file descriptor for SSL", utl_user_defined_data);
    SSL_free(ssl);
    return NET_TCP_ERR_SSL;
  }
  short accept_result = SSL_accept(ssl);
  if (accept_result <= 0) {
    short sslError = SSL_get_error(ssl, accept_result);
    utl_error_func("SSL accept failed", utl_user_defined_data);
    if (sslError == SSL_ERROR_SYSCALL) {
      unsigned long long err;
      while ((err = ERR_get_error()) != 0) {
        utl_error_func("OpenSSL Error", utl_user_defined_data);
      }
      if (errno != 0) {
        utl_error_func("System call error", utl_user_defined_data);
      }
    }
    SSL_free(ssl);
    return NET_TCP_ERR_SSL_HANDSHAKE;
  }
  net_socket_ssl_mapping *mapping = net_get_ssl_mapping(socket, true);
  if (!mapping) {
    utl_error_func("Failed to map SSL object with socket", utl_user_defined_data);
    SSL_free(ssl); 
    return NET_TCP_ERR_SSL; 
  }
  mapping->ssl = ssl;
  return NET_TCP_SUCCESS;
}

net_tcp_status net_tcp_ssl_close(net_tcp_socket socket) {
  SSL *ssl = net_tcp_get_ssl(socket);
  if (!ssl) {
    utl_error_func("No SSL object associated with the socket, closing socket without SSL shutdown", utl_user_defined_data);
    net_tcp_close(socket);
    return NET_TCP_ERR_NO_SSL;
  }
  short shutdown_result = SSL_shutdown(ssl);
  if (shutdown_result == 0) {
    shutdown_result = SSL_shutdown(ssl);
    if (shutdown_result != 1) {
      utl_error_func("SSL shutdown did not complete cleanly", utl_user_defined_data);
    }
  } 
  else if (shutdown_result < 0) {
    utl_error_func("SSL shutdown failed", utl_user_defined_data);
  }
  SSL_free(ssl);
  net_tcp_set_ssl(socket, NULL);
  net_tcp_status close_status = net_tcp_close(socket);
  if (close_status != NET_TCP_SUCCESS) {
    utl_error_func("Socket close failed", utl_user_defined_data);
    return close_status;
  }
  return NET_TCP_SUCCESS;
}

net_tcp_status net_tcp_ssl_send(net_tcp_socket socket, const void *buf, unsigned int len, unsigned int *sent) {
  if (!buf || len == 0) {
    utl_error_func("Invalid buffer or length for SSL send", utl_user_defined_data);
    return NET_TCP_ERR_SEND;
  }
  SSL *ssl = net_tcp_get_ssl(socket);
  if (!ssl) {
    utl_error_func("No SSL object associated with the socket", utl_user_defined_data);
    return NET_TCP_ERR_NO_SSL;
  }
  short total_sent = 0;
  short result = 0;
  const char *data_ptr = (const char *)buf;
  while (len > 0) {
    result = SSL_write(ssl, data_ptr, len);
    if (result <= 0) {
      short sslError = SSL_get_error(ssl, result);
      switch (sslError) {
        case SSL_ERROR_WANT_WRITE:
        case SSL_ERROR_WANT_READ:
          if (sent) {
            *sent = total_sent;
          }
          return NET_TCP_SUCCESS;
        case SSL_ERROR_ZERO_RETURN:
          utl_error_func("SSL connection closed by peer", utl_user_defined_data);
          return NET_TCP_ERR_CLOSE;
        case SSL_ERROR_SYSCALL:
          utl_error_func("SSL write system call error", utl_user_defined_data);
          return NET_TCP_ERR_SEND;
        default:
          utl_error_func("SSL write error", utl_user_defined_data);
          return NET_TCP_ERR_SEND;
      }
    }
    total_sent += result;
    data_ptr += result;
    len -= result;
  }
  if (sent) {
    *sent = total_sent;
  }
  return NET_TCP_SUCCESS;
}

net_tcp_status net_tcp_ssl_recv(net_tcp_socket socket, void *buf, unsigned int len, unsigned int *received) {
  if (!buf || len == 0) {
    utl_error_func("Invalid buffer or length for SSL receive", utl_user_defined_data);
    return NET_TCP_ERR_RECV;
  }
  SSL *ssl = net_tcp_get_ssl(socket);
  if (!ssl) {
    utl_error_func("[tcp_ssl_recv] No SSL object associated with the socket", utl_user_defined_data);
    return NET_TCP_ERR_NO_SSL;
  }
  short result = SSL_read(ssl, buf, len);
  if (result > 0) {
    if (received) {
      *received = result;
    }
    return NET_TCP_SUCCESS;
  } 
  else {
    short ssl_error = SSL_get_error(ssl, result);
    switch (ssl_error) {
      case SSL_ERROR_WANT_READ:
      case SSL_ERROR_WANT_WRITE:
        if (received) {
          *received = 0;
        }
        return NET_TCP_SUCCESS;
      case SSL_ERROR_ZERO_RETURN:
        utl_error_func("SSL connection closed by peer", utl_user_defined_data);
        return NET_TCP_ERR_CLOSE;
      case SSL_ERROR_SYSCALL: {
        if (ERR_peek_error() == 0) {
          if (result == 0 || errno == 0) {
            utl_error_func("SSL connection closed by peer or EOF reached", utl_user_defined_data);
            SSL_free(ssl);
            net_tcp_set_ssl(socket, NULL);
            net_tcp_close(socket);
            return NET_TCP_ERR_CLOSE;
          } 
          else {
            utl_error_func("SSL system call error", utl_user_defined_data);
          }
        }
        utl_error_func("SSL read system call error", utl_user_defined_data);
        return NET_TCP_ERR_RECV;
      }
      default:
        utl_error_func("SSL read error", utl_user_defined_data);
        return NET_TCP_ERR_RECV;
    }
  }
}

net_tcp_status net_tcp_get_connection_quality(net_tcp_socket socket, float *rtt, float *variance) {
#if defined(_WIN32) || defined(_WIN64)
  utl_error_func("Direct RTT measurement not supported on Windows", utl_user_defined_data);
  return NET_TCP_ERR_UNSUPPORTED;
#else
  struct tcp_info info;
  socklen_t len = sizeof(info);
  memset(&info, 0, sizeof(info));
  if (getsockopt(socket, IPPROTO_TCP, TCP_INFO, (void *)&info, &len) == 0) {
    *rtt = info.tcpi_rtt / 1000.0;
    *variance = info.tcpi_rttvar / 1000.0;
    return NET_TCP_SUCCESS;
  } 
  else {
    utl_error_func("Failed to get TCP connection quality", utl_user_defined_data);
    return NET_TCP_ERR_GENERIC;
  }
#endif
}

net_tcp_status net_tcp_async_send(net_tcp_socket socket, const void *buf, unsigned int len) {
  unsigned int result = send(socket, buf, len, 0);
  if (result == (unsigned int)NET_TCP_INVALID_SOCKET) {
#if defined(_WIN32) || defined(_WIN64)
    short lastError = WSAGetLastError();
    if (lastError == WSAEWOULDBLOCK) {
      utl_error_func("Connection is blocked", utl_user_defined_data);
      return NET_TCP_ERR_WOULD_BLOCK;
    }
#else
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      utl_error_func("Connection is blocked", utl_user_defined_data);
      return NET_TCP_ERR_WOULD_BLOCK;
    }
#endif
    return NET_TCP_ERR_SEND;
  }
  return NET_TCP_SUCCESS;
}

net_tcp_status net_tcp_async_recv(net_tcp_socket socket, void *buf, unsigned int len) {
  unsigned int result = recv(socket, buf, len, 0);
  if (result == (unsigned int)NET_TCP_INVALID_SOCKET) {
#if defined(_WIN32) || defined(_WIN64)
    short last_error = WSAGetLastError();
    if (last_error == WSAEWOULDBLOCK) {
      utl_error_func("No data available to read", utl_user_defined_data);
      return NET_TCP_ERR_WOULD_BLOCK;
    }
#else
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      utl_error_func("No data available to read", utl_user_defined_data);
      return NET_TCP_ERR_WOULD_BLOCK;
    }
#endif
    return NET_TCP_ERR_RECV;
  } 
  else if (result == 0) {
    return NET_TCP_ERR_RECV;
  }
  return NET_TCP_SUCCESS;
}

