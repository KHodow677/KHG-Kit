#include "khg_tcp/error.h"
#include "khg_tcp/tcp.h"
#include <stdbool.h>

extern char BUFFER[TCP_STREAM_BUFFER_SIZE];

void process_error(tcp_error e, void *user_data);
bool is_number(const char *str);

bool print_buffer(const char *buffer, int length, void *user_data);
bool ignore_buffer(const char *buffer, int length, void *user_data);
bool message_buffer(const char *buffer, int length, void *user_data);

