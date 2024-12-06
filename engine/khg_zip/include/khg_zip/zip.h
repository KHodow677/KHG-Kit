#pragma once

#include "miniz.h"
#include <stdint.h>
#include <stdio.h>

#if defined(_WIN32) || defined(_WIN64)
#include <direct.h>
#include <io.h>
#define HAS_DEVICE(P) ((((P)[0] >= 'A' && (P)[0] <= 'Z') || ((P)[0] >= 'a' && (P)[0] <= 'z')) && (P)[1] == ':')
#define FILESYSTEM_PREFIX_LEN(P) (HAS_DEVICE(P) ? 2 : 0)
#else
#include <sys/stat.h>
#define HAS_DEVICE(P) 0
#define FILESYSTEM_PREFIX_LEN(P) 0
#endif

#ifndef ISSLASH
#define ISSLASH(C) ((C) == '/' || (C) == '\\')
#endif

#define CLEANUP(ptr) {\
  if (ptr) {\
    free((void *)ptr);\
    ptr = NULL;\
  }\
}

#define UNX_IFDIR 0040000
#define UNX_IFREG 0100000
#define UNX_IFSOCK 0140000
#define UNX_IFLNK 0120000
#define UNX_IFBLK 0060000
#define UNX_IFCHR 0020000
#define UNX_IFIFO 0010000

#define ZIP_DEFAULT_COMPRESSION_LEVEL 6

#define ZIP_ENOINIT -1      // not initialized
#define ZIP_EINVENTNAME -2  // invalid entry name
#define ZIP_ENOENT -3       // entry not found
#define ZIP_EINVMODE -4     // invalid zip mode
#define ZIP_EINVLVL -5      // invalid compression level
#define ZIP_ENOSUP64 -6     // no zip 64 support
#define ZIP_EMEMSET -7      // memset error
#define ZIP_EWRTENT -8      // cannot write data to entry
#define ZIP_ETDEFLINIT -9   // cannot initialize tdefl compressor
#define ZIP_EINVIDX -10     // invalid index
#define ZIP_ENOHDR -11      // header not found
#define ZIP_ETDEFLBUF -12   // cannot flush tdefl buffer
#define ZIP_ECRTHDR -13     // cannot create entry header
#define ZIP_EWRTHDR -14     // cannot write entry header
#define ZIP_EWRTDIR -15     // cannot write to central dir
#define ZIP_EOPNFILE -16    // cannot open file
#define ZIP_EINVENTTYPE -17 // invalid entry type
#define ZIP_EMEMNOALLOC -18 // extracting data using no memory allocation
#define ZIP_ENOFILE -19     // file not found
#define ZIP_ENOPERM -20     // no permission
#define ZIP_EOOMEM -21      // out of memory
#define ZIP_EINVZIPNAME -22 // invalid zip archive name
#define ZIP_EMKDIR -23      // make dir error
#define ZIP_ESYMLINK -24    // symlink error
#define ZIP_ECLSZIP -25     // close archive error
#define ZIP_ECAPSIZE -26    // capacity size too small
#define ZIP_EFSEEK -27      // fseek error
#define ZIP_EFREAD -28      // fread error
#define ZIP_EFWRITE -29     // fwrite error
#define ZIP_ERINIT -30      // cannot initialize reader
#define ZIP_EWINIT -31      // cannot initialize writer
#define ZIP_EWRINIT -32     // cannot initialize writer from reader
#define ZIP_EINVAL -33      // invalid argument
#define ZIP_ENORITER -34    // cannot initialize reader iterator

struct zip_entry_t {
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
};

struct zip_t {
  mz_zip_archive archive;
  mz_uint level;
  struct zip_entry_t entry;
};

enum zip_modify_t {
  MZ_KEEP = 0,
  MZ_DELETE = 1,
  MZ_MOVE = 2,
};

struct zip_entry_mark_t {
  ssize_t file_index;
  enum zip_modify_t type;
  mz_uint64 m_local_header_ofs;
  size_t lf_length;
};

