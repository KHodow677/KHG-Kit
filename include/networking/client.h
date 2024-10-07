#pragma once

#include "khg_tcp/tcp.h"

void hoster_run(const char *room_code);
void joiner_run(const char *room_code);

void tcp_client_create_room(tcp_channel *channel, const char *room_code);
void tcp_client_join_room(tcp_channel *channel, const char *room_code);

void tcp_client_send(tcp_channel *channel, const char *room_code, const char *command, const char *message);
void tcp_client_receive(tcp_channel *channel);

