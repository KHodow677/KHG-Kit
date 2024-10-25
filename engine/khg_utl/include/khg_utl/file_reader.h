#pragma once

#include "khg_utl/file_writer.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

typedef enum {
  UTL_READ_TEXT,
  UTL_READ_BINARY,
  UTL_READ_UNICODE,
  UTL_READ_BUFFERED,
  UTL_READ_UNBUFFERED,
  UTL_READ_LINE,
} utl_read_mode;

typedef enum {
  UTL_READ_ENCODING_UTF16,
  UTL_READ_ENCODING_UTF32,
} utl_read_encoding_type;

typedef struct {
  FILE *file_reader;
  utl_read_mode mode;
  bool is_open;
  utl_read_encoding_type encoding;
  char *file_path;
} utl_file_reader;

utl_file_reader *utl_file_reader_open(const char *filename, const utl_read_mode mode);

bool utl_file_reader_close(utl_file_reader *reader);
bool utl_file_reader_set_encoding(utl_file_reader *reader, const utl_read_encoding_type encoding);
bool utl_file_reader_seek(utl_file_reader *reader, long offset, const utl_cursor_position cursor_pos);
bool utl_file_reader_is_open(utl_file_reader *reader);
bool utl_file_reader_eof(utl_file_reader *reader);
bool utl_file_reader_copy(utl_file_reader *src_reader, utl_file_writer *dest_writer);
bool utl_file_reader_read_line(char *buffer, size_t size, utl_file_reader *reader);
bool utl_file_reader_read_lines(utl_file_reader *reader, char ***buffer, size_t num_lines);

size_t utl_file_reader_get_position(utl_file_reader *reader);
size_t utl_file_reader_read(void* buffer, size_t size, size_t count, utl_file_reader *reader);
size_t utl_file_reader_get_size(utl_file_reader *reader);
size_t utl_file_reader_read_fmt(utl_file_reader *reader, const char* format, ...);

const char *utl_file_reader_get_file_name(utl_file_reader *reader);

