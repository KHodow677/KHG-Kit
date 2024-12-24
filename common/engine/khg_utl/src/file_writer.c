#include "khg_utl/file_writer.h"
#include "khg_utl/encoding.h"
#include "khg_utl/error_func.h"
#include "khg_utl/string.h"
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32) || defined(_WIN64) 
#include <windows.h>
#include <io.h>
#else
#include <fcntl.h>
#include <unistd.h>
#endif 

static size_t u16_strlen(const uint16_t *str) {
  const uint16_t *s = str;
  while (*s != 0) {
    s++;
  }
  return s - str;
}

static size_t u32_strlen(const uint32_t *str) {
  const uint32_t *s = str;
  while (*s != 0) {
    s++;
  }
  return s - str;
}

utl_file_writer *utl_file_writer_open(const char *filename, const utl_write_mode mode) {
  if (!filename) {
    utl_error_func("Filename is null", utl_user_defined_data);
    exit(-1);
  }
  utl_file_writer *writer = (utl_file_writer *)malloc(sizeof(utl_file_writer));
  if (!writer) {
    utl_error_func("Cannot allocate memory", utl_user_defined_data);
    exit(-1);
  }
  const char *modeStr = NULL;
  switch (mode) {
    case UTL_WRITE_TEXT:
      modeStr = "w";
      break;
    case UTL_WRITE_BINARY:
      modeStr = "wb";
      break;
    case UTL_WRITE_UNICODE:
#if defined(_WIN32) || defined(_WIN64)
      modeStr = "w, ccs=UTF-8";
#else
      modeStr = "w";
#endif 
      break;
    case UTL_WRITE_BUFFERED:
      modeStr = "w";
      break;
    case UTL_WRITE_UNBUFFERED:
      modeStr = "w";
      break;
    case UTL_WRITE_APPEND:
#if defined(_WIN32) || defined(_WIN64)
      modeStr = "a, ccs=UTF-8";
#else 
      modeStr = "a";
#endif 
      break;
    default:
#if defined(_WIN32) || defined(_WIN64)
      modeStr = "w, ccs=UTF-8";
#else
      modeStr = "w";
#endif 
      break;
  }
#if defined(_WIN32) || defined(_WIN64)
  wchar_t *wFileName = utl_encoding_utf8_to_wchar(filename);
  wchar_t *wMode = utl_encoding_utf8_to_wchar(modeStr);
  if (!wMode) {
    utl_error_func("Cannot convert mode to wchar", utl_user_defined_data);
    exit(-1);
  }
  if (!wFileName) {
    utl_error_func("Cannot convert filename to wchar", utl_user_defined_data);
    exit(-1);
  }
  writer->file_writer = _wfopen(wFileName, wMode);
  free(wMode);
  free(wFileName);
#else 
  writer->file_writer = fopen(filename, modeStr);
#endif 
  if (writer->file_writer == NULL) {
    utl_error_func("Cannot open file", utl_user_defined_data);
    free(writer);
    exit(-1);
  }
  writer->mode = mode;
  writer->is_open = true;
  writer->encoding = UTL_WRITE_ENCODING_UTF16;
  writer->file_path = utl_string_strdup(filename);
  return writer;
}

utl_file_writer *utl_file_writer_append(const char *filename, const utl_write_mode mode) {
  if (!filename) {
    utl_error_func("Filename is null", utl_user_defined_data);
    return NULL;
  }
  utl_file_writer *writer = (utl_file_writer *)malloc(sizeof(utl_file_writer));
  if (!writer) {
    utl_error_func("Cannot allocate memory", utl_user_defined_data);
    return NULL;
  }
  const char *modeStr = NULL;
  switch (mode) {
    case UTL_WRITE_TEXT:
      modeStr = "a";
      break;
    case UTL_WRITE_BINARY:
      modeStr = "ab";
      break;
    case UTL_WRITE_UNICODE:
#if defined(_WIN32) || defined(_WIN64)
      modeStr = "a, ccs=UTF-8";
#else
      modeStr = "a";
#endif 
      break;
    case UTL_WRITE_BUFFERED:
      modeStr = "a";
      break;
    case UTL_WRITE_UNBUFFERED:
      modeStr = "a";
      break;
    case UTL_WRITE_APPEND:
#if defined(_WIN32) || defined(_WIN64)
      modeStr = "a, ccs=UTF-8";
#else 
      modeStr = "a";
#endif 
      break;
    default:
      modeStr = "a";
      break;
  }
#if defined(_WIN32) || defined(_WIN64)
  wchar_t *wFileName = utl_encoding_utf8_to_wchar(filename);
  wchar_t *wMode = utl_encoding_utf8_to_wchar(modeStr);
  if (!wMode) {
    utl_error_func("Cannot convert mode to wchar", utl_user_defined_data);
    free(writer);
    return NULL;
  }
  if (!wFileName) {
    utl_error_func("Cannot convert filename to wchar", utl_user_defined_data);
    free(writer);
    free(wMode);
    return NULL;
  }
  writer->file_writer = _wfopen(wFileName, wMode);
  free(wMode);
  free(wFileName);
#else 
  writer->file_writer = fopen(filename, modeStr);
#endif 
  if (writer->file_writer == NULL) {
    utl_error_func("Cannot open file", utl_user_defined_data);
    free(writer);
    return NULL;
  }
  writer->mode = mode;
  writer->is_open = true;
  writer->encoding = UTL_WRITE_ENCODING_UTF16;
  writer->file_path = utl_string_strdup(filename);
  return writer;
}

