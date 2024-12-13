#pragma once

#include "miniz.h"
#include <stdint.h>
#include <stdio.h>

#define ZIP_DEFAULT_COMPRESSION_LEVEL 6

typedef struct zip_entry {
  ssize_t index;
  char *name;
  mz_uint64 uncomp_size;
  mz_uint64 comp_size;
  mz_uint32 uncomp_crc32;
  mz_uint64 dir_offset;
  mz_uint8 header[MZ_ZIP_LOCAL_DIR_HEADER_SIZE];
  mz_uint64 header_offset;
  mz_uint16 method;
  mz_zip_writer_add_state state;
  tdefl_compressor comp;
  mz_uint32 external_attr;
  time_t m_time;
} zip_entry;

typedef struct zip {
  mz_zip_archive archive;
  mz_uint level;
  zip_entry entry;
} zip;

typedef enum zip_modify {
  ZIP_KEEP = 0,
  ZIP_DELETE = 1,
  ZIP_MOVE = 2,
} zip_modify;

typedef struct zip_entry_mark {
  ssize_t file_index;
  zip_modify type;
  mz_uint64 m_local_header_ofs;
  size_t lf_length;
} zip_entry_mark;

const char *zip_str_error(int errnum);

zip *zip_open(const char *zipname, int level, char mode);
zip *zip_open_with_error(const char *zipname, int level, char mode, int *errnum);
void zip_close(zip *zip);

int zip_is_64(zip *zip);
int zip_offset(zip *zip, uint64_t *offset);

int zip_entry_open(zip *zip, const char *entryname);
int zip_entry_open_case_sensitive(zip *zip, const char *entryname);
int zip_entry_open_by_index(zip *zip, size_t index);
int zip_entry_close(zip *zip);
const char *zip_entry_name(zip *zip);
ssize_t zip_entry_index(zip *zip);
int zip_entry_is_dir(zip *zip);
unsigned long long zip_entry_size(zip *zip);
unsigned long long zip_entry_uncomp_size(zip *zip);
unsigned long long zip_entry_comp_size(zip *zip);
unsigned int zip_entry_crc_32(zip *zip);

unsigned long long zip_entry_dir_offset(zip *zip);
unsigned long long zip_entry_header_offset(zip *zip);

int zip_entry_write(zip *zip, const void *buf, size_t bufsize);
int zip_entry_fwrite(zip *zip, const char *filename);

ssize_t zip_entry_read(zip *zip, void **buf, size_t *bufsize);
ssize_t zip_entry_no_alloc_read(zip *zip, void *buf, size_t bufsize);
ssize_t zip_entry_no_alloc_read_with_offset(zip *zip, size_t offset, size_t size, void *buf);
int zip_entry_fread(zip *zip, const char *filename);
int zip_entry_extract(zip *zip, size_t (*on_extract)(void *arg, uint64_t offset, const void *data, size_t size), void *arg);

ssize_t zip_entries_total(zip *zip);
ssize_t zip_entries_delete(zip *zip, char *const entries[], size_t len);
ssize_t zip_entries_delete_by_index(zip *zip, size_t entries[], size_t len);

int zip_stream_extract(const char *stream, size_t size, const char *dir, int (*on_extract)(const char *filename, void *arg), void *arg);
zip *zip_stream_open(const char *stream, size_t size, int level, char mode);
zip *zip_stream_open_with_error(const char *stream, size_t size, int level, char mode, int *errnum);
ssize_t zip_stream_copy(zip *zip, void **buf, size_t *bufsize);
void zip_stream_close(zip *zip);

zip *zip_cstream_open(FILE *stream, int level, char mode);
zip *zip_cstream_open_with_error(FILE *stream, int level, char mode, int *errnum);
void zip_cstream_close(zip *zip);

int zip_create(const char *zipname, const char *filenames[], size_t len);
int zip_extract(const char *zipname, const char *dir, int (*on_extract_entry)(const char *filename, void *arg), void *arg);

