#include "khg_zip/zip.h"
#include "khg_zip/info.h"
#include <errno.h>
#include <unistd.h>
#include <time.h>

static const char *zip_basename(const char *name) {
  char const *base = name += FILESYSTEM_PREFIX_LEN(name);
  int all_slashes = 1;
  for (const char *p = name; *p; p++) {
    if (ISSLASH(*p))
      base = p + 1;
    else
      all_slashes = 0;
  }
  if (*base == '\0' && ISSLASH(*name) && all_slashes) {
    --base;
  }
  return base;
}

static int zip_mkpath(char *path) {
  char npath[MZ_ZIP_MAX_ARCHIVE_FILENAME_SIZE + 1];
  memset(npath, 0, MZ_ZIP_MAX_ARCHIVE_FILENAME_SIZE + 1);
  int len = 0;
  int has_device = HAS_DEVICE(path);
  if (has_device) {
    npath[0] = path[0];
    npath[1] = path[1];
    len = 2;
  }
  for (char *p = path + len; *p && len < MZ_ZIP_MAX_ARCHIVE_FILENAME_SIZE; p++) {
    if (ISSLASH(*p) && ((!has_device && len > 0) || (has_device && len > 2))) {
#if defined(_WIN32) || defined(_WIN64)
#else
      if ('\\' == *p) {
        *p = '/';
      }
#endif
      if (MZ_MKDIR(npath) == -1) {
        if (errno != EEXIST) {
          return ZIP_EMKDIR;
        }
      }
    }
    npath[len++] = *p;
  }
  return 0;
}

static char *zip_strclone(const char *str, size_t n) {
  char *rpl = (char *)calloc((1 + n), sizeof(char));
  if (!rpl) {
    return NULL;
  }
  char c;
  char *begin = rpl;
  for (size_t i = 0; (i < n) && (c = *str++); ++i) {
    *rpl++ = c;
  }
  return begin;
}

static char *zip_strrpl(const char *str, size_t n, char oldchar, char newchar) {
  char *rpl = (char *)calloc((1 + n), sizeof(char));
  if (!rpl) {
    return NULL;
  }
  char c;
  char *begin = rpl;
  for (size_t i = 0; (i < n) && (c = *str++); ++i) {
    if (c == oldchar) {
      c = newchar;
    }
    *rpl++ = c;
  }
  return begin;
}

static inline int zip_strchr_match(const char *const str, size_t len, char c) {
  for (size_t i = 0; i < len; ++i) {
    if (str[i] != c) {
      return 0;
    }
  }
  return 1;
}

static char *zip_name_normalize(char *name, char *const nname, size_t len) {
  if (name == NULL || nname == NULL || len <= 0) {
    return NULL;
  }
  while (ISSLASH(*name)) {
    name++;
  }
  char c;
  size_t offn = 0, ncpy = 0;
  while ((c = *name++)) {
    if (ISSLASH(c)) {
      if (ncpy > 0 && !zip_strchr_match(&nname[offn], ncpy, '.')) {
        offn += ncpy;
        nname[offn++] = c;
      }
      ncpy = 0;
    } 
    else {
      nname[offn + ncpy] = c;
      if (c) {
        ncpy++;
      }
    }
  }
  if (!zip_strchr_match(&nname[offn], ncpy, '.')) {
    nname[offn + ncpy] = '\0';
  } 
  else {
    nname[offn] = '\0';
  }
  return nname;
}

static int zip_archive_truncate(mz_zip_archive *pzip) {
  mz_zip_internal_state *pState = pzip->m_pState;
  if ((pzip->m_pWrite == mz_zip_heap_write_func) && (pState->m_pMem)) {
    return 0;
  }
  mz_uint64 file_size = pzip->m_archive_size;
  if (pzip->m_zip_mode == MZ_ZIP_MODE_WRITING_HAS_BEEN_FINALIZED) {
    if (pState->m_pFile) {
      int fd = fileno(pState->m_pFile);
      return ftruncate(fd, pState->m_file_archive_start_ofs + file_size);
    }
  }
  return 0;
}

static int zip_archive_extract(mz_zip_archive *zip_archive, const char *dir, int (*on_extract)(const char *filename, void *arg), void *arg) {
  char path[MZ_ZIP_MAX_ARCHIVE_FILENAME_SIZE + 1];
  char symlink_to[MZ_ZIP_MAX_ARCHIVE_FILENAME_SIZE + 1];
  memset(path, 0, sizeof(path));
  memset(symlink_to, 0, sizeof(symlink_to));
  size_t dirlen = 0, filename_size = MZ_ZIP_MAX_ARCHIVE_FILENAME_SIZE;
  mz_uint32 xattr = 0;
  dirlen = strlen(dir);
  if (dirlen + 1 > MZ_ZIP_MAX_ARCHIVE_FILENAME_SIZE) {
    return ZIP_EINVENTNAME;
  }
  mz_zip_archive_file_stat info;
  memset((void *)&info, 0, sizeof(mz_zip_archive_file_stat));
  strncpy(path, dir, MZ_ZIP_MAX_ARCHIVE_FILENAME_SIZE);
  if (!ISSLASH(path[dirlen - 1])) {
#if defined(_WIN32) || defined(_WIN64)
    path[dirlen] = '\\';
#else
    path[dirlen] = '/';
#endif
    ++dirlen;
  }
  if (filename_size > MZ_ZIP_MAX_ARCHIVE_FILENAME_SIZE - dirlen) {
    filename_size = MZ_ZIP_MAX_ARCHIVE_FILENAME_SIZE - dirlen;
  }
  mz_uint n = mz_zip_reader_get_num_files(zip_archive);
  int err = 0;
  for (mz_uint i = 0; i < n; ++i) {
    if (!mz_zip_reader_file_stat(zip_archive, i, &info)) {
      err = ZIP_ENOENT;
      goto out;
    }
    if (!zip_name_normalize(info.m_filename, info.m_filename, strlen(info.m_filename))) {
      err = ZIP_EINVENTNAME;
      goto out;
    }
    strncpy(&path[dirlen], info.m_filename, filename_size);
    err = zip_mkpath(path);
    if (err < 0) {
      goto out;
    }
    if ((((info.m_version_made_by >> 8) == 3) || ((info.m_version_made_by >> 8) == 19)) && info.m_external_attr & (0x20 << 24)) {
#if defined(_WIN32) || defined(_WIN64)
#else
      if (info.m_uncomp_size > MZ_ZIP_MAX_ARCHIVE_FILENAME_SIZE || !mz_zip_reader_extract_to_mem_no_alloc( zip_archive, i, symlink_to, MZ_ZIP_MAX_ARCHIVE_FILENAME_SIZE, 0, NULL, 0)) {
        err = ZIP_EMEMNOALLOC;
        goto out;
      }
      symlink_to[info.m_uncomp_size] = '\0';
      if (symlink(symlink_to, path) != 0) {
        err = ZIP_ESYMLINK;
        goto out;
      }
#endif
    } 
    else {
      if (!mz_zip_reader_is_file_a_directory(zip_archive, i)) {
        if (!mz_zip_reader_extract_to_file(zip_archive, i, path, 0)) {
          err = ZIP_ENOFILE;
          goto out;
        }
      }
      xattr = (info.m_external_attr >> 16) & 0xFFFF;
      if (xattr > 0 && xattr <= MZ_UINT16_MAX) {
        if (CHMOD(path, (mode_t)xattr) < 0) {
          err = ZIP_ENOPERM;
          goto out;
        }
      }
    }
    if (on_extract) {
      if (on_extract(path, arg) < 0) {
        goto out;
      }
    }
  }
out:
  if (!mz_zip_reader_end(zip_archive)) {
    err = ZIP_ECLSZIP;
  }
  return err;
}

