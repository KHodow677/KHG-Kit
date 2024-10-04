#include "net_process.h"
#include "khg_tcp/tcp.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char BUFFER[TCP_STREAM_BUFFER_SIZE] = "";

bool is_number(const char *str) { 
  char *endptr; 
  errno = 0;
  long val = strtol(str, &endptr, 10); 
  if (errno == ERANGE || (endptr == str)) { 
    return false;
  } 
  return true;
} 

bool print_buffer(const char *buffer, const int length, void *user_data) {
  printf("%s\n", buffer);
	return strlen(buffer) == (size_t)length;
}

bool set_buffer(const char *buffer, const int length, void *user_data) {
  memset(BUFFER, 0, sizeof(BUFFER));
  strncpy(BUFFER, buffer, length);
	return strlen(buffer) == (size_t)length;
}

bool ignore_buffer(const char *buffer, int length, void *user_data) {
	return strlen(buffer) == (size_t) length;
}
