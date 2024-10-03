#include "khg_tcp/tcp.h"
#include "khg_utl/vector.h"

typedef struct game_server {
  tcp_server *server;
  utl_vector *client_list;
  const char *ip;
  const char *port;
} game_server;

int server_run(void);

void server_open(game_server *server, const char *ip, const char *port);
void server_close(const game_server *server);
void server_accept_client(game_server *server);
void server_send_message(const game_server *server, const tcp_channel *client, const char *message, const int length);
void server_receive_message(const game_server *server, tcp_channel *client);

