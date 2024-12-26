#include "khg_utl/config.h"
#include "networking/info.h"
#include <string.h>

char SERVER_IP_ADDRESS[256];
char SERVER_PROTOCOL[8];
char SERVER_TAG[16];

void net_info_setup(const char *filepath) {
  utl_config_file *config = utl_config_create(filepath);
  strcpy(SERVER_IP_ADDRESS, (char *)utl_config_get_value(config, "net_info", "connect_ip"));
  strcpy(SERVER_PROTOCOL, (char *)utl_config_get_value(config, "net_info", "connect_protocol"));
  strcpy(SERVER_TAG, (char *)utl_config_get_value(config, "net_info", "connect_tag"));
  utl_config_deallocate(config);
}
