#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define DES_BLOCK_SIZE 8

typedef enum {
  crypto_md4,
  crypto_md5,
  crypto_sha1,
  crypto_sha224,
  crypto_sha256,
  crypto_sha384,
  crypto_sha512,
  crypto_sha3_224,
  crypto_sha3_256,
  crypto_sha3_384,
  crypto_sha3_512,
  crypto_shake_128,
  crypto_shake_256,
  crypto_blake2b_512,
  crypto_blake2s_256,
  crypto_mdc2,
  crypto_ripemd_160,
  crypto_sha512_224,
} hash_algorithm ;

typedef enum {
  crypto_mode_ecb,
  crypto_mode_cbc,
  crypto_mode_cfb,
  crypto_mode_ofb,
} crypto_mode;

uint8_t *crypto_hash_data(const uint8_t* data, size_t length, hash_algorithm algorithm, size_t *outLength);

void crypto_print_hash(const uint8_t* hash, size_t length);
void crypto_generate_random_iv(uint8_t *iv, size_t length);

void *crypto_des_encrypt(const uint8_t* plaintext, size_t len, const uint8_t* key, const uint8_t* iv, crypto_mode mode, size_t* out_len);
void *crypto_des_decrypt(const uint8_t* ciphertext, size_t len, const uint8_t* key, const uint8_t* iv, crypto_mode mode, size_t* out_len);
