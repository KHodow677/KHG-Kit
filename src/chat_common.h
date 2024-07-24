#pragma once

#include "khg_net/net.h"

#define LISTEN_PORT 34567

typedef struct {
  char hostname[1024];
	enet_uint16 port;
} ServerInfo;