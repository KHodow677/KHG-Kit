#include "khg_tea/tea.h"
#include "khg_utl/error_func.h"
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#if defined(_WIN32) || defined(_WIN64)
    #include <io.h>
    #define O_BINARY _O_BINARY
#else
    #include <sys/types.h>
    #include <sys/stat.h>
    #define O_BINARY 0 // O_BINARY is not needed on POSIX systems
#endif

void tea_decode(uint32_t *v, uint32_t *k) {
  uint32_t v0 = v[0], v1 = v[1], delta = 0x9e3779b9, n = 32, sum = delta * 32;
  while(n--) {
    v1 -= ((v0<<4) + k[2]) ^ (v0 + sum) ^ ((v0>>5) + k[3]);
    v0 -= ((v1<<4) + k[0]) ^ (v1 + sum) ^ ((v1>>5) + k[1]);
    sum -= delta;
  }
  v[0] = v0;
  v[1] = v1;
}

void tea_code(uint32_t *v, uint32_t *k) {
  uint32_t v0 = v[0], v1 = v[1], delta = 0x9e3779b9, n = 32, sum = 0;
  while(n--) {
    sum += delta;
    v0 += ((v1<<4) + k[0]) ^ (v1 + sum) ^ ((v1>>5) + k[1]);
    v1 += ((v0<<4) + k[2]) ^ (v0 + sum) ^ ((v0>>5) + k[3]);
  }
  v[0] = v0;
  v[1] = v1;
}

bool tea_encrypt_decrypt(int mode, char *key, int flags, char *in_file, char *out_file) {
  char d[DATA_SIZE];
  int len, inf, outf;
  outf = (flags & TEA_FLAG_OUTPUT_STDOUT) ? STDOUT_FILENO : 0;
  if (access(in_file, F_OK) == -1){
    error_func("Skipping, Input file does not exists", user_defined_data);
    return false;
  }
  if ((inf = open(in_file, O_RDONLY|O_BINARY)) == -1) {
    error_func("Open - input", user_defined_data);
    return false;
  }
  if ( outf == 0 ) {
    if ((outf = open(out_file, O_CREAT|O_WRONLY|O_BINARY, DEFAULT_FILE_CREATION_MODE)) == -1) {
      error_func("Open - output", user_defined_data);
      close(inf);
      return false;
    }
  }
  while ((len = read(inf,d,DATA_SIZE)) > 0) {
    memset(&d[len], 0, DATA_SIZE - len);    
    if (mode == ENCRYPT) {
      tea_code((uint32_t *) d, (uint32_t *) key);
    }
    else {
      tea_decode((uint32_t *) d, (uint32_t *) key);
    }
    if ((len = write(outf,d,DATA_SIZE)) < 0) {
      error_func("Write", user_defined_data);
      break;
    }
  }
  if (len < 0){
    error_func("Read / Write", user_defined_data);
    goto failed_exit;
  }
  close(inf);
  if (outf != STDOUT_FILENO) {
    close(outf);
  }
  return true;
  failed_exit:
    close(inf);
    if (outf != STDOUT_FILENO) {
      close(outf);
    }
    return false;
}
