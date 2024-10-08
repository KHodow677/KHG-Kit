#pragma once

#include "khg_tcp/tcp.h"

void hoster_run(const char *room_code);
void joiner_run(const char *room_code);

void tcp_client_send(tcp_channel *channel);
void tcp_client_receive(tcp_channel *channel);

