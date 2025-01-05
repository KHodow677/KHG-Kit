#pragma once

#include "miniz.h"
#include <stdio.h>

#define ZIP_DEFAULT_COMPRESSION_LEVEL 6

typedef struct zip_entry {
  long long index;
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
  long long file_index;
  zip_modify type;
  mz_uint64 m_local_header_ofs;
  unsigned int lf_length;
} zip_entry_mark;

const char *zip_str_error(int errnum);

zip *zip_open(const char *zipname, int level, char mode);
zip *zip_open_with_error(const char *zipname, int level, char mode, int *errnum);
void zip_close(zip *zip);

int zip_is_64(zip *zip);
int zip_offset(zip *zip, unsigned long long *offset);

int zip_entry_open(zip *zip, const char *entryname);
int zip_entry_open_case_sensitive(zip *zip, const char *entryname);
int zip_entry_open_by_index(zip *zip, unsigned int index);
int zip_entry_close(zip *zip);
const char *zip_entry_name(zip *zip);
long long zip_entry_index(zip *zip);
int zip_entry_is_dir(zip *zip);
unsigned long long zip_entry_size(zip *zip);
unsigned long long zip_entry_uncomp_size(zip *zip);
unsigned long long zip_entry_comp_size(zip *zip);
unsigned int zip_entry_crc_32(zip *zip);

unsigned long long zip_entry_dir_offset(zip *zip);
unsigned long long zip_entry_header_offset(zip *zip);

int zip_entry_write(zip *zip, const void *buf, unsigned int bufsize);
int zip_entry_fwrite(zip *zip, const char *filename);

long long zip_entry_read(zip *zip, void **buf, unsigned int *bufsize);
long long zip_entry_no_alloc_read(zip *zip, void *buf, unsigned int bufsize);
long long zip_entry_no_alloc_read_with_offset(zip *zip, unsigned int offset, unsigned int size, void *buf);
int zip_entry_fread(zip *zip, const char *filename);
int zip_entry_extract(zip *zip, unsigned int (*on_extract)(void *arg, unsigned long long offset, const void *data, unsigned int size), void *arg);

long long zip_entries_total(zip *zip);
long long zip_entries_delete(zip *zip, char *const entries[], unsigned int len);
long long zip_entries_delete_by_index(zip *zip, unsigned int entries[], unsigned int len);

int zip_stream_extract(const char *stream, unsigned int size, const char *dir, int (*on_extract)(const char *filename, void *arg), void *arg);
zip *zip_stream_open(const char *stream, unsigned int size, int level, char mode);
zip *zip_stream_open_with_error(const char *stream, unsigned int size, int level, char mode, int *errnum);
long long zip_stream_copy(zip *zip, void **buf, unsigned int *bufsize);
void zip_stream_close(zip *zip);

zip *zip_cstream_open(FILE *stream, int level, char mode);
zip *zip_cstream_open_with_error(FILE *stream, int level, char mode, int *errnum);
void zip_cstream_close(zip *zip);

int zip_create(const char *zipname, const char *filenames[], unsigned int len);
int zip_extract(const char *zipname, const char *dir, int (*on_extract_entry)(const char *filename, void *arg), void *arg);

