#pragma once

#if defined(_WIN32)

#ifdef _MSC_VER
#ifdef ENET_BUILDING_LIB
#pragma warning (disable: 4267) // size_t to int conversion
#pragma warning (disable: 4244) // 64bit to 32bit int
#pragma warning (disable: 4018) // signed/unsigned mismatch
#pragma warning (disable: 4146) // unary minus operator applied to unsigned type
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

#define ENET_CALLBACK __cdecl

typedef fd_set ENetSocketSet;

#define net_socketset_empty(sockset) FD_ZERO (& (sockset))
#define net_socketset_add(sockset, socket) FD_SET (socket, & (sockset))
#define net_socketset_remove(sockset, socket) FD_CLR (socket, & (sockset))
#define net_socketset_check(sockset, socket) FD_ISSET (socket, & (sockset))

#endif
