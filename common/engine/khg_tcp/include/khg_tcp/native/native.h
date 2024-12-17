#pragma once

#if defined(_WIN32) || defined(_WIN64)
  #include <winsock2.h>
  #include <Ws2tcpip.h>
#else
  #include <sys/socket.h>
  #include <sys/ioctl.h>
  #include <arpa/inet.h>
  #include <netdb.h>
  #include <unistd.h>
#endif

#if defined(_WIN32) || defined(_WIN64)
  #define TCP_INVALID_SOCKET (~0ULL)
  #define TCP_SET_NON_BLOCKING(s, value) ioctlsocket(s, FIONBIO, value)
  #define TCP_SHUTDOWN_SOCKET(s) shutdown(s, SD_BOTH)
  #define TCP_CLOSE_SOCKET(s) closesocket(s)
#else
  #define TCP_INVALID_SOCKET (-1LL)
  #define TCP_SET_NON_BLOCKING(s, value) ioctl(s, FIONBIO, value)
  #define TCP_SHUTDOWN_SOCKET(s) shutdown(s, SHUT_RDWR)
  #define TCP_CLOSE_SOCKET(s) close(s)
#endif