const char *const zip_errlist[35] = {
  NULL,
  "not initialized\0",
  "invalid entry name\0",
  "entry not found\0",
  "invalid zip mode\0",
  "invalid compression level\0",
  "no zip 64 support\0",
  "memset error\0",
  "cannot write data to entry\0",
  "cannot initialize tdefl compressor\0",
  "invalid index\0",
  "header not found\0",
  "cannot flush tdefl buffer\0",
  "cannot write entry header\0",
  "cannot create entry header\0",
  "cannot write to central dir\0",
  "cannot open file\0",
  "invalid entry type\0",
  "extracting data using no memory allocation\0",
  "file not found\0",
  "no permission\0",
  "out of memory\0",
  "invalid zip archive name\0",
  "make dir error\0",
  "symlink error\0",
  "close archive error\0",
  "capacity size too small\0",
  "fseek error\0",
  "fread error\0",
  "fwrite error\0",
  "cannot initialize reader\0",
  "cannot initialize writer\0",
  "cannot initialize writer from reader\0",
  "invalid argument\0",
  "cannot initialize reader iterator\0",
};

const char *zip_strerror(int errnum);

struct zip_t *zip_open(const char *zipname, int level, char mode);
struct zip_t *zip_openwitherror(const char *zipname, int level, char mode, int *errnum);
void zip_close(struct zip_t *zip);

int zip_is64(struct zip_t *zip);
int zip_offset(struct zip_t *zip, uint64_t *offset);

int zip_entry_open(struct zip_t *zip, const char *entryname);
int zip_entry_opencasesensitive(struct zip_t *zip, const char *entryname);
int zip_entry_openbyindex(struct zip_t *zip, size_t index);
int zip_entry_close(struct zip_t *zip);
const char *zip_entry_name(struct zip_t *zip);
ssize_t zip_entry_index(struct zip_t *zip);
int zip_entry_isdir(struct zip_t *zip);
unsigned long long zip_entry_size(struct zip_t *zip);
unsigned long long zip_entry_uncomp_size(struct zip_t *zip);
unsigned long long zip_entry_comp_size(struct zip_t *zip);
unsigned int zip_entry_crc32(struct zip_t *zip);

unsigned long long zip_entry_dir_offset(struct zip_t *zip);
unsigned long long zip_entry_header_offset(struct zip_t *zip);

int zip_entry_write(struct zip_t *zip, const void *buf, size_t bufsize);
int zip_entry_fwrite(struct zip_t *zip, const char *filename);

ssize_t zip_entry_read(struct zip_t *zip, void **buf, size_t *bufsize);
ssize_t zip_entry_noallocread(struct zip_t *zip, void *buf, size_t bufsize);
ssize_t zip_entry_noallocreadwithoffset(struct zip_t *zip, size_t offset, size_t size, void *buf);
int zip_entry_fread(struct zip_t *zip, const char *filename);

int zip_entry_extract(struct zip_t *zip, size_t (*on_extract)(void *arg, uint64_t offset, const void *data, size_t size), void *arg);
ssize_t zip_entries_total(struct zip_t *zip);

ssize_t zip_entries_delete(struct zip_t *zip, char *const entries[], size_t len);
ssize_t zip_entries_deletebyindex(struct zip_t *zip, size_t entries[], size_t len);

int zip_stream_extract(const char *stream, size_t size, const char *dir, int (*on_extract)(const char *filename, void *arg), void *arg);
struct zip_t *zip_stream_open(const char *stream, size_t size, int level, char mode);
struct zip_t *zip_stream_openwitherror(const char *stream, size_t size, int level, char mode, int *errnum);
ssize_t zip_stream_copy(struct zip_t *zip, void **buf, size_t *bufsize);
void zip_stream_close(struct zip_t *zip);

struct zip_t *zip_cstream_open(FILE *stream, int level, char mode);
struct zip_t *zip_cstream_openwitherror(FILE *stream, int level, char mode, int *errnum);
void zip_cstream_close(struct zip_t *zip);

int zip_create(const char *zipname, const char *filenames[], size_t len);
int zip_extract(const char *zipname, const char *dir, int (*on_extract_entry)(const char *filename, void *arg), void *arg);

