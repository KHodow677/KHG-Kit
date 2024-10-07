#pragma once

void sender_run(const char *room_code);
void receiver_run(const char *room_code);

int tcp_client_create_room(const char *room_code);
int tcp_client_join_room(const char *room_code);

int tcp_client_send(const char *message);
int tcp_client_receive();