static inline void zip_archive_finalize(mz_zip_archive *pzip) {
  mz_zip_writer_finalize_archive(pzip);
  zip_archive_truncate(pzip);
}

static ssize_t zip_entry_mark_entry(zip *zip, zip_entry_mark *entry_mark, const size_t n, char *const entries[], const size_t len) {
  ssize_t err = 0;
  if (!zip || !entry_mark || !entries) {
    return ZIP_ENOINIT;
  }
  mz_zip_archive_file_stat file_stat;
  mz_uint64 d_pos = UINT64_MAX;
  for (size_t i = 0; i < n; ++i) {
    if ((err = zip_entry_open_by_index(zip, i))) {
      return (ssize_t)err;
    }
    mz_bool name_matches = MZ_FALSE;
    for (size_t j = 0; j < len; ++j) {
      if (strcmp(zip->entry.name, entries[j]) == 0) {
        name_matches = MZ_TRUE;
        break;
      }
    }
    if (name_matches) {
      entry_mark[i].type = ZIP_DELETE;
    } 
    else {
      entry_mark[i].type = ZIP_KEEP;
    }
    if (!mz_zip_reader_file_stat(&zip->archive, (mz_uint)i, &file_stat)) {
      return ZIP_ENOENT;
    }
    zip_entry_close(zip);
    entry_mark[i].m_local_header_ofs = file_stat.m_local_header_ofs;
    entry_mark[i].file_index = (ssize_t)-1;
    entry_mark[i].lf_length = 0;
    if ((entry_mark[i].type) == ZIP_DELETE && (d_pos > entry_mark[i].m_local_header_ofs)) {
      d_pos = entry_mark[i].m_local_header_ofs;
    }
  }
  for (size_t i = 0; i < n; ++i) {
    if ((entry_mark[i].m_local_header_ofs > d_pos) &&
        (entry_mark[i].type != ZIP_DELETE)) {
      entry_mark[i].type = ZIP_MOVE;
    }
  }
  return err;
}

static ssize_t zip_entry_markbyindex(zip *zip, zip_entry_mark *entry_mark, const size_t n, size_t entries[], const size_t len) {
  ssize_t err = 0;
  if (!zip || !entry_mark || !entries) {
    return ZIP_ENOINIT;
  }
  mz_zip_archive_file_stat file_stat;
  mz_uint64 d_pos = UINT64_MAX;
  for (size_t i = 0; i < n; ++i) {
    if ((err = zip_entry_open_by_index(zip, i))) {
      return (ssize_t)err;
    }
    mz_bool matches = MZ_FALSE;
    for (size_t j = 0; j < len; ++j) {
      if (i == entries[j]) {
        matches = MZ_TRUE;
        break;
      }
    }
    if (matches) {
      entry_mark[i].type = ZIP_DELETE;
    } 
    else {
      entry_mark[i].type = ZIP_KEEP;
    }
    if (!mz_zip_reader_file_stat(&zip->archive, (mz_uint)i, &file_stat)) {
      return ZIP_ENOENT;
    }
    zip_entry_close(zip);
    entry_mark[i].m_local_header_ofs = file_stat.m_local_header_ofs;
    entry_mark[i].file_index = (ssize_t)-1;
    entry_mark[i].lf_length = 0;
    if ((entry_mark[i].type) == ZIP_DELETE && (d_pos > entry_mark[i].m_local_header_ofs)) {
      d_pos = entry_mark[i].m_local_header_ofs;
    }
  }
  for (size_t i = 0; i < n; ++i) {
    if ((entry_mark[i].m_local_header_ofs > d_pos) && (entry_mark[i].type != ZIP_DELETE)) {
      entry_mark[i].type = ZIP_MOVE;
    }
  }
  return err;
}

static ssize_t zip_index_next(mz_uint64 *local_header_ofs_array, ssize_t cur_index) {
  ssize_t new_index = 0;
  for (ssize_t i = cur_index - 1; i >= 0; --i) {
    if (local_header_ofs_array[cur_index] > local_header_ofs_array[i]) {
      new_index = i + 1;
      return new_index;
    }
  }
  return new_index;
}

static ssize_t zip_sort(mz_uint64 *local_header_ofs_array, ssize_t cur_index) {
  ssize_t nxt_index = zip_index_next(local_header_ofs_array, cur_index);
  if (nxt_index != cur_index) {
    mz_uint64 temp = local_header_ofs_array[cur_index];
    for (ssize_t i = cur_index; i > nxt_index; i--) {
      local_header_ofs_array[i] = local_header_ofs_array[i - 1];
    }
    local_header_ofs_array[nxt_index] = temp;
  }
  return nxt_index;
}

static int zip_index_update(zip_entry_mark *entry_mark, ssize_t last_index, ssize_t nxt_index) {
  for (ssize_t j = 0; j < last_index; j++) {
    if (entry_mark[j].file_index >= nxt_index) {
      entry_mark[j].file_index += 1;
    }
  }
  entry_mark[nxt_index].file_index = last_index;
  return 0;
}

static int zip_entry_finalize(zip *zip, zip_entry_mark *entry_mark, const size_t n) {
  mz_uint64 *local_header_ofs_array = (mz_uint64 *)calloc(n, sizeof(mz_uint64));
  if (!local_header_ofs_array) {
    return ZIP_EOOMEM;
  }
  for (size_t i = 0; i < n; ++i) {
    local_header_ofs_array[i] = entry_mark[i].m_local_header_ofs;
    ssize_t index = zip_sort(local_header_ofs_array, i);
    if ((size_t)index != i) {
      zip_index_update(entry_mark, i, index);
    }
    entry_mark[i].file_index = index;
  }
  size_t *length = (size_t *)calloc(n, sizeof(size_t));
  if (!length) {
    CLEANUP(local_header_ofs_array);
    return ZIP_EOOMEM;
  }
  for (size_t i = 0; i < n - 1; i++) {
    length[i] = (size_t)(local_header_ofs_array[i + 1] - local_header_ofs_array[i]);
  }
  length[n - 1] = (size_t)(zip->archive.m_archive_size - local_header_ofs_array[n - 1]);
  for (size_t i = 0; i < n; i++) {
    entry_mark[i].lf_length = length[entry_mark[i].file_index];
  }
  CLEANUP(length);
  CLEANUP(local_header_ofs_array);
  return 0;
}

static ssize_t zip_entry_set(zip *zip, zip_entry_mark *entry_mark, size_t n, char *const entries[], const size_t len) {
  ssize_t err = 0;
  if ((err = zip_entry_mark_entry(zip, entry_mark, n, entries, len)) < 0) {
    return err;
  }
  if ((err = zip_entry_finalize(zip, entry_mark, n)) < 0) {
    return err;
  }
  return 0;
}

static ssize_t zip_entry_setbyindex(zip *zip, zip_entry_mark *entry_mark, size_t n, size_t entries[], const size_t len) {
  ssize_t err = 0;
  if ((err = zip_entry_markbyindex(zip, entry_mark, n, entries, len)) < 0) {
    return err;
  }
  if ((err = zip_entry_finalize(zip, entry_mark, n)) < 0) {
    return err;
  }
  return 0;
}

static ssize_t zip_mem_move(void *pBuf, size_t bufSize, const mz_uint64 to, const mz_uint64 from, const size_t length) {
  uint8_t *dst = NULL, *src = NULL, *end = NULL;
  if (!pBuf) {
    return ZIP_EINVIDX;
  }
  end = (uint8_t *)pBuf + bufSize;
  if (to > bufSize) {
    return ZIP_EINVIDX;
  }
  if (from > bufSize) {
    return ZIP_EINVIDX;
  }
  dst = (uint8_t *)pBuf + to;
  src = (uint8_t *)pBuf + from;
  if (((dst + length) > end) || ((src + length) > end)) {
    return ZIP_EINVIDX;
  }
  memmove(dst, src, length);
  return length;
}

