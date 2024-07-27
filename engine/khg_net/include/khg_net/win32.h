#pragma once

#if defined(_WIN32)

#ifdef _MSC_VER
#ifdef NET_BUILDING_LIB
#pragma warning (disable: 4267)
#pragma warning (disable: 4244)
#pragma warning (disable: 4018)
#pragma warning (disable: 4146)
#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE
#endif
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#endif
#endif

#include <winsock2.h>

typedef SOCKET ENetSocket;

#define NET_SOCKET_NULL INVALID_SOCKET

#define net_host_to_net_16(value) (htons (value))
#define net_host_to_net_32(value) (htonl (value))

#define net_net_to_host_16(value) (ntohs (value))
#define net_net_to_host_32(value) (ntohl (value))

typedef struct {
  size_t dataLength;
  void *data;
} net_buffer;

#define NET_CALLBACK __cdecl

typedef fd_set ENetSocketSet;

#define net_socketset_empty(sockset) FD_ZERO (& (sockset))
#define net_socketset_add(sockset, socket) FD_SET (socket, & (sockset))
#define net_socketset_remove(sockset, socket) FD_CLR (socket, & (sockset))
#define net_socketset_check(sockset, socket) FD_ISSET (socket, & (sockset))

#endif