bool utl_file_writer_close(utl_file_writer *writer) {
  if (writer->file_writer == NULL) {
    return false;
  }
  if (fclose(writer->file_writer)) {
    utl_error_func("Failed to close the file", utl_user_defined_data);
    return false;
  }
  writer->is_open = false;
  free(writer->file_path);
  free(writer);
  return true;
}

size_t utl_file_writer_get_position(utl_file_writer *writer) {
  if (writer->file_writer == NULL) {
    utl_error_func("Object is NULL and invalid", utl_user_defined_data);
    return (size_t)-1;
  }
  long cursor_position = ftell(writer->file_writer);
  if (cursor_position == -1L) {
    utl_error_func("Could not determine file position", utl_user_defined_data);
    return (size_t)-1;
  }
  return (size_t)cursor_position;
}

size_t utl_file_writer_write(void *buffer, size_t size, size_t count, utl_file_writer *writer) {
  if (!writer || !writer->file_writer || !buffer) {
    utl_error_func("Invalid argument", utl_user_defined_data);
    return 0;
  }
  if (writer->mode == UTL_WRITE_BINARY) {
    return fwrite(buffer, size, count, writer->file_writer);
  }
  size_t written = 0;
  switch (writer->encoding) {
    case UTL_WRITE_ENCODING_UTF32: {
      uint32_t *utf32Buffer = utl_encoding_utf8_to_utf32((const uint8_t*)buffer, size * count);
      if (!utf32Buffer) {
        utl_error_func("Conversion to UTF-32 failed", utl_user_defined_data);
        return 0;
      }
      written = fwrite(utf32Buffer, sizeof(uint32_t), u32_strlen((uint32_t *)utf32Buffer), writer->file_writer);
      free(utf32Buffer);
      break;
    }
    case UTL_WRITE_ENCODING_UTF16: {
#if defined(_WIN32) || defined(_WIN64)
      if (writer->mode == UTL_WRITE_UNICODE || writer->mode == UTL_WRITE_APPEND) {
        wchar_t *wBuffer = utl_encoding_utf8_to_wchar((const char*)buffer);
        if (!wBuffer) {
          utl_error_func("Conversion to wchar_t failed", utl_user_defined_data);
          return 0;
        }
        written = fwrite(wBuffer, sizeof(wchar_t), wcslen(wBuffer), writer->file_writer);
        free(wBuffer);
      } 
      else {
        written = fwrite(buffer, size, count, writer->file_writer);
      }
#else
      if (writer->encoding == UTL_WRITE_ENCODING_UTF16) {
        uint16_t *utf16Buffer = utl_encoding_utf8_to_utf16((const uint8_t *)buffer, size * count);
        if (!utf16Buffer) {
          utl_error_func("Conversion to UTF-16 failed", utl_user_defined_data);
          return 0;
        }
        written = fwrite(utf16Buffer, sizeof(uint16_t), u16_strlen((uint16_t *)utf16Buffer), writer->file_writer);
        free(utf16Buffer);
      } 
      else {
        written = fwrite(buffer, size, count, writer->file_writer);
      }
#endif
      break;
    }
    default:
      written = fwrite(buffer, size, count, writer->file_writer);
      break;
  }
  return written;
}