static ssize_t zip_file_move(MZ_FILE *m_pFile, const mz_uint64 to, const mz_uint64 from, const size_t length, mz_uint8 *move_buf, const size_t capacity_size) {
  if (length > capacity_size) {
    return ZIP_ECAPSIZE;
  }
  if (MZ_FSEEK64(m_pFile, from, SEEK_SET)) {
    return ZIP_EFSEEK;
  }
  if (fread(move_buf, 1, length, m_pFile) != length) {
    return ZIP_EFREAD;
  }
  if (MZ_FSEEK64(m_pFile, to, SEEK_SET)) {
    return ZIP_EFSEEK;
  }
  if (fwrite(move_buf, 1, length, m_pFile) != length) {
    return ZIP_EFWRITE;
  }
  return (ssize_t)length;
}

static ssize_t zip_files_move(zip *zip, mz_uint64 writen_num, mz_uint64 read_num, size_t length) {
  ssize_t n = 0;
  const size_t page_size = 1 << 12;
  mz_zip_internal_state *pState = zip->archive.m_pState;
  mz_uint8 *move_buf = (mz_uint8 *)calloc(1, page_size);
  if (!move_buf) {
    return ZIP_EOOMEM;
  }
  ssize_t moved_length = 0;
  ssize_t move_count = 0;
  while ((mz_int64)length > 0) {
    move_count = (length >= page_size) ? page_size : length;
    if (pState->m_pFile) {
      n = zip_file_move(pState->m_pFile, writen_num, read_num, move_count, move_buf, page_size);
    } 
    else if (pState->m_pMem) {
      n = zip_mem_move(pState->m_pMem, pState->m_mem_size, writen_num, read_num, move_count);
    } 
    else {
      return ZIP_ENOFILE;
    }
    if (n < 0) {
      moved_length = n;
      goto cleanup;
    }
    if (n != move_count) {
      goto cleanup;
    }
    writen_num += move_count;
    read_num += move_count;
    length -= move_count;
    moved_length += move_count;
  }
cleanup:
  CLEANUP(move_buf);
  return moved_length;
}

static int zip_central_dir_move(mz_zip_internal_state *pState, int begin, int end, int entry_num) {
  if (begin == entry_num) {
    return 0;
  }
  size_t l_size = 0;
  size_t r_size = 0;
  mz_uint32 d_size = 0;
  mz_uint8 *next = NULL;
  mz_uint8 *deleted = &MZ_ZIP_ARRAY_ELEMENT(&pState->m_central_dir, mz_uint8, MZ_ZIP_ARRAY_ELEMENT(&pState->m_central_dir_offsets, mz_uint32, begin));
  l_size = (size_t)(deleted - (mz_uint8 *)(pState->m_central_dir.m_p));
  if (end == entry_num) {
    r_size = 0;
  } 
  else {
    next = &MZ_ZIP_ARRAY_ELEMENT(&pState->m_central_dir, mz_uint8, MZ_ZIP_ARRAY_ELEMENT(&pState->m_central_dir_offsets, mz_uint32, end));
    r_size = pState->m_central_dir.m_size - (mz_uint32)(next - (mz_uint8 *)(pState->m_central_dir.m_p));
    d_size = (mz_uint32)(next - deleted);
  }
  if (next && l_size == 0) {
    memmove(pState->m_central_dir.m_p, next, r_size);
    pState->m_central_dir.m_p = MZ_REALLOC(pState->m_central_dir.m_p, r_size);
    for (int i = end; i < entry_num; i++) {
      MZ_ZIP_ARRAY_ELEMENT(&pState->m_central_dir_offsets, mz_uint32, i) -= d_size;
    }
  }
  if (next && l_size * r_size != 0) {
    memmove(deleted, next, r_size);
    for (int i = end; i < entry_num; i++) {
      MZ_ZIP_ARRAY_ELEMENT(&pState->m_central_dir_offsets, mz_uint32, i) -= d_size;
    }
  }
  pState->m_central_dir.m_size = l_size + r_size;
  return 0;
}

static int zip_central_dir_delete(mz_zip_internal_state *pState, int *deleted_entry_index_array, int entry_num) {
  int i = 0;
  int begin = 0;
  int end = 0;
  int d_num = 0;
  while (i < entry_num) {
    while ((i < entry_num) && (!deleted_entry_index_array[i])) {
      i++;
    }
    begin = i;
    while ((i < entry_num) && (deleted_entry_index_array[i])) {
      i++;
    }
    end = i;
    zip_central_dir_move(pState, begin, end, entry_num);
  }
  i = 0;
  while (i < entry_num) {
    while ((i < entry_num) && (!deleted_entry_index_array[i])) {
      i++;
    }
    begin = i;
    if (begin == entry_num) {
      break;
    }
    while ((i < entry_num) && (deleted_entry_index_array[i])) {
      i++;
    }
    end = i;
    int k = 0;
    for (int j = end; j < entry_num; j++) {
      MZ_ZIP_ARRAY_ELEMENT(&pState->m_central_dir_offsets, mz_uint32, begin + k) = (mz_uint32)MZ_ZIP_ARRAY_ELEMENT(&pState->m_central_dir_offsets, mz_uint32, j);
      k++;
    }
    d_num += end - begin;
  }
  pState->m_central_dir_offsets.m_size = sizeof(mz_uint32) * (entry_num - d_num);
  return 0;
}

static ssize_t zip_entries_delete_mark(zip *zip, zip_entry_mark *entry_mark, int entry_num) {
  mz_bool *deleted_entry_flag_array = (mz_bool *)calloc(entry_num, sizeof(mz_bool));
  if (deleted_entry_flag_array == NULL) {
    return ZIP_EOOMEM;
  }
  mz_zip_internal_state *pState = zip->archive.m_pState;
  zip->archive.m_zip_mode = MZ_ZIP_MODE_WRITING;
  if (pState->m_pFile) {
    if (MZ_FSEEK64(pState->m_pFile, 0, SEEK_SET)) {
      CLEANUP(deleted_entry_flag_array);
      return ZIP_ENOENT;
    }
  }
  mz_uint64 writen_num = 0;
  mz_uint64 read_num = 0;
  size_t deleted_length = 0;
  size_t move_length = 0;
  size_t deleted_entry_num = 0;
  int i = 0;
  ssize_t n = 0;
  while (i < entry_num) {
    while ((i < entry_num) && (entry_mark[i].type == ZIP_KEEP)) {
      writen_num += entry_mark[i].lf_length;
      read_num = writen_num;
      i++;
    }
    while ((i < entry_num) && (entry_mark[i].type == ZIP_DELETE)) {
      deleted_entry_flag_array[i] = MZ_TRUE;
      read_num += entry_mark[i].lf_length;
      deleted_length += entry_mark[i].lf_length;
      i++;
      deleted_entry_num++;
    }
    while ((i < entry_num) && (entry_mark[i].type == ZIP_MOVE)) {
      move_length += entry_mark[i].lf_length;
      mz_uint8 *p = &MZ_ZIP_ARRAY_ELEMENT(&pState->m_central_dir, mz_uint8, MZ_ZIP_ARRAY_ELEMENT(&pState->m_central_dir_offsets, mz_uint32, i));
      if (!p) {
        CLEANUP(deleted_entry_flag_array);
        return ZIP_ENOENT;
      }
      mz_uint32 offset = MZ_READ_LE32(p + MZ_ZIP_CDH_LOCAL_HEADER_OFS);
      offset -= (mz_uint32)deleted_length;
      MZ_WRITE_LE32(p + MZ_ZIP_CDH_LOCAL_HEADER_OFS, offset);
      i++;
    }
    n = zip_files_move(zip, writen_num, read_num, move_length);
    if (n != (ssize_t)move_length) {
      CLEANUP(deleted_entry_flag_array);
      return n;
    }
    writen_num += move_length;
    read_num += move_length;
  }
  zip->archive.m_archive_size -= (mz_uint64)deleted_length;
  zip->archive.m_total_files = (mz_uint32)entry_num - (mz_uint32)deleted_entry_num;
  zip_central_dir_delete(pState, deleted_entry_flag_array, entry_num);
  CLEANUP(deleted_entry_flag_array);
  return (ssize_t)deleted_entry_num;
}

