#pragma once

#include "khg_tcp/tcp.h"

extern char SERVER_IP_ADDRESS[];
extern char SERVER_PROTOCOL[];

void hoster_run();
void joiner_run();

void tcp_client_send(tcp_channel *channel);
void tcp_client_receive(tcp_channel *channel);

/*int test_tcp_client_send(void);*/
/*int test_tcp_client_receive(void);*/