bool utl_file_writer_write_line(char *buffer, size_t size, utl_file_writer *writer) {
  if (!writer || writer->file_writer == NULL) {
    utl_error_func("Object is NULL or invalid", utl_user_defined_data);
    return false;
  }
  if (buffer == NULL) {
    utl_error_func("Buffer is NULL", utl_user_defined_data);
    return false;
  }
  size_t written = 0;
  size_t elementToWriteSize = size;
#if defined(_WIN32) || defined(_WIN64)
  if (writer->mode == UTL_WRITE_UNICODE) {
    wchar_t *wBuffer = utl_encoding_utf8_to_wchar(buffer);
    if (!wBuffer) {
      utl_error_func("Cannot convert buffer to wchar", utl_user_defined_data);
      return false;
    }
    elementToWriteSize = wcslen(wBuffer);
    written = fwrite(wBuffer, sizeof(wchar_t), elementToWriteSize, writer->file_writer);
    free(wBuffer);
  } 
  else {
    written = fwrite(buffer, sizeof(char), elementToWriteSize, writer->file_writer);
  }
#else 
  written = fwrite(buffer, sizeof(char), size, writer->file_writer);
#endif 
  if (written < elementToWriteSize) {
    utl_error_func("Could not write the entire buffer", utl_user_defined_data);
    return false;
  }
#if defined(_WIN32) || defined(_WIN64)
  if (writer->mode == UTL_WRITE_UNICODE) {
    wchar_t newLine[] = L"\n";
    written = fwrite(newLine, sizeof(wchar_t), 1, writer->file_writer);
  } 
  else {
    char newLine[] = "\n";
    written = fwrite(newLine, sizeof(char), 1, writer->file_writer);
  }
#else 
  char newLine[] = "\n";
  written = fwrite(newLine, sizeof(char), 1, writer->file_writer);
#endif 
  return written == 1;
}

bool utl_file_writer_is_open(utl_file_writer *writer) {
  if (!writer) {
    utl_error_func("Pointer is NULL", utl_user_defined_data);
    return false;
  }
  if (writer->file_writer == NULL) {
    utl_error_func("Object is NULL, file is not open", utl_user_defined_data);
    return false;
  }
  return writer->is_open;
}

bool utl_file_writer_flush(utl_file_writer *writer) {
  if (!writer || writer->file_writer == NULL) {
    utl_error_func("Object is NULL or invalid", utl_user_defined_data);
    return false;
  }
  if (fflush(writer->file_writer) == EOF) {
    utl_error_func("Failed to flush the writer", utl_user_defined_data);
    return false;
  }
  return true;
}

bool utl_file_writer_set_encoding(utl_file_writer *writer, const utl_write_encoding_type encoding) {
  if (!writer || writer->file_writer == NULL) {
    utl_error_func("Object is NULL or invalid", utl_user_defined_data);
    return false;
  }
  if (!(encoding >= UTL_WRITE_ENCODING_UTF16 && encoding <= UTL_WRITE_ENCODING_UTF32)) {
    utl_error_func("Invalid encoding type", utl_user_defined_data);
    return false;
  }
  writer->encoding = encoding;
  return true;
}

bool utl_file_writer_copy(utl_file_writer *src_writer, utl_file_writer *dest_writer) {
  if (!src_writer || src_writer->file_writer == NULL || src_writer->file_path == NULL) {
    utl_error_func("Object or file path is NULL or invalid", utl_user_defined_data);
    return false;
  }
  if (!dest_writer || dest_writer->file_writer == NULL || dest_writer->file_path == NULL) {
    utl_error_func("Object or file path is NULL or invalid", utl_user_defined_data);
    return false;
  }
  FILE *src_file = fopen(src_writer->file_path, "rb");
  if (!src_file) {
    utl_error_func("Cannot reopen source file for reading", utl_user_defined_data);
    return false;
  }
  FILE *dest_file = fopen(dest_writer->file_path, "wb");
  if (!dest_file) {
    utl_error_func("Cannot reopen destination file for writing", utl_user_defined_data);
    fclose(src_file);
    return false;
  }
  char buffer[4096];
  size_t bytes_read;
  while ((bytes_read = fread(buffer, sizeof(char), sizeof(buffer), src_file))) {
    if (fwrite(buffer, sizeof(char), bytes_read, dest_file) != bytes_read) {
      utl_error_func("Failed to write data to the destination file", utl_user_defined_data);
      fclose(src_file);
      fclose(dest_file);
      return false;
    }
  }
  fclose(src_file);
  fclose(dest_file);
  return true;
}

const char *utl_file_writer_get_file_name(utl_file_writer *writer) {
  if (!writer || writer->file_writer == NULL) {
    utl_error_func("Object is null or invalid", utl_user_defined_data);
    return NULL;
  }
  if (!writer->file_path) {
    utl_error_func("File path for object is null", utl_user_defined_data);
    return NULL;
  }
  return (const char *)writer->file_path;
}

