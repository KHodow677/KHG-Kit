#include "khg_csv/csv.h"
#include <stdlib.h>
#include <string.h>

#if (!defined(_WIN32)) && (!defined(_WIN64))
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#endif

#define CSV_GET_PAGE_ALIGNED( orig, page ) (((orig) + ((page) - 1)) & ~((page) - 1))

#define CSV_QUOTE_BR(c, n) {\
  if (c##n == quote)\
    handle->quotes++;\
  else if (c##n == '\n' && !(handle->quotes & 1))\
    return p + n;\
}

#if defined(_WIN32) || defined(_WIN64)

static void *csv_map_mem(CsvHandle handle) {
  size_t size = handle->blockSize;
  if (handle->mapSize + size > handle->fileSize) {
    size = 0;
  }
  handle->mem = MapViewOfFileEx(handle->fm, FILE_MAP_COPY, (DWORD)(handle->mapSize >> 32), (DWORD)(handle->mapSize & 0xFFFFFFFF), size, NULL);
  return handle->mem;
}

static void csv_unmap_mem(CsvHandle handle) {
  if (handle->mem) {
    UnmapViewOfFileEx(handle->mem, 0);
  }
}

#else

static void *csv_map_mem(csv_handle handle) {
  handle->mem = mmap(0, handle->blockSize, PROT_READ | PROT_WRITE, MAP_PRIVATE, handle->fh, handle->mapSize);
  return handle->mem;
}

static void csv_unmap_mem(csv_handle handle) {
  if (handle->mem) {
    munmap(handle->mem, handle->blockSize);
  }
}

#endif

static int csv_ensure_mapped(csv_handle handle) {
  file_off_t newSize;
  if (handle->pos < handle->size) {
    return 0;
  }
  csv_unmap_mem(handle);  
  handle->mem = NULL;
  if (handle->mapSize >= handle->fileSize) {
    return -EINVAL;
  }
  newSize = handle->mapSize + handle->blockSize;
  if (csv_map_mem(handle)) {
    handle->pos = 0;
    handle->mapSize = newSize;
    handle->size = handle->blockSize;
    if (handle->mapSize > handle->fileSize) {
      handle->size = (size_t)(handle->fileSize % handle->blockSize);
    }
    return 0;
  }
  return -ENOMEM;
}

static char *csv_chunk_to_aux_buf(csv_handle handle, char *p, size_t size) {
  size_t newSize = handle->auxbufPos + size + 1;
  if (handle->auxbufSize < newSize) {
    void *mem = realloc(handle->auxbuf, newSize);
    if (!mem) {
      return NULL;
    }
    handle->auxbuf = mem;
    handle->auxbufSize = newSize;
  }
  memcpy((char *)handle->auxbuf + handle->auxbufPos, p, size);
  handle->auxbufPos += size;
  *(char *)((char *)handle->auxbuf + handle->auxbufPos) = '\0';
  return handle->auxbuf;
}

static void csv_terminate_line(char *p, size_t size) {
  char *res = p;
  if (size >= 2 && p[-1] == '\r') {
    --res;
  }
  *res = 0;
}

static char *csv_search_lf(char *p, size_t size, csv_handle handle) {
  char *end = p + size;
  char quote = handle->quote;
#ifdef CSV_UNPACK_64_SEARCH
  uint64_t *pd = (uint64_t*)p;
  uint64_t *pde = pd + (size / sizeof(uint64_t));
  for (; pd < pde; pd++) {
    char c0, c1, c2, c3, c4, c5, c6, c7;
    p = (char *)pd;
    c0 = p[0];
    c1 = p[1];
    c2 = p[2];
    c3 = p[3];
    c4 = p[4];
    c5 = p[5];
    c6 = p[6];
    c7 = p[7];
    CSV_QUOTE_BR(c, 0);
    CSV_QUOTE_BR(c, 1);
    CSV_QUOTE_BR(c, 2);
    CSV_QUOTE_BR(c, 3);
    CSV_QUOTE_BR(c, 4);
    CSV_QUOTE_BR(c, 5);
    CSV_QUOTE_BR(c, 6);
    CSV_QUOTE_BR(c, 7);
  }
  p = (char *)pde;
#endif
  for (; p < end; p++) {
    char c0 = *p;
    CSV_QUOTE_BR(c, 0);
  }
  return NULL;
}

csv_handle csv_open(const char *filename) {
  return csv_open_2(filename, ',', '"', '\\');
}

#if defined(_WIN32) || defined(_WIN64)

CsvHandle csv_open_2(const char *filename, char delim, char quote, char escape) {
  LARGE_INTEGER fsize;
  SYSTEM_INFO info;
  size_t pageSize = 0;
  CsvHandle handle = calloc(1, sizeof(struct CsvHandle_));
  if (!handle) {
    return NULL;
  }
  handle->delim = delim;
  handle->quote = quote;
  handle->escape = escape;
  GetSystemInfo(&info);
  handle->blockSize = CSV_GET_PAGE_ALIGNED(BUFFER_WIDTH_APROX, info.dwPageSize);
  handle->fh = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (handle->fh == INVALID_HANDLE_VALUE) {
    goto fail;
  }
  if (GetFileSizeEx(handle->fh, &fsize) == FALSE) {
    goto fail;
  }
  handle->fileSize = fsize.QuadPart;
  if (!handle->fileSize) {
    goto fail;
  }
  handle->fm = CreateFileMapping(handle->fh, NULL, PAGE_WRITECOPY, 0, 0, NULL);
  if (handle->fm == NULL) {
    goto fail;
  }
  return handle;
fail:
  if (handle->fh != INVALID_HANDLE_VALUE) {
    CloseHandle(handle->fh);
  }
  free(handle);
  return NULL;
}

void csv_close(CsvHandle handle) {
  if (!handle) {
    return;
  }
  UnmapMem(handle);
  CloseHandle(handle->fm);
  CloseHandle(handle->fh);
  free(handle->auxbuf);
  free(handle);
}

#else 

csv_handle csv_open_2(const char *filename, char delim, char quote, char escape) {
  long pageSize;
  struct stat fs;
  csv_handle handle = calloc(1, sizeof(struct CsvHandle_));
  if (!handle) {
    goto fail;
  }
  handle->delim = delim;
  handle->quote = quote;
  handle->escape = escape;
  pageSize = sysconf(_SC_PAGESIZE);
  if (pageSize < 0) {
    goto fail;
  }
  handle->blockSize = CSV_GET_PAGE_ALIGNED(BUFFER_WIDTH_APROX, pageSize);
  handle->fh = open(filename, O_RDONLY);
  if (handle->fh < 0) {
    goto fail;
  }
  if (fstat(handle->fh, &fs)) {
    close(handle->fh);
    goto fail;
  }
  handle->fileSize = fs.st_size;
  return handle;
fail:
  free(handle);
  return NULL;
}

void csv_close(csv_handle handle) {
  if (!handle) {
    return;
  }
  csv_unmap_mem(handle);
  close(handle->fh);
  free(handle->auxbuf);
  free(handle);
}

#endif

char *csv_read_next_row(csv_handle handle) {
  size_t size;
  char *p = NULL;
  char *found = NULL;
  do {
    int err = csv_ensure_mapped(handle);
    handle->context = NULL;
    if (err == -EINVAL) {
      if (p == NULL) {
        break;
      }
      return handle->auxbuf;
    }
    else if (err == -ENOMEM) {
      break;
    }
    size = handle->size - handle->pos;
    if (!size) {
      break;
    }
    p = (char*)handle->mem + handle->pos;
    found = csv_search_lf(p, size, handle);
    if (found) {
      size = (size_t)(found - p) + 1;
      handle->pos += size;
      handle->quotes = 0;
      if (handle->auxbufPos) {
        if (!csv_chunk_to_aux_buf(handle, p, size)) {
          break;
        }
        p = handle->auxbuf;
        size = handle->auxbufPos;
      }
      handle->auxbufPos = 0;
      csv_terminate_line(p + size - 1, size);
      return p;
    }
    else {
      handle->pos = handle->size;
    }
    if (!csv_chunk_to_aux_buf(handle, p, size)) {
      break;
    }
  } while (!found);
  return NULL;
}

char *csv_read_next_col(char *row, csv_handle handle) {
  char *p = handle->context ? handle->context : row;
  char *d = p;
  char *b = p;
  int quoted = 0;
  quoted = *p == handle->quote;
  if (quoted) {
    p++;
  }
  for (; *p; p++, d++) {
    int dq = 0;
    if (*p == handle->escape && p[1]) {
      p++;
    }
    if (*p == handle->quote && p[1] == handle->quote) {
      dq = 1;
      p++;
    }
    if (quoted && !dq) {
      if (*p == handle->quote) {
        break;
      }
    }
    else if (*p == handle->delim) {
      break;
    }
    if (d != p) {
      *d = *p;
    }
  }
  if (!*p) {
    if (p == b) {
      return NULL;
    }
    handle->context = p;
  }
  else {
    *d = '\0';
    if (quoted) {
      for (p++; *p; p++) {
        if (*p == handle->delim) {
          break;
        }
      }
      if (*p) {
        p++;
      }
      handle->context = p;
    }
    else {
      handle->context = p + 1;
    }
  }
  return b;
}