const char *zip_str_error(int errnum) {
  errnum = -errnum;
  if (errnum <= 0 || errnum >= 33) {
    return NULL;
  }
  return ZIP_ERROR_LIST[errnum];
}

zip *zip_open(const char *zipname, int level, char mode) {
  int errnum = 0;
  return zip_open_with_error(zipname, level, mode, &errnum);
}

zip *zip_open_with_error(const char *zipname, int level, char mode, int *errnum) {
  *errnum = 0;
  if (!zipname || strlen(zipname) < 1) {
    *errnum = ZIP_EINVZIPNAME;
    goto cleanup;
  }
  if (level < 0) {
    level = MZ_DEFAULT_LEVEL;
  }
  if ((level & 0xF) > MZ_UBER_COMPRESSION) {
    *errnum = ZIP_EINVLVL;
    goto cleanup;
  }
  zip *z = calloc((size_t)1, sizeof(zip));
  if (!z) {
    *errnum = ZIP_EOOMEM;
    goto cleanup;
  }
  z->level = (mz_uint)level;
  z->entry.index = -1;
  switch (mode) {
    case 'w':
      if (!mz_zip_writer_init_file_v2(&(z->archive), zipname, 0, MZ_ZIP_FLAG_WRITE_ZIP64)) {
        *errnum = ZIP_EWINIT;
        goto cleanup;
      }
      break;
    case 'r':
      if (!mz_zip_reader_init_file_v2(&(z->archive), zipname, z->level | MZ_ZIP_FLAG_DO_NOT_SORT_CENTRAL_DIRECTORY, 0, 0)) {
        *errnum = ZIP_ERINIT;
        goto cleanup;
      }
      break;
    case 'a':
    case 'd': {
      MZ_FILE *fp = MZ_FOPEN(zipname, "r+b");
      if (!fp) {
        *errnum = ZIP_EOPNFILE;
        goto cleanup;
      }
      if (!mz_zip_reader_init_cfile(&(z->archive), fp, 0, z->level | MZ_ZIP_FLAG_DO_NOT_SORT_CENTRAL_DIRECTORY)) {
        *errnum = ZIP_ERINIT;
        fclose(fp);
        goto cleanup;
      }
      if (!mz_zip_writer_init_from_reader_v2(&(z->archive), zipname, 0)) {
        *errnum = ZIP_EWRINIT;
        fclose(fp);
        mz_zip_reader_end(&(z->archive));
        goto cleanup;
      }
      z->archive.m_zip_type = MZ_ZIP_TYPE_FILE; }
      break;
    default:
      *errnum = ZIP_EINVMODE;
      goto cleanup;
  }
  return z;
cleanup:
  CLEANUP(z);
  return NULL;
}

void zip_close(zip *zip) {
  if (zip) {
    mz_zip_archive *pZip = &(zip->archive);
    if (pZip->m_zip_mode == MZ_ZIP_MODE_WRITING) {
      mz_zip_writer_finalize_archive(pZip);
    }
    if (pZip->m_zip_mode == MZ_ZIP_MODE_WRITING ||
        pZip->m_zip_mode == MZ_ZIP_MODE_WRITING_HAS_BEEN_FINALIZED) {
      zip_archive_truncate(pZip);
      mz_zip_writer_end(pZip);
    }
    if (pZip->m_zip_mode == MZ_ZIP_MODE_READING) {
      mz_zip_reader_end(pZip);
    }
    CLEANUP(zip);
  }
}

int zip_is_64(zip *zip) {
  if (!zip || !zip->archive.m_pState) {
    return ZIP_ENOINIT;
  }
  return (int)zip->archive.m_pState->m_zip64;
}

int zip_offset(zip *zip, uint64_t *offset) {
  if (!zip || !zip->archive.m_pState) {
    return ZIP_ENOINIT;
  }
  *offset = mz_zip_get_archive_file_start_offset(&zip->archive);
  return 0;
}

