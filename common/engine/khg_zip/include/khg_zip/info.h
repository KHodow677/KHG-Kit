#pragma once

#include <stdlib.h>

#if defined(_WIN32) || defined(_WIN64)
#include <direct.h>
#include <io.h>
#define zip_has_device(P) ((((P)[0] >= 'A' && (P)[0] <= 'Z') || ((P)[0] >= 'a' && (P)[0] <= 'z')) && (P)[1] == ':')
#define zip_filesystem_prefix_len(P) (zip_has_device(P) ? 2 : 0)
#else
#include <sys/stat.h>
#define zip_has_device(P) 0
#define zip_filesystem_prefix_len(P) 0
#endif

#ifndef zip_is_slash
#define zip_is_slash(C) ((C) == '/' || (C) == '\\')
#endif

#define zip_cleanup(ptr) {\
  if (ptr) {\
    free((void *)ptr);\
    ptr = NULL;\
  }\
}

#define UNIX_IFDIR 0040000
#define UNIX_IFREG 0100000
#define UNIX_IFSOCK 0140000
#define UNIX_IFLNK 0120000
#define UNIX_IFBLK 0060000
#define UNIX_IFCHR 0020000
#define UNIX_IFIFO 0010000

#define ZIP_ENOINIT -1
#define ZIP_EINVENTNAME -2
#define ZIP_ENOENT -3
#define ZIP_EINVMODE -4
#define ZIP_EINVLVL -5
#define ZIP_ENOSUP64 -6
#define ZIP_EMEMSET -7
#define ZIP_EWRTENT -8
#define ZIP_ETDEFLINIT -9
#define ZIP_EINVIDX -10
#define ZIP_ENOHDR -11
#define ZIP_ETDEFLBUF -12
#define ZIP_ECRTHDR -13
#define ZIP_EWRTHDR -14
#define ZIP_EWRTDIR -15
#define ZIP_EOPNFILE -16
#define ZIP_EINVENTTYPE -17
#define ZIP_EMEMNOALLOC -18
#define ZIP_ENOFILE -19
#define ZIP_ENOPERM -20
#define ZIP_EOOMEM -21
#define ZIP_EINVZIPNAME -22
#define ZIP_EMKDIR -23
#define ZIP_ESYMLINK -24
#define ZIP_ECLSZIP -25
#define ZIP_ECAPSIZE -26
#define ZIP_EFSEEK -27
#define ZIP_EFREAD -28
#define ZIP_EFWRITE -29
#define ZIP_ERINIT -30
#define ZIP_EWINIT -31
#define ZIP_EWRINIT -32
#define ZIP_EINVAL -33
#define ZIP_ENORITER -34

const char *const ZIP_ERROR_LIST[35] = {
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

