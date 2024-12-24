#include "khg_utl/secret.h"
#include "khg_utl/error_func.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#if defined(_WIN32) || defined (_WIN64)
  #include <windows.h>
  #include <bcrypt.h>
#else
  #include <fcntl.h>
  #include <unistd.h>
#endif

void utl_secret_token_bytes(unsigned char *buffer, size_t size) {
#ifdef _WIN32
  if (!BCRYPT_SUCCESS(BCryptGenRandom(NULL, buffer, (ULONG)size, BCRYPT_USE_SYSTEM_PREFERRED_RNG))) {
    utl_error_func("BCryptGenRandom failed", utl_user_defined_data);
    exit(EXIT_FAILURE);
  }
#else
  int fd = open("/dev/urandom", O_RDONLY);
  if (fd < 0) {
    utl_error_func("Unable to open /dev/urandom", utl_user_defined_data);
    exit(EXIT_FAILURE);
  }
  if (read(fd, buffer, size) != (ssize_t)size) {
    utl_error_func("Unable to read from /dev/urandom", utl_user_defined_data);
    close(fd);
    exit(EXIT_FAILURE);
  }
  close(fd);
#endif
}

int utl_secret_randbelow(int n) {
  unsigned char buffer[sizeof(int)];
  utl_secret_token_bytes(buffer, sizeof(buffer));
  int random_value = *(int *)buffer;
  int result = abs(random_value) % n;
  return result;
}

void utl_secret_token_hex(char *buffer, size_t nbytes) {
  unsigned char *bytes = malloc(nbytes);
  utl_secret_token_bytes(bytes, nbytes);
  for (size_t i = 0; i < nbytes; i++) {
    sprintf(buffer + (i * 2), "%02x", bytes[i]);
  }
  free(bytes);
}

void utl_secret_token_urlsafe(char *buffer, size_t nbytes) {
  static const char urlsafe_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";
  unsigned char *bytes = malloc(nbytes);
  utl_secret_token_bytes(bytes, nbytes);
  for (size_t i = 0; i < nbytes; i++) {
    buffer[i] = urlsafe_table[bytes[i] % 64];
  }
  free(bytes);
}

int utl_secret_compare_digest(const unsigned char *a, const unsigned char *b, size_t length) {
  unsigned char result = 0;
  for (size_t i = 0; i < length; i++) {
    result |= a[i] ^ b[i];
  }
  return result == 0;
}

void *utl_secret_choice(const void* seq, size_t size, size_t elem_size) {
  if (size == 0) {
    utl_error_func("Cannot choose from an empty sequence", utl_user_defined_data);
    return NULL;
  }
  int random_index = utl_secret_randbelow(size);
  return (void*)((char*)seq + (random_index * elem_size)); 
}

unsigned int utl_secret_randbits(int k) {
  if (k <= 0 || k > (int)(sizeof(unsigned int) * CHAR_BIT)) {
    utl_error_func("K is out of range", utl_user_defined_data);
    exit(EXIT_FAILURE);
  }
  unsigned int random_value = 0;
  utl_secret_token_bytes((unsigned char *)&random_value, sizeof(random_value));
  random_value &= (1U << k) - 1;
  return random_value;
}