static int _zip_entry_open(zip *zip, const char *entryname, int case_sensitive) {
  if (!zip) {
    return ZIP_ENOINIT;
  }
  mz_uint64 local_dir_header_ofs = zip->archive.m_archive_size;
  if (!entryname) {
    return ZIP_EINVENTNAME;
  }
  size_t entrylen = strlen(entryname);
  if (entrylen == 0) {
    return ZIP_EINVENTNAME;
  }
  if (zip->entry.name) {
    CLEANUP(zip->entry.name);
  }
  mz_zip_archive *pzip = &(zip->archive);
  mz_zip_archive_file_stat stats;
  int err = 0;
  if (pzip->m_zip_mode == MZ_ZIP_MODE_READING) {
    zip->entry.name = zip_strclone(entryname, entrylen);
    if (!zip->entry.name) {
      return ZIP_EINVENTNAME;
    }
    zip->entry.index = (ssize_t)mz_zip_reader_locate_file(pzip, zip->entry.name, NULL, case_sensitive ? MZ_ZIP_FLAG_CASE_SENSITIVE : 0);
    if (zip->entry.index < (ssize_t)0) {
      err = ZIP_ENOENT;
      goto cleanup;
    }
    if (!mz_zip_reader_file_stat(pzip, (mz_uint)zip->entry.index, &stats)) {
      err = ZIP_ENOENT;
      goto cleanup;
    }
    zip->entry.comp_size = stats.m_comp_size;
    zip->entry.uncomp_size = stats.m_uncomp_size;
    zip->entry.uncomp_crc32 = stats.m_crc32;
    zip->entry.dir_offset = stats.m_central_dir_ofs;
    zip->entry.header_offset = stats.m_local_header_ofs;
    zip->entry.method = stats.m_method;
    zip->entry.external_attr = stats.m_external_attr;
#ifndef MINIZ_NO_TIME
    zip->entry.m_time = stats.m_time;
#endif
    return 0;
  }
  zip->entry.name = zip_strrpl(entryname, entrylen, '\\', '/');
  if (!zip->entry.name) {
    return ZIP_EINVENTNAME;
  }
  mz_uint level = zip->level & 0xF;
  zip->entry.index = (ssize_t)zip->archive.m_total_files;
  zip->entry.comp_size = 0;
  zip->entry.uncomp_size = 0;
  zip->entry.uncomp_crc32 = MZ_CRC32_INIT;
  zip->entry.dir_offset = zip->archive.m_archive_size;
  zip->entry.header_offset = zip->archive.m_archive_size;
  memset(zip->entry.header, 0, MZ_ZIP_LOCAL_DIR_HEADER_SIZE * sizeof(mz_uint8));
  zip->entry.method = level ? MZ_DEFLATED : 0;
#if MZ_PLATFORM == 3 || MZ_PLATFORM == 19
  zip->entry.external_attr = (mz_uint32)(0100644) << 16;
#else
  zip->entry.external_attr = 0;
#endif
  mz_uint num_alignment_padding_bytes = mz_zip_writer_compute_padding_needed_for_file_alignment(pzip);
  if (!pzip->m_pState || (pzip->m_zip_mode != MZ_ZIP_MODE_WRITING)) {
    err = ZIP_EINVMODE;
    goto cleanup;
  }
  if (zip->level & MZ_ZIP_FLAG_COMPRESSED_DATA) {
    err = ZIP_EINVLVL;
    goto cleanup;
  }
  if (!mz_zip_writer_write_zeros(pzip, zip->entry.dir_offset, num_alignment_padding_bytes)) {
    err = ZIP_EMEMSET;
    goto cleanup;
  }
  local_dir_header_ofs += num_alignment_padding_bytes;
  zip->entry.m_time = time(NULL);
  mz_uint16 dos_time = 0, dos_date = 0;
#ifndef MINIZ_NO_TIME
  mz_zip_time_t_to_dos_time(zip->entry.m_time, &dos_time, &dos_date);
#endif
  mz_uint8 extra_data[MZ_ZIP64_MAX_CENTRAL_EXTRA_FIELD_SIZE];
  mz_uint32 extra_size = mz_zip_writer_create_zip64_extra_data(extra_data, NULL, NULL, (local_dir_header_ofs >= MZ_UINT32_MAX) ? &local_dir_header_ofs : NULL);
  if (!mz_zip_writer_create_local_dir_header(pzip, zip->entry.header, entrylen, (mz_uint16)extra_size, 0, 0, 0, zip->entry.method, MZ_ZIP_GENERAL_PURPOSE_BIT_FLAG_UTF8 | MZ_ZIP_LDH_BIT_FLAG_HAS_LOCATOR, dos_time, dos_date)) {
    err = ZIP_EMEMSET;
    goto cleanup;
  }
  zip->entry.header_offset = zip->entry.dir_offset + num_alignment_padding_bytes;
  if (pzip->m_pWrite(pzip->m_pIO_opaque, zip->entry.header_offset, zip->entry.header, sizeof(zip->entry.header)) != sizeof(zip->entry.header)) {
    err = ZIP_EMEMSET;
    goto cleanup;
  }
  if (pzip->m_file_offset_alignment) {
    MZ_ASSERT((zip->entry.header_offset & (pzip->m_file_offset_alignment - 1)) == 0);
  }
  zip->entry.dir_offset += num_alignment_padding_bytes + sizeof(zip->entry.header);
  if (pzip->m_pWrite(pzip->m_pIO_opaque, zip->entry.dir_offset, zip->entry.name, entrylen) != entrylen) {
    err = ZIP_EWRTENT;
    goto cleanup;
  }
  zip->entry.dir_offset += entrylen;
  if (pzip->m_pWrite(pzip->m_pIO_opaque, zip->entry.dir_offset, extra_data, extra_size) != extra_size) {
    err = ZIP_EWRTENT;
    goto cleanup;
  }
  zip->entry.dir_offset += extra_size;
  if (level) {
    zip->entry.state.m_pZip = pzip;
    zip->entry.state.m_cur_archive_file_ofs = zip->entry.dir_offset;
    zip->entry.state.m_comp_size = 0;
    if (tdefl_init(&(zip->entry.comp), mz_zip_writer_add_put_buf_callback, &(zip->entry.state), (int)tdefl_create_comp_flags_from_zip_params( (int)level, -15, MZ_DEFAULT_STRATEGY)) != TDEFL_STATUS_OKAY) {
      err = ZIP_ETDEFLINIT;
      goto cleanup;
    }
  }
  return 0;
cleanup:
  CLEANUP(zip->entry.name);
  return err;
}

int zip_entry_open(zip *zip, const char *entryname) {
  return _zip_entry_open(zip, entryname, 0);
}

int zip_entry_open_case_sensitive(zip *zip, const char *entryname) {
  return _zip_entry_open(zip, entryname, 1);
}

int zip_entry_open_by_index(zip *zip, size_t index) {
  if (!zip) {
    return ZIP_ENOINIT;
  }
  mz_zip_archive *pZip = &(zip->archive);
  if (pZip->m_zip_mode != MZ_ZIP_MODE_READING) {
    return ZIP_EINVMODE;
  }
  if (index >= (size_t)pZip->m_total_files) {
    return ZIP_EINVIDX;
  }
  const mz_uint8 *pHeader;
  if (!(pHeader = &MZ_ZIP_ARRAY_ELEMENT(&pZip->m_pState->m_central_dir, mz_uint8, MZ_ZIP_ARRAY_ELEMENT(&pZip->m_pState->m_central_dir_offsets, mz_uint32, index)))) {
    return ZIP_ENOHDR;
  }
  mz_uint namelen = MZ_READ_LE16(pHeader + MZ_ZIP_CDH_FILENAME_LEN_OFS);
  const char *pFilename = (const char *)pHeader + MZ_ZIP_CENTRAL_DIR_HEADER_SIZE;
  if (zip->entry.name) {
    CLEANUP(zip->entry.name);
  }
  zip->entry.name = zip_strclone(pFilename, namelen);
  if (!zip->entry.name) {
    return ZIP_EINVENTNAME;
  }
  mz_zip_archive_file_stat stats;
  if (!mz_zip_reader_file_stat(pZip, (mz_uint)index, &stats)) {
    return ZIP_ENOENT;
  }
  zip->entry.index = (ssize_t)index;
  zip->entry.comp_size = stats.m_comp_size;
  zip->entry.uncomp_size = stats.m_uncomp_size;
  zip->entry.uncomp_crc32 = stats.m_crc32;
  zip->entry.dir_offset = stats.m_central_dir_ofs;
  zip->entry.header_offset = stats.m_local_header_ofs;
  zip->entry.method = stats.m_method;
  zip->entry.external_attr = stats.m_external_attr;
#ifndef MINIZ_NO_TIME
  zip->entry.m_time = stats.m_time;
#endif
  return 0;
}