const char *utl_file_writer_get_encoding(utl_file_writer *writer) {
  if (!writer || writer->file_writer == NULL) {
    utl_error_func("Object is null or invalid", utl_user_defined_data);
    return NULL;
  }
  if (!(writer->encoding >= UTL_WRITE_ENCODING_UTF16 && writer->encoding <= UTL_WRITE_ENCODING_UTF32)) {
    utl_error_func("Invalid encoding type", utl_user_defined_data);
    return NULL;
  }
  char *encoding = NULL;
  switch (writer->encoding) {
    case UTL_WRITE_ENCODING_UTF16:
      encoding = utl_string_strdup("ENCODING_UTF16");
      break;
    case UTL_WRITE_ENCODING_UTF32:
      encoding = utl_string_strdup("ENCODING_UTF32");
      break;
  }
  return encoding;
}

size_t utl_file_writer_write_fmt(utl_file_writer *writer, const char *format, ...) {
  if (!writer || !writer->file_writer || !format) {
    utl_error_func("Invalid argument", utl_user_defined_data);
    return 0;
  }
  va_list args;
  va_start(args, format);
  char buffer[2048];
  vsnprintf(buffer, sizeof(buffer), format, args);
  size_t written = utl_file_writer_write(buffer, strlen(buffer), 1, writer);
  va_end(args);
  return written;
}

size_t utl_file_writer_get_size(utl_file_writer *writer) {
  if (!writer || writer->file_writer == NULL) {
    utl_error_func("Object is NULL or invalid", utl_user_defined_data);
    return 0;
  }
  if (!utl_file_writer_flush(writer)) {
    utl_error_func("Failed to flush data", utl_user_defined_data);
    return 0;
  }
  size_t current_position = utl_file_writer_get_position(writer);
  if (fseek(writer->file_writer, 0, SEEK_END) != 0) {
    utl_error_func("Failed to seek to end of file", utl_user_defined_data);
    return 0;
  }
  size_t size = utl_file_writer_get_position(writer);
  if (fseek(writer->file_writer, current_position, SEEK_SET) != 0) {
    utl_error_func("Failed to return to original position", utl_user_defined_data);
  }
  return size;
}

bool utl_file_writer_lock(utl_file_writer *writer) {
  if (!writer || writer->file_writer == NULL) {
    utl_error_func("Object is NULL or invalid", utl_user_defined_data);
    return false;
  }
#if defined(_WIN32) || defined(_WIN64)
  HANDLE hFile = (HANDLE)_get_osfhandle(_fileno(writer->file_writer));
  OVERLAPPED overlapped = {0};
  if (LockFileEx(hFile, LOCKFILE_EXCLUSIVE_LOCK, 0, MAXDWORD, MAXDWORD, &overlapped) == 0) {
    utl_error_func("Unable to lock file in Windows", utl_user_defined_data);
    return false;
  }
#else
  struct flock fl = {0};
  fl.l_type = F_WRLCK;
  fl.l_whence = SEEK_SET;
  fl.l_start = 0;
  fl.l_len = 0;
  if (fcntl(fileno(writer->file_writer), F_SETLKW, &fl) == -1) {
    utl_error_func("Unable to lock file in Unix", utl_user_defined_data);
    return false;
  }
#endif
  return true;
}

bool utl_file_writer_unlock(utl_file_writer *writer) {
  if (!writer || writer->file_writer == NULL) {
    utl_error_func("Object is NULL or invalid", utl_user_defined_data);
    return false;
  }
#if defined(_WIN32) || defined(_WIN64)
  HANDLE hFile = (HANDLE)_get_osfhandle(_fileno(writer->file_writer));
  OVERLAPPED overlapped = {0};
  if (UnlockFileEx(hFile, 0, MAXDWORD, MAXDWORD, &overlapped) == 0) {
    utl_error_func("Unable to unlock file in Windows", utl_user_defined_data);
    return false;
  }
#else
  struct flock fl = {0};
  fl.l_type = F_UNLCK;
  fl.l_whence = SEEK_SET;
  fl.l_start = 0;
  fl.l_len = 0; // Unlock the whole fileqqq
  if (fcntl(fileno(writer->file_writer), F_SETLK, &fl) == -1) {
    utl_error_func("Unable to unlock file in Unix", utl_user_defined_data);
    return false;
  }
#endif
  utl_error_func("File successfully unlocked", utl_user_defined_data);
  return true;
}

