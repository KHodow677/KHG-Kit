#pragma once

#include "khg_tcp/tcp.h"

#define SERVER_ADDRESS "165.22.176.143"
#define SERVER_PORT "80"
#define TIMEOUT_MS 5000
#define BUFFER_SIZE 2048

bool send_post_request(tcp_channel *channel, const char *receiver_id, const char *message);
bool send_get_request(tcp_channel *channel);
void receive_response(tcp_channel *channel);
int client_start();