int zip_entry_close(zip *zip) {
  int err = 0;
  if (!zip) {
    err = ZIP_ENOINIT;
    goto cleanup;
  }
  mz_zip_archive *pzip = &(zip->archive);
  if (pzip->m_zip_mode == MZ_ZIP_MODE_READING) {
    goto cleanup;
  }
  mz_uint level = zip->level & 0xF;
  tdefl_status done;
  if (level) {
    done = tdefl_compress_buffer(&(zip->entry.comp), "", 0, TDEFL_FINISH);
    if (done != TDEFL_STATUS_DONE && done != TDEFL_STATUS_OKAY) {
      err = ZIP_ETDEFLBUF;
      goto cleanup;
    }
    zip->entry.comp_size = zip->entry.state.m_comp_size;
    zip->entry.dir_offset = zip->entry.state.m_cur_archive_file_ofs;
    zip->entry.method = MZ_DEFLATED;
  }
  mz_uint16 entrylen = (mz_uint16)strlen(zip->entry.name);
  mz_uint16 dos_time = 0, dos_date = 0;
#ifndef MINIZ_NO_TIME
  mz_zip_time_t_to_dos_time(zip->entry.m_time, &dos_time, &dos_date);
#endif
  mz_uint8 local_dir_footer[MZ_ZIP_DATA_DESCRIPTER_SIZE64];
  MZ_WRITE_LE32(local_dir_footer + 0, MZ_ZIP_DATA_DESCRIPTOR_ID);
  MZ_WRITE_LE32(local_dir_footer + 4, zip->entry.uncomp_crc32);
  MZ_WRITE_LE64(local_dir_footer + 8, zip->entry.comp_size);
  MZ_WRITE_LE64(local_dir_footer + 16, zip->entry.uncomp_size);
  mz_uint32 local_dir_footer_size = MZ_ZIP_DATA_DESCRIPTER_SIZE64;
  if (pzip->m_pWrite(pzip->m_pIO_opaque, zip->entry.dir_offset, local_dir_footer, local_dir_footer_size) != local_dir_footer_size) {
    err = ZIP_EWRTHDR;
    goto cleanup;
  }
  zip->entry.dir_offset += local_dir_footer_size;
  mz_uint8 extra_data[MZ_ZIP64_MAX_CENTRAL_EXTRA_FIELD_SIZE];
  mz_uint8 *pExtra_data = extra_data;
  mz_uint32 extra_size = mz_zip_writer_create_zip64_extra_data(extra_data, (zip->entry.uncomp_size >= MZ_UINT32_MAX) ? &zip->entry.uncomp_size : NULL, (zip->entry.comp_size >= MZ_UINT32_MAX) ? &zip->entry.comp_size : NULL, (zip->entry.header_offset >= MZ_UINT32_MAX) ? &zip->entry.header_offset : NULL);
  if ((entrylen) && ISSLASH(zip->entry.name[entrylen - 1]) && !zip->entry.uncomp_size) {
    zip->entry.external_attr |= MZ_ZIP_DOS_DIR_ATTRIBUTE_BITFLAG;
  }
  if (!mz_zip_writer_add_to_central_dir(pzip, zip->entry.name, entrylen, pExtra_data, (mz_uint16)extra_size, "", 0, zip->entry.uncomp_size, zip->entry.comp_size, zip->entry.uncomp_crc32, zip->entry.method, MZ_ZIP_GENERAL_PURPOSE_BIT_FLAG_UTF8 | MZ_ZIP_LDH_BIT_FLAG_HAS_LOCATOR, dos_time, dos_date, zip->entry.header_offset, zip->entry.external_attr, NULL, 0)) { // Cannot write to zip central dir
    err = ZIP_EWRTDIR;
    goto cleanup;
  }
  pzip->m_total_files++;
  pzip->m_archive_size = zip->entry.dir_offset;
cleanup:
  if (zip) {
    zip->entry.m_time = 0;
    zip->entry.index = -1;
    CLEANUP(zip->entry.name);
  }
  return err;
}

const char *zip_entry_name(zip *zip) {
  if (!zip) {
    return NULL;
  }
  return zip->entry.name;
}

ssize_t zip_entry_index(zip *zip) {
  if (!zip) {
    return (ssize_t)ZIP_ENOINIT;
  }
  return zip->entry.index;
}

int zip_entry_is_dir(zip *zip) {
  if (!zip) {
    return ZIP_ENOINIT;
  }
  if (zip->entry.index < (ssize_t)0) {
    return ZIP_EINVIDX;
  }
  mz_uint16 entrylen = (mz_uint16)strlen(zip->entry.name);
  return ISSLASH(zip->entry.name[entrylen - 1]);
}

unsigned long long zip_entry_size(zip *zip) {
  return zip_entry_uncomp_size(zip);
}

unsigned long long zip_entry_uncomp_size(zip *zip) {
  return zip ? zip->entry.uncomp_size : 0;
}

unsigned long long zip_entry_comp_size(zip *zip) {
  return zip ? zip->entry.comp_size : 0;
}

unsigned int zip_entry_crc_32(zip *zip) {
  return zip ? zip->entry.uncomp_crc32 : 0;
}

unsigned long long zip_entry_dir_offset(zip *zip) {
  return zip ? zip->entry.dir_offset : 0;
}

unsigned long long zip_entry_header_offset(zip *zip) {
  return zip ? zip->entry.header_offset : 0;
}

int zip_entry_write(zip *zip, const void *buf, size_t bufsize) {
  if (!zip) {
    return ZIP_ENOINIT;
  }
  mz_zip_archive *pzip = &(zip->archive);
  mz_uint level;
  tdefl_status status;
  if (buf && bufsize > 0) {
    zip->entry.uncomp_size += bufsize;
    zip->entry.uncomp_crc32 = (mz_uint32)mz_crc32(zip->entry.uncomp_crc32, (const mz_uint8 *)buf, bufsize);
    level = zip->level & 0xF;
    if (!level) {
      if ((pzip->m_pWrite(pzip->m_pIO_opaque, zip->entry.dir_offset, buf, bufsize) != bufsize)) {
        return ZIP_EWRTENT;
      }
      zip->entry.dir_offset += bufsize;
      zip->entry.comp_size += bufsize;
    } else {
      status = tdefl_compress_buffer(&(zip->entry.comp), buf, bufsize, TDEFL_NO_FLUSH);
      if (status != TDEFL_STATUS_DONE && status != TDEFL_STATUS_OKAY) {
        return ZIP_ETDEFLBUF;
      }
    }
  }
  return 0;
}

int zip_entry_fwrite(zip *zip, const char *filename) {
  if (!zip) {
    return ZIP_ENOINIT;
  }
  mz_uint8 buf[MZ_ZIP_MAX_IO_BUF_SIZE];
  memset(buf, 0, MZ_ZIP_MAX_IO_BUF_SIZE);
  struct MZ_FILE_STAT_STRUCT file_stat;
  memset((void *)&file_stat, 0, sizeof(struct MZ_FILE_STAT_STRUCT));
  if (MZ_FILE_STAT(filename, &file_stat) != 0) {
    return ZIP_ENOENT;
  }
  mz_uint16 modes;
#if defined(_WIN32) || defined(_WIN64)
  (void)modes;
#else
  modes = file_stat.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO | S_ISUID | S_ISGID | S_ISVTX);
  if (S_ISDIR(file_stat.st_mode)) {
    modes |= UNX_IFDIR;
  }
  if (S_ISREG(file_stat.st_mode)) {
    modes |= UNX_IFREG;
  }
  if (S_ISLNK(file_stat.st_mode)) {
    modes |= UNX_IFLNK;
  }
  if (S_ISBLK(file_stat.st_mode)) {
    modes |= UNX_IFBLK;
  }
  if (S_ISCHR(file_stat.st_mode)) {
    modes |= UNX_IFCHR;
  }
  if (S_ISFIFO(file_stat.st_mode)) {
    modes |= UNX_IFIFO;
  }
  if (S_ISSOCK(file_stat.st_mode)) {
    modes |= UNX_IFSOCK;
  }
  zip->entry.external_attr = (modes << 16) | !(file_stat.st_mode & S_IWUSR);
  if ((file_stat.st_mode & S_IFMT) == S_IFDIR) {
    zip->entry.external_attr |= MZ_ZIP_DOS_DIR_ATTRIBUTE_BITFLAG;
  }
#endif
  zip->entry.m_time = file_stat.st_mtime;
  MZ_FILE *stream = NULL;
  if (!(stream = MZ_FOPEN(filename, "rb"))) {
    return ZIP_EOPNFILE;
  }
  int err = 0;
  size_t n = 0;
  while ((n = fread(buf, sizeof(mz_uint8), MZ_ZIP_MAX_IO_BUF_SIZE, stream)) > 0) {
    if (zip_entry_write(zip, buf, n) < 0) {
      err = ZIP_EWRTENT;
      break;
    }
  }
  fclose(stream);
  return err;
}

