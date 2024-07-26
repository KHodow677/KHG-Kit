#pragma once

#if defined(__linux__) || defined(__APPLE__)

#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#ifdef MSG_MAXIOVLEN
#define ENET_BUFFER_MAXIMUM MSG_MAXIOVLEN
#endif

typedef int net_socket;

#define NET_SOCKET_NULL -1

#define net_host_to_net_16(value) (htons (value)) /**< macro that converts host to net byte-order of a 16-bit value */
#define net_host_to_net_32(value) (htonl (value)) /**< macro that converts host to net byte-order of a 32-bit value */

#define net_net_to_host_16(value) (ntohs (value)) /**< macro that converts net to host byte-order of a 16-bit value */
#define net_net_to_host_32(value) (ntohl (value)) /**< macro that converts net to host byte-order of a 32-bit value */

typedef struct {
  void *data;
  size_t dataLength;
} net_buffer;

#define NET_CALLBACK

typedef fd_set net_socket_set;

#define net_socketset_empty(sockset) FD_ZERO (&(sockset))
#define net_socketset_add(sockset, socket) FD_SET (socket, &(sockset))
#define net_socketset_remove(sockset, socket) FD_CLR (socket, &(sockset))
#define net_socketset_check(sockset, socket) FD_ISSET (socket, &(sockset))

#endif
