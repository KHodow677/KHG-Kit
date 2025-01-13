#include "khg_net/tcp.h"
#include "khg_utl/error_func.h"
#include <stdlib.h>
#include <string.h>

#if defined(WIN32) || defined(WIN64)
#else
#include <errno.h>
#endif

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

