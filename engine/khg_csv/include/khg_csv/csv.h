#pragma once
#include <stddef.h>
#include <stdint.h>

#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
typedef unsigned long long file_off_t;
#else
#include <sys/types.h>
typedef off_t file_off_t;
#endif

#define BUFFER_WIDTH_APROX (40 * 1024 * 1024)
#if defined (__aarch64__) || defined (__amd64__) || defined (_M_AMD64)
#define CSV_UNPACK_64_SEARCH
#endif

typedef struct CsvHandle_ {
#if defined(_WIN32) || defined(_WIN64)
  HANDLE fh;
  HANDLE fm;
#else
  int fh;
#endif
  void *mem;
  size_t pos;
  size_t size;
  char *context;
  size_t blockSize;
  file_off_t fileSize;
  file_off_t mapSize;
  size_t auxbufSize;
  size_t auxbufPos;
  size_t quotes;
  void *auxbuf;
  char delim;
  char quote;
  char escape;
} *csv_handle;

csv_handle csv_open(const char *filename);
csv_handle csv_open_2(const char *filename, char delim, char quote, char escape);

void csv_close(csv_handle handle);

char *csv_read_next_row(csv_handle handle);
char *csv_read_next_col(char *row, csv_handle handle);

