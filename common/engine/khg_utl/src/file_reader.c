#include "khg_utl/file_reader.h"
#include "khg_utl/error_func.h"
#include "khg_utl/string.h"
#include <stdlib.h>
#include <string.h>

utl_file_reader *utl_file_reader_open(const char *filename, const utl_read_mode mode) {
  if (!filename) {
    utl_error_func("Filename is null", utl_user_defined_data);
    exit(-1);
  }
  utl_file_reader *reader = (utl_file_reader *) malloc(sizeof(utl_file_reader));
  if (!reader) {
    utl_error_func("Cannot allocate memory for file reader", utl_user_defined_data);
    exit(-1);
  }
  const char *modeStr = NULL;
  switch (mode) {
    case UTL_READ_TEXT:
      modeStr = "r";
      break;
    case UTL_READ_BINARY:
      modeStr = "rb";
      break;
    case UTL_READ_UNICODE:
      modeStr = "r";
      break;
    case UTL_READ_BUFFERED:
      modeStr = "r";
      break;
    case UTL_READ_UNBUFFERED:
      modeStr = "r";
      break;
    default:
      utl_error_func("Not a valid mode for reading, initializing default mode 'r'", utl_user_defined_data);
      modeStr = "r";
      break;
  }
  reader->file_reader = fopen(filename, modeStr);
  if (reader->file_reader == NULL) {
    utl_error_func("Cannot open file", utl_user_defined_data);
    free(reader);
    exit(-1);
  }
  reader->mode = mode;
  reader->is_open = true;
  reader->file_path = utl_string_strdup(filename);
  return reader;
}

bool utl_file_reader_close(utl_file_reader *reader) {
  if (!reader) {
    utl_error_func("Object is null", utl_user_defined_data);
    return false;
  }
  if (reader->file_reader && fclose(reader->file_reader)) {
    utl_error_func("Failed to close file", utl_user_defined_data);
    return false;
  }
  if (reader->file_path) {
    free(reader->file_path);
    reader->file_path = NULL;
  }
  reader->is_open = false;
  free(reader);
  return true;
}

size_t utl_file_reader_get_position(utl_file_reader *reader) {
  if (reader->file_reader == NULL) {
    utl_error_func("Object is null or invalid", utl_user_defined_data);
    return (size_t)-1;
  }
  long cursor_position = ftell(reader->file_reader);
  if (cursor_position == -1L) {
    utl_error_func("Could not determine file position", utl_user_defined_data);
    return (size_t)-1;
  }
  return (size_t)cursor_position;
}

bool utl_file_reader_is_open(utl_file_reader *reader) {
  if (!reader) {
    utl_error_func("Pointer is NULL", utl_user_defined_data);
    return false;
  }
  if (reader->file_reader == NULL) {
    utl_error_func("Object is NULL and the file is not open", utl_user_defined_data);
    return false;
  }
  return reader->is_open;
}

const char *utl_file_reader_get_file_name(utl_file_reader *reader) {
  if (!reader || reader->file_reader == NULL) {
    utl_error_func("Object is null or invalid", utl_user_defined_data);
    return NULL;
  }
  if (!reader->file_path) {
    utl_error_func("File path is null", utl_user_defined_data);
    return NULL;
  }
  return (const char *)reader->file_path;
}

bool utl_file_reader_seek(utl_file_reader *reader, long offset, const utl_cursor_position cursor_pos) {
  if (!reader || reader->file_reader == NULL) {
    utl_error_func("Object is null or invalid", utl_user_defined_data);
    return false;
  }
  int pos;
  switch (cursor_pos) {
    case UTL_POS_BEGIN:
      pos = SEEK_SET;
      break;
    case UTL_POS_END:
      pos = SEEK_END;
      break;
    case UTL_POS_CURRENT:
      pos = SEEK_CUR;
      break;
    default:
      pos = SEEK_SET;
      break;
  }
  if (fseek(reader->file_reader, offset, pos) != 0) {
    utl_error_func("Fseek failed", utl_user_defined_data);
    return false;
  }
  return true;
}

bool utl_file_reader_eof(utl_file_reader *reader) {
  if (!reader || reader->file_reader == NULL) {
    utl_error_func("Object is NULL or invalid", utl_user_defined_data);
    return false;
  }
  bool eof_reached = feof(reader->file_reader) != 0;
  return eof_reached;
}

size_t utl_file_reader_get_size(utl_file_reader *reader) {
  if (!reader || reader->file_reader == NULL) {
    utl_error_func("FileReader object is not valid or NULL", utl_user_defined_data);
    return 0;
  }
  long current_position = utl_file_reader_get_position(reader);
  if (fseek(reader->file_reader, 0, SEEK_END) != 0) {
    utl_error_func("Fseek failed to seek to end of file", utl_user_defined_data);
    return 0;
  }
  size_t size = utl_file_reader_get_position(reader);
  if (fseek(reader->file_reader, current_position, SEEK_SET) != 0) {
    utl_error_func("Fseek failed to return to original position", utl_user_defined_data);
  }
  return size;
}