ssize_t zip_entry_read(zip *zip, void **buf, size_t *bufsize) {
  if (!zip) {
    return (ssize_t)ZIP_ENOINIT;
  }
  mz_zip_archive *pzip = &(zip->archive);
  if (pzip->m_zip_mode != MZ_ZIP_MODE_READING || zip->entry.index < (ssize_t)0) {
    return (ssize_t)ZIP_ENOENT;
  }
  mz_uint idx = (mz_uint)zip->entry.index;
  if (mz_zip_reader_is_file_a_directory(pzip, idx)) {
    return (ssize_t)ZIP_EINVENTTYPE;
  }
  size_t size = 0;
  *buf = mz_zip_reader_extract_to_heap(pzip, idx, &size, 0);
  if (*buf && bufsize) {
    *bufsize = size;
  }
  return (ssize_t)size;
}

ssize_t zip_entry_no_alloc_read(zip *zip, void *buf, size_t bufsize) {
  if (!zip) {
    return (ssize_t)ZIP_ENOINIT;
  }
  mz_zip_archive *pzip = &(zip->archive);
  if (pzip->m_zip_mode != MZ_ZIP_MODE_READING || zip->entry.index < (ssize_t)0) {
    return (ssize_t)ZIP_ENOENT;
  }
  if (!mz_zip_reader_extract_to_mem_no_alloc(pzip, (mz_uint)zip->entry.index, buf, bufsize, 0, NULL, 0)) {
    return (ssize_t)ZIP_EMEMNOALLOC;
  }
  return (ssize_t)zip->entry.uncomp_size;
}

ssize_t zip_entry_no_alloc_read_with_offset(zip *zip, size_t offset, size_t size, void *buf) {
  if (!zip) {
    return (ssize_t)ZIP_ENOINIT;
  }
  if (offset >= (size_t)zip->entry.uncomp_size) {
    return (ssize_t)ZIP_EINVAL;
  }
  if ((offset + size) > (size_t)zip->entry.uncomp_size) {
    size = (ssize_t)zip->entry.uncomp_size - offset;
  }
  mz_zip_archive *pzip = &(zip->archive);
  if (pzip->m_zip_mode != MZ_ZIP_MODE_READING || zip->entry.index < (ssize_t)0) {
    return (ssize_t)ZIP_ENOENT;
  }
  mz_zip_reader_extract_iter_state *iter = mz_zip_reader_extract_iter_new(pzip, (mz_uint)zip->entry.index, 0);
  if (!iter) {
    return (ssize_t)ZIP_ENORITER;
  }
  mz_uint8 *writebuf = (mz_uint8 *)buf;
  size_t file_offset = 0;
  size_t write_cursor = 0;
  size_t to_read = size;
  while (file_offset < zip->entry.uncomp_size && to_read > 0) {
    size_t nread = mz_zip_reader_extract_iter_read(iter, (void *)&writebuf[write_cursor], to_read);
    if (nread == 0) {
      break;
    }
    if (offset < (file_offset + nread)) {
      size_t read_cursor = offset - file_offset;
      MZ_ASSERT(read_cursor < size);
      size_t read_size = nread - read_cursor;
      if (to_read < read_size) {
        read_size = to_read;
      }
      MZ_ASSERT(read_size <= size);
      if (read_cursor != 0) {
        memmove(&writebuf[write_cursor], &writebuf[read_cursor], read_size);
      }
      write_cursor += read_size;
      offset += read_size;
      to_read -= read_size;
    }
    file_offset += nread;
  }
  mz_zip_reader_extract_iter_free(iter);
  return (ssize_t)write_cursor;
}

int zip_entry_fread(zip *zip, const char *filename) {
  if (!zip) {
    return ZIP_ENOINIT;
  }
  mz_zip_archive_file_stat info;
  memset((void *)&info, 0, sizeof(mz_zip_archive_file_stat));
  mz_zip_archive *pzip = &(zip->archive);
  if (pzip->m_zip_mode != MZ_ZIP_MODE_READING || zip->entry.index < (ssize_t)0) {
    return ZIP_ENOENT;
  }
  mz_uint idx = (mz_uint)zip->entry.index;
  if (mz_zip_reader_is_file_a_directory(pzip, idx)) {
    return ZIP_EINVENTTYPE;
  }

  if (!mz_zip_reader_extract_to_file(pzip, idx, filename, 0)) {
    return ZIP_ENOFILE;
  }
  if (!mz_zip_reader_file_stat(pzip, idx, &info)) {
    return ZIP_ENOFILE;
  }
  mz_uint32 xattr = (info.m_external_attr >> 16) & 0xFFFF;
  if (xattr > 0 && xattr <= MZ_UINT16_MAX) {
    if (CHMOD(filename, (mode_t)xattr) < 0) {
      return ZIP_ENOPERM;
    }
  }
  return 0;
}

int zip_entry_extract(zip *zip, size_t (*on_extract)(void *arg, uint64_t offset, const void *buf, size_t bufsize), void *arg) {
  if (!zip) {
    return ZIP_ENOINIT;
  }
  mz_zip_archive *pzip = &(zip->archive);
  if (pzip->m_zip_mode != MZ_ZIP_MODE_READING || zip->entry.index < (ssize_t)0) {
    return ZIP_ENOENT;
  }
  mz_uint idx = (mz_uint)zip->entry.index;
  return (mz_zip_reader_extract_to_callback(pzip, idx, on_extract, arg, 0)) ? 0 : ZIP_EINVIDX;
}

ssize_t zip_entries_total(zip *zip) {
  if (!zip) {
    return ZIP_ENOINIT;
  }
  return (ssize_t)zip->archive.m_total_files;
}

ssize_t zip_entries_delete(zip *zip, char *const entries[], size_t len) {
  if (zip == NULL || (entries == NULL && len != 0)) {
    return ZIP_ENOINIT;
  }
  if (entries == NULL && len == 0) {
    return 0;
  }
  ssize_t n = zip_entries_total(zip);
  if (n < 0) {
    return n;
  }
  zip_entry_mark *entry_mark = (zip_entry_mark *)calloc((size_t)n, sizeof(zip_entry_mark));
  if (!entry_mark) {
    return ZIP_EOOMEM;
  }
  zip->archive.m_zip_mode = MZ_ZIP_MODE_READING;
  ssize_t err = zip_entry_set(zip, entry_mark, (size_t)n, entries, len);
  if (err < 0) {
    CLEANUP(entry_mark);
    return err;
  }
  err = zip_entries_delete_mark(zip, entry_mark, (int)n);
  CLEANUP(entry_mark);
  return err;
}

ssize_t zip_entries_delete_by_index(zip *zip, size_t entries[], size_t len) {
  if (zip == NULL || (entries == NULL && len != 0)) {
    return ZIP_ENOINIT;
  }
  if (entries == NULL && len == 0) {
    return 0;
  }
  ssize_t n = zip_entries_total(zip);
  if (n < 0) {
    return n;
  }
  zip_entry_mark *entry_mark = (zip_entry_mark *)calloc((size_t)n, sizeof(zip_entry_mark));
  if (!entry_mark) {
    return ZIP_EOOMEM;
  }
  zip->archive.m_zip_mode = MZ_ZIP_MODE_READING;
  ssize_t err = zip_entry_setbyindex(zip, entry_mark, (size_t)n, entries, len);
  if (err < 0) {
    CLEANUP(entry_mark);
    return err;
  }
  err = zip_entries_delete_mark(zip, entry_mark, (int)n);
  CLEANUP(entry_mark);
  return err;
}

int zip_stream_extract(const char *stream, size_t size, const char *dir, int (*on_extract)(const char *filename, void *arg), void *arg) {
  if (!stream || !dir) {
    return ZIP_ENOINIT;
  }
  mz_zip_archive zip_archive;
  if (!memset(&zip_archive, 0, sizeof(mz_zip_archive))) {
    return ZIP_EMEMSET;
  }
  if (!mz_zip_reader_init_mem(&zip_archive, stream, size, 0)) {
    return ZIP_ENOINIT;
  }
  return zip_archive_extract(&zip_archive, dir, on_extract, arg);
}

