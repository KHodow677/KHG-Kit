#include "networking/client.h"
#include "khg_tcp/error.h"
#include "khg_tcp/tcp.h"
#include "khg_utl/config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

static const char *NETWORK_DELIMITER = "::";
static const char *NETWORK_RESULT_COMMAND = "RESLT";

char SERVER_IP_ADDRESS[256];
char SERVER_PROTOCOL[8];

static void process_error(tcp_error e, void *user_data) {
  tcp_channel **channel = (tcp_channel **) user_data;
  perror(tcp_error_to_string(e));
  tcp_close_channel(*channel);
  tcp_term();
  exit(-1);
}

void hoster_run() {
  utl_config_file *config = utl_config_create("res/assets/data/net/info.ini");
  strcpy(SERVER_IP_ADDRESS, (char *)utl_config_get_value(config, "net_info", "connect_ip"));
  strcpy(SERVER_PROTOCOL, (char *)utl_config_get_value(config, "net_info", "connect_protocol"));
  utl_config_deallocate(config);
  printf("IP: %s\nPORT: %s\n", SERVER_IP_ADDRESS, SERVER_PROTOCOL);
  tcp_init();
  tcp_channel *err_channel = NULL;
  tcp_set_error_callback(process_error, &err_channel);
  tcp_channel *channel = tcp_connect(SERVER_IP_ADDRESS, SERVER_PROTOCOL);
  tcp_client_send(channel, "FETCH", "AA");
  tcp_close_channel(channel);
  tcp_close_channel(err_channel);
  tcp_term();
}

void tcp_client_send(tcp_channel *channel, const char *command, const char *parameter) {
  tcp_init();
  const char *data = "KHGSVR_V1::%s::%s";
  const char *request = "POST /send HTTP/1.1\r\nHost: %s\r\nContent-Type: application/json\r\nContent-Length: %zu\r\n\r\n%s";
  char formatted_data[256];
  snprintf(formatted_data, sizeof(formatted_data), data, command, parameter);
  char formatted_request[1024];
  snprintf(formatted_request, sizeof(formatted_request), request, SERVER_IP_ADDRESS, strlen(formatted_data), formatted_data);
  tcp_send(channel, formatted_request, strlen(formatted_request), 500);
  char formatted_response[1024];
  tcp_receive(channel, formatted_response, 1024, 500);
  char *packet_info = strstr(formatted_response, NETWORK_DELIMITER) + 2 * strlen(NETWORK_DELIMITER) + strlen(NETWORK_RESULT_COMMAND);
  if (packet_info) {
    printf("%s\n", packet_info);
  }
}

