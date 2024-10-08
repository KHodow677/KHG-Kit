#include "networking/server.h"
#include "khg_tcp/tcp.h"
#include "khg_utl/map.h"

static utl_map *CLIENT_MESSAGES = NULL;
static utl_map *CLIENT_MAPPING = NULL;

void server_run() {
  tcp_init();
  tcp_server *server = tcp_open_server("localhost", "3000", 10);
  while(1) {
    tcp_channel *channel = tcp_accept(server, 0);
  }
};

