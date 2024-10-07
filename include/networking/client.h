#pragma once

void hoster_run(const char *room_code);
void joiner_run(const char *room_code);

void tcp_client_create_room(const char *room_code);
void tcp_client_join_room(const char *room_code);

void tcp_client_send(const char *command, const char *message);
void tcp_client_receive();

