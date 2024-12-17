#include "networking/processing.h"
#include "khg_tcp/error.h"
#include "khg_tcp/tcp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char BUFFER[TCP_STREAM_BUFFER_SIZE] = "";

void process_error(tcp_error e, void *user_data) {
  tcp_channel **channel = (tcp_channel **) user_data;
  perror(tcp_error_to_string(e));
  tcp_close_channel(*channel);
  tcp_term();
  exit(-1);
}

bool is_number(const char *str) { 
  char *endptr; 
  errno = 0;
  long val = strtol(str, &endptr, 10); 
  if (errno == ERANGE || (endptr == str)) { 
    return false;
  } 
  return true;
} 

bool print_buffer(const char *buffer, int length, void *user_data) {
  (void) user_data;
  memset(BUFFER, 0, sizeof(BUFFER));
  strncpy(BUFFER, buffer, length);
  printf("%s\n", BUFFER);
  return strlen(buffer) == (size_t) length;
}

bool ignore_buffer(const char *buffer, int length, void *user_data) {
  (void) user_data;
  return strlen(buffer) == (size_t) length;
}

bool message_buffer(const char *buffer, int length, void *user_data) {
  (void) user_data;
  memset(BUFFER, 0, sizeof(BUFFER));
  strncpy(BUFFER, buffer, length);
  char *body = strstr(BUFFER, "\r\n\r\n");
  if (body) {
    body += 4;
    char *newline = strchr(body, '\n');
    if (newline) {
      *newline = '\0';
    }
    if (!is_number(body)) {
      printf("%s\n", body);
    }
  } 
  else {
    printf("No valid response body found.\n");
  }
  return strlen(buffer) == (size_t) length;
}