bool utl_file_writer_seek(utl_file_writer *writer, long offset, const utl_cursor_position cursor_pos) {
  if (!writer || writer->file_writer == NULL) {
    utl_error_func("Object is NULL or invalid", utl_user_defined_data);
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
  if (fseek(writer->file_writer, offset, pos) != 0) {
    utl_error_func("Fseek failed", utl_user_defined_data);
    return false;
  }
  return true;
}

bool utl_file_writer_truncate(utl_file_writer *writer, size_t size) {
  if (!writer || writer->file_writer == NULL) {
    utl_error_func("Object is NULL or invalid", utl_user_defined_data);
    return false;
  }
  if (!utl_file_writer_flush(writer)) {
    utl_error_func("Failed to flush the file", utl_user_defined_data);
    return false;
  }
  int fd;
#if defined(_WIN32) || defined(_WIN64)
  fd = _fileno(writer->file_writer);
  if (_chsize_s(fd, size) != 0) {
    utl_error_func("Could not truncate file in Windows", utl_user_defined_data);
    return false;
  }
#else 
  fd = fileno(writer->file_writer);
  if (ftruncate(fd, size) == -1) {
    utl_error_func("Could not truncate file in Unix", utl_user_defined_data);
    return false;
  }
#endif 
  return true;
}

bool utl_file_writer_write_batch(utl_file_writer *writer, const void **buffers, const size_t *sizes, size_t count) {
  if (!writer || !writer->file_writer || !buffers || !sizes) {
    utl_error_func("Invalid arguments", utl_user_defined_data);
    return false;
  }
  size_t all_bytes = 0;
  size_t total_written = 0;
  for (size_t i = 0; i < count; ++i) {
    const void *buffer = buffers[i];
    size_t size = sizes[i];
    all_bytes += size;
    if (!buffer || size == 0) {
      utl_error_func("Invalid buffer or size at index", utl_user_defined_data);
      continue;
    }
    size_t written = 0;
    void *convertedBuffer = NULL; 
    size_t convertedSize = 0;     
    switch (writer->encoding) {
      case UTL_WRITE_ENCODING_UTF32: {
        uint32_t *utf32Buffer = utl_encoding_utf8_to_utf32((const uint8_t *)buffer, size);
        if (!utf32Buffer) {
          utl_error_func("Conversion to UTF-32 failed at index", utl_user_defined_data);
          continue;
        }
        convertedBuffer = utf32Buffer;
        convertedSize = wcslen((wchar_t *)utf32Buffer) * sizeof(uint32_t);
        break;
      }
      default:
      case UTL_WRITE_ENCODING_UTF16: {
#if defined(_WIN32) || defined(_WIN64)
        wchar_t *wBuffer = utl_encoding_utf8_to_wchar((const char *)buffer);
        if (!wBuffer) {
          utl_error_func("Conversion to wchar_t failed at index", utl_user_defined_data);
          continue;
        }
        convertedBuffer = wBuffer;
        convertedSize = wcslen(wBuffer) * sizeof(wchar_t);
#else
        uint16_t *utf16Buffer = utl_encoding_utf8_to_utf16((const uint8_t *)buffer, size);
        if (!utf16Buffer) {
          utl_error_func("Conversion to UTF-16 failed at index", utl_user_defined_data);
          continue;
        }
        convertedBuffer = utf16Buffer;
        convertedSize = wcslen((wchar_t*)utf16Buffer) * sizeof(uint16_t);
#endif
        break;
      }
    }
    written = fwrite(convertedBuffer, 1, convertedSize, writer->file_writer);
    free(convertedBuffer);
    if (written != convertedSize) {
      utl_error_func("Partial or failed write at index", utl_user_defined_data);
      return false;
    }
    total_written += written;
  }
  if (writer->mode == UTL_WRITE_UNICODE) {
    all_bytes *= 2;
  }
  return total_written == all_bytes;
}

bool utl_file_writer_append_fmt(utl_file_writer *writer, const char *format, ...) {
  if (!writer || !writer->file_writer || !format) {
    utl_error_func("Invalid arguments", utl_user_defined_data);
    return false;
  }
  if (writer->mode != UTL_WRITE_APPEND) {
    utl_error_func("Object must be in append mode", utl_user_defined_data);
    return false;
  }
  va_list args;
  va_start(args, format);
  char buffer[2048]; 
  vsnprintf(buffer, sizeof(buffer), format, args);
  size_t written = utl_file_writer_write(buffer, strlen(buffer), 1, writer);
  va_end(args);
  return written > 0;
}