size_t utl_file_reader_read(void *buffer, size_t size, size_t count, utl_file_reader *reader) {
  if (!reader || !reader->file_reader || !buffer) {
    utl_error_func("Invalid argument", utl_user_defined_data);
    return 0;
  }
  if (reader->mode == UTL_READ_BINARY || reader->mode == UTL_READ_UNBUFFERED || reader->mode == UTL_READ_BUFFERED) {
    size_t elements_read = fread(buffer, size, count, reader->file_reader);
    return elements_read;
  }
  if (reader->mode == UTL_READ_TEXT || reader->mode == UTL_READ_UNICODE) {
      char *rawBuffer = (char *)malloc(sizeof(char) * (count + 1));
      if (!rawBuffer) {
        utl_error_func("Memory allocation failed", utl_user_defined_data);
        return 0;
      }
      size_t actualRead = fread(rawBuffer, sizeof(char), count, reader->file_reader);
      rawBuffer[actualRead] = '\0';
      memcpy(buffer, rawBuffer, actualRead);
      free(rawBuffer);  
      return actualRead;
  }
  utl_error_func("Unsupported read mode", utl_user_defined_data);
  return 0;
}

bool utl_file_reader_read_line(char *buffer, size_t size, utl_file_reader *reader) {
  if (!reader || !reader->file_reader || !buffer) {
    utl_error_func("Invalid argument", utl_user_defined_data);
    return false;
  }
  if (reader->mode == UTL_READ_UNICODE) {
    char wBuffer[1024];
    if (fgets(wBuffer, 1024, reader->file_reader) == NULL) {
      if (!feof(reader->file_reader)) {
        utl_error_func("Failed to read line in UTF-16 mode", utl_user_defined_data);
      }
      return false;
    }
    char *utf8Buffer = wBuffer;
    strncpy(buffer, utf8Buffer, size - 1);
    buffer[size - 1] = '\0';
  } 
  else {
    if (fgets(buffer, size, reader->file_reader) == NULL) {
      if (!feof(reader->file_reader)) {
        utl_error_func("Failed to read line in non-UTF-16 mode", utl_user_defined_data);
      }
      return false;
    }
    buffer[strcspn(buffer, "\r\n")] = '\0';
  }
  return true;
}

size_t utl_file_reader_read_fmt(utl_file_reader *reader, const char *format, ...) {
  if (!reader || !reader->file_reader || !format) {
    utl_error_func("Invalid argument", utl_user_defined_data);
    return 0;
  }
  char wBuffer[1024]; 
  if (fgets(wBuffer, sizeof(wBuffer) / sizeof(char), reader->file_reader) == NULL) {
    utl_error_func("Failed to read formatted data", utl_user_defined_data);
    return 0; 
  }
  char *utf8Buffer = wBuffer;
  va_list args;
  va_start(args, format);
  size_t read = vsscanf(utf8Buffer, format, args);
  va_end(args);
  return read; 
}

bool utl_file_reader_copy(utl_file_reader* src_reader, utl_file_writer* dest_writer) {
  if (!src_reader || !src_reader->file_reader || !dest_writer || !dest_writer->file_writer) {
    utl_error_func("Invalid argument", utl_user_defined_data);
    return false;
  }
  char wBuffer[1024];
  size_t bytesRead, bytesToWrite;
  while ((bytesRead = fread(wBuffer, sizeof(char), 1024, src_reader->file_reader)) > 0) {
    char *utf8Buffer = NULL;
    size_t utf8BufferSize = 0;
    utf8Buffer = wBuffer;
    utf8BufferSize = utl_string_length_utf8(utf8Buffer);
    bytesToWrite = utf8BufferSize;
    size_t bytesWritten = utl_file_writer_write(utf8Buffer, sizeof(char), bytesToWrite, dest_writer);
    if (bytesWritten < bytesToWrite) {
      utl_error_func("Could not write all data to the destination file", utl_user_defined_data);
      return false;
    }
  }
  if (!feof(src_reader->file_reader)) {
    utl_error_func("Unexpected end of file during copy operation", utl_user_defined_data);
    return false;
  }
  return true;
}

bool utl_file_reader_read_lines(utl_file_reader *reader, char ***buffer, size_t num_lines) {
  if (!reader || !reader->file_reader || !buffer) {
    utl_error_func("Invalid arguments", utl_user_defined_data);
    return false;
  }
  *buffer = malloc(num_lines * sizeof(char *));
  if (!*buffer) {
    utl_error_func("Memory allocation failed for buffer", utl_user_defined_data);
    return false;
  }
  size_t lines_read = 0;
  char line_buffer[1024]; 
  while (lines_read < num_lines && !feof(reader->file_reader)) {
    if (utl_file_reader_read_line(line_buffer, sizeof(line_buffer), reader)) {
      (*buffer)[lines_read] = utl_string_strdup(line_buffer);
      if (!(*buffer)[lines_read]) {
        utl_error_func("Memory allocation failed", utl_user_defined_data);
        for (size_t i = 0; i < lines_read; ++i) {
          free((*buffer)[i]);
        }
        free(*buffer);
        return false;
      }
      lines_read++;
    } 
  }
  return lines_read == num_lines;
}

