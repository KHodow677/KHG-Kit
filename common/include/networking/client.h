#pragma once

#include "khg_tcp/tcp.h"

void hoster_run(void);
void tcp_client_send(tcp_channel *channel, const char *tag, const char *command, const char *parameter);

