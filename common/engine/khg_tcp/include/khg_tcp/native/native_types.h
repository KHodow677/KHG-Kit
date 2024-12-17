#pragma once

#if defined(_WIN32) || defined(_WIN64)
  typedef unsigned long long int socket_t;
#else
  typedef long long int socket_t;
#endif

