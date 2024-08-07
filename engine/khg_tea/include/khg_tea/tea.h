#pragma once

#include <stdint.h>
#include <stdbool.h>

#define KEY_SIZE 16
#define DATA_SIZE 8

#define TEA_FLAG_OUTPUT_STDOUT 1 << 0
#define DEFAULT_FILE_CREATION_MODE    00644

enum opmode {
  UNSET, 
  ENCRYPT, 
  DECRYPT
};

void tea_code(uint32_t v[1], uint32_t k[3]);
void tea_decode(uint32_t v[2], uint32_t k[4]);
bool tea_encrypt_decrypt(int mode, char *key, int flags, char *in_file, char *out_file);
