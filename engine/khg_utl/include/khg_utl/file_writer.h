#pragma once

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

typedef enum {
  UTL_POS_BEGIN,
  UTL_POS_END,
  UTL_POS_CURRENT,
} utl_cursor_position;

typedef enum {
  UTL_WRITE_ENCODING_UTF16,
  UTL_WRITE_ENCODING_UTF32,
} utl_write_encoding_type;

typedef enum {
  UTL_WRITE_TEXT,
  UTL_WRITE_BINARY,
  UTL_WRITE_UNICODE,
  UTL_WRITE_BUFFERED,
  UTL_WRITE_UNBUFFERED,
  UTL_WRITE_LINE,
  UTL_WRITE_APPEND,
} utl_write_mode;

typedef struct {
  FILE *file_writer;
  utl_write_mode mode;
  bool is_open;
  utl_write_encoding_type encoding;
  char *file_path;
} utl_file_writer;

utl_file_writer *utl_file_writer_open(const char* filename, const utl_write_mode mode);
utl_file_writer *utl_file_writer_append(const char* filename, const utl_write_mode mode);

size_t utl_file_writer_get_position(utl_file_writer *writer);
size_t utl_file_writer_write(void* buffer, size_t size, size_t count, utl_file_writer *writer);
size_t utl_file_writer_write_fmt(utl_file_writer *writer, const char *format, ...);
size_t utl_file_writer_get_size(utl_file_writer *writer);

bool utl_file_writer_write_line(char *buffer, size_t size, utl_file_writer *writer);
bool utl_file_writer_close(utl_file_writer *writer);
bool utl_file_writer_is_open(utl_file_writer *writer);
bool utl_file_writer_flush(utl_file_writer *writer);
bool utl_file_writer_set_encoding(utl_file_writer *writer, const utl_write_encoding_type encoding);
bool utl_file_writer_copy(utl_file_writer *src_writer, utl_file_writer *dest_writer);
bool utl_file_writer_lock(utl_file_writer *writer);
bool utl_file_writer_unlock(utl_file_writer *writer);
bool utl_file_writer_seek(utl_file_writer *writer, long offset, const utl_cursor_position cursor_pos);
bool utl_file_writer_truncate(utl_file_writer *writer, size_t size);
bool utl_file_writer_write_batch(utl_file_writer *writer, const void **buffers, const size_t *sizes, size_t count);
bool utl_file_writer_append_fmt(utl_file_writer *writer, const char *format, ...);

const char *utl_file_writer_get_file_name(utl_file_writer *writer);
const char *utl_file_writer_get_encoding(utl_file_writer *writer);