zip *zip_stream_open(const char *stream, size_t size, int level, char mode) {
  int errnum = 0;
  return zip_stream_open_with_error(stream, size, level, mode, &errnum);
}

zip *zip_stream_open_with_error(const char *stream, size_t size, int level, char mode, int *errnum) {
  zip *z = (zip *)calloc((size_t)1, sizeof(zip));
  if (!z) {
    *errnum = ZIP_EOOMEM;
    return NULL;
  }
  if (level < 0) {
    level = MZ_DEFAULT_LEVEL;
  }
  if ((level & 0xF) > MZ_UBER_COMPRESSION) {
    *errnum = ZIP_EINVLVL;
    goto cleanup;
  }
  z->level = (mz_uint)level;
  if ((stream != NULL) && (size > 0) && (mode == 'r')) {
    if (!mz_zip_reader_init_mem(&(z->archive), stream, size, 0)) {
      *errnum = ZIP_ERINIT;
      goto cleanup;
    }
  } 
  else if ((stream == NULL) && (size == 0) && (mode == 'w')) {
    if (!mz_zip_writer_init_heap(&(z->archive), 0, 1024)) {
      *errnum = ZIP_EWINIT;
      goto cleanup;
    }
  } 
  else {
    *errnum = ZIP_EINVMODE;
    goto cleanup;
  }
  *errnum = 0;
  return z;
cleanup:
  CLEANUP(z);
  return NULL;
}

ssize_t zip_stream_copy(zip *zip, void **buf, size_t *bufsize) {
  if (!zip) {
    return (ssize_t)ZIP_ENOINIT;
  }
  zip_archive_finalize(&(zip->archive));
  size_t n = (size_t)zip->archive.m_archive_size;
  if (bufsize != NULL) {
    *bufsize = n;
  }
  *buf = calloc(n, sizeof(unsigned char));
  memcpy(*buf, zip->archive.m_pState->m_pMem, n);
  return (ssize_t)n;
}

void zip_stream_close(zip *zip) {
  if (zip) {
    mz_zip_writer_end(&(zip->archive));
    mz_zip_reader_end(&(zip->archive));
    CLEANUP(zip);
  }
}

zip *zip_cstream_open(FILE *stream, int level, char mode) {
  int errnum = 0;
  return zip_cstream_open_with_error(stream, level, mode, &errnum);
}

zip *zip_cstream_open_with_error(FILE *stream, int level, char mode, int *errnum) {
  *errnum = 0;
  if (!stream) {
    *errnum = ZIP_ENOFILE;
    goto cleanup;
  }
  if (level < 0)
    level = MZ_DEFAULT_LEVEL;
  if ((level & 0xF) > MZ_UBER_COMPRESSION) {
    *errnum = ZIP_EINVLVL;
    goto cleanup;
  }
  zip *z = (zip *)calloc((size_t)1, sizeof(zip));
  if (!z) {
    *errnum = ZIP_EOOMEM;
    goto cleanup;
  }
  z->level = (mz_uint)level;
  switch (mode) {
    case 'w':
      if (!mz_zip_writer_init_cfile(&(z->archive), stream, MZ_ZIP_FLAG_WRITE_ZIP64)) {
        *errnum = ZIP_EWINIT;
        goto cleanup;
      }
      break;
    case 'r':
      if (!mz_zip_reader_init_cfile(&(z->archive), stream, 0, z->level | MZ_ZIP_FLAG_DO_NOT_SORT_CENTRAL_DIRECTORY)) {
        *errnum = ZIP_ERINIT;
        goto cleanup;
      }
      break;
    case 'a':
    case 'd':
      if (!mz_zip_reader_init_cfile(&(z->archive), stream, 0, z->level | MZ_ZIP_FLAG_DO_NOT_SORT_CENTRAL_DIRECTORY)) {
        *errnum = ZIP_ERINIT;
        goto cleanup;
      }
      if ((mode == 'a' || mode == 'd')) {
        if (!mz_zip_writer_init_from_reader_v2(&(z->archive), NULL, 0)) {
          *errnum = ZIP_EWRINIT;
          mz_zip_reader_end(&(z->archive));
          goto cleanup;
        }
      }
      break;
    default:
      *errnum = ZIP_EINVMODE;
      goto cleanup;
  }
  return z;
cleanup:
  CLEANUP(z);
  return NULL;
}

void zip_cstream_close(zip *zip) { 
  zip_close(zip);
}

int zip_create(const char *zipname, const char *filenames[], size_t len) {
  if (!zipname || strlen(zipname) < 1) {
    return ZIP_EINVZIPNAME;
  }
  mz_zip_archive zip_archive;
  if (!memset(&(zip_archive), 0, sizeof(zip_archive))) {
    return ZIP_EMEMSET;
  }
  if (!mz_zip_writer_init_file(&zip_archive, zipname, 0)) {
    return ZIP_ENOINIT;
  }
  struct MZ_FILE_STAT_STRUCT file_stat;
  if (!memset((void *)&file_stat, 0, sizeof(struct MZ_FILE_STAT_STRUCT))) {
    return ZIP_EMEMSET;
  }
  int err = 0;
  for (size_t i = 0; i < len; ++i) {
    const char *name = filenames[i];
    if (!name) {
      err = ZIP_EINVENTNAME;
      break;
    }
    if (MZ_FILE_STAT(name, &file_stat) != 0) {
      err = ZIP_ENOFILE;
      break;
    }
#if defined(_WIN32) || defined(_WIN64)
    (void)modes; // unused
#else
    mz_uint16 modes = file_stat.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO | S_ISUID | S_ISGID | S_ISVTX);
    if (S_ISDIR(file_stat.st_mode)) {
      modes |= UNX_IFDIR;
    }
    if (S_ISREG(file_stat.st_mode)) {
      modes |= UNX_IFREG;
    }
    if (S_ISLNK(file_stat.st_mode)) {
      modes |= UNX_IFLNK;
    }
    if (S_ISBLK(file_stat.st_mode)) {
      modes |= UNX_IFBLK;
    }
    if (S_ISCHR(file_stat.st_mode)) {
      modes |= UNX_IFCHR;
    }
    if (S_ISFIFO(file_stat.st_mode)) {
      modes |= UNX_IFIFO;
    }
    if (S_ISSOCK(file_stat.st_mode)) {
      modes |= UNX_IFSOCK;
    }
    mz_uint32 ext_attributes = (modes << 16) | !(file_stat.st_mode & S_IWUSR);
    if ((file_stat.st_mode & S_IFMT) == S_IFDIR) {
      ext_attributes |= MZ_ZIP_DOS_DIR_ATTRIBUTE_BITFLAG;
    }
#endif
    if (!mz_zip_writer_add_file(&zip_archive, zip_basename(name), name, "", 0, ZIP_DEFAULT_COMPRESSION_LEVEL, ext_attributes)) {
      err = ZIP_ENOFILE;
      break;
    }
  }
  mz_zip_writer_finalize_archive(&zip_archive);
  mz_zip_writer_end(&zip_archive);
  return err;
}

int zip_extract(const char *zipname, const char *dir, int (*on_extract)(const char *filename, void *arg), void *arg) {
  if (!zipname || !dir) {
    return ZIP_EINVZIPNAME;
  }
  mz_zip_archive zip_archive;
  if (!memset(&zip_archive, 0, sizeof(mz_zip_archive))) {
    return ZIP_EMEMSET;
  }
  if (!mz_zip_reader_init_file(&zip_archive, zipname, 0)) {
    return ZIP_ENOINIT;
  }
  return zip_archive_extract(&zip_archive, dir, on_extract, arg);
}

