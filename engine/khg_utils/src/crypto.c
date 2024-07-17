#include "khg_utils/crypto.h"
#include "khg_utils/error_func.h"
#include <stdlib.h>
#include <string.h>
#include "openssl/md4.h"
#include "openssl/md5.h"
#include "openssl/evp.h"
#include "openssl/sha.h"
#include "openssl/ripemd.h"
#include "openssl/mdc2.h"
#include "openssl/des.h"
#include "openssl/err.h"

#ifndef DES_BLOCK_SIZE
#define DES_BLOCK_SIZE 8
#endif

uint8_t* crypto_hash_data(const uint8_t *data, size_t length, HashAlgorithm algorithm, size_t *outLength) {
  if (!outLength) {
    return NULL;
  }
  *outLength = 0;
  uint8_t *md = NULL;
  EVP_MD_CTX *mdctx;
  const EVP_MD *md_type;
  unsigned char md_value[EVP_MAX_MD_SIZE];
  switch (algorithm) {
    case CRYPTO_MD4:
      md = (uint8_t *)malloc(MD4_DIGEST_LENGTH);
      if (md && MD4(data, length, md)) {
        *outLength = MD4_DIGEST_LENGTH;
      }
      break;
    case CRYPTO_MD5:
      md = (uint8_t *)malloc(MD5_DIGEST_LENGTH);
      if (md && MD5(data, length, md)) {
        *outLength = MD5_DIGEST_LENGTH;
      }
      break;
    case CRYPTO_SHA1:
      md = (uint8_t *)malloc(SHA_DIGEST_LENGTH);
      if (md && SHA1(data, length, md)) {
        *outLength = SHA_DIGEST_LENGTH;
      }
      break;
    case CRYPTO_SHA224:
      md = (uint8_t *)malloc(SHA224_DIGEST_LENGTH);
      if (md && SHA224(data, length, md)) {
        *outLength = SHA224_DIGEST_LENGTH;
      }
      break;
    case CRYPTO_SHA256:
      md = (uint8_t *)malloc(SHA256_DIGEST_LENGTH);
      if (md && SHA256(data, length, md)) {
        *outLength = SHA256_DIGEST_LENGTH;
      }
      break;
    case CRYPTO_SHA384:
      md = (uint8_t *)malloc(SHA384_DIGEST_LENGTH);
      if (md && SHA384(data, length, md)) {
        *outLength = SHA384_DIGEST_LENGTH;
      }
      break;
    case CRYPTO_SHA512:
      md = (uint8_t *)malloc(SHA512_DIGEST_LENGTH);
      if (md && SHA512(data, length, md)) {
        *outLength = SHA512_DIGEST_LENGTH;
      }
      break;
    case CRYPTO_SHA3_224:
      md_type = EVP_sha3_224();
      *outLength = SHA224_DIGEST_LENGTH;
      md = (uint8_t *)malloc(*outLength);
      if (!md) {
        error_func("Failed to allocate memory for hash", user_defined_data);
        return NULL;
      }
      break;
    case CRYPTO_SHA3_256:
      md_type = EVP_sha3_256();
      *outLength = SHA256_DIGEST_LENGTH;
      md = (uint8_t *)malloc(*outLength);
      if (!md) {
        error_func("Failed to allocate memory for hash", user_defined_data);
        return NULL;
      }
      break;
    case CRYPTO_SHA3_384:
      md_type = EVP_sha3_384();
      *outLength = SHA384_DIGEST_LENGTH;
      md = (uint8_t *)malloc(*outLength);
      if (!md) {
        error_func("Failed to allocate memory for hash", user_defined_data);
        return NULL;
      }
      break;
    case CRYPTO_SHA3_512:
      md_type = EVP_sha3_512();
      *outLength = SHA512_DIGEST_LENGTH;
      md = (uint8_t *)malloc(*outLength);
      if (!md) {
        error_func("Failed to allocate memory for hash", user_defined_data);
        return NULL;
      }
      break;
    case CRYPTO_SHAKE_128:
      md_type = EVP_shake128();
      *outLength = 32;
      break;
    case CRYPTO_SHAKE_256:
      md_type = EVP_shake256();
      *outLength = 64;
      break;
    case CRYPTO_BLAKE2B_512:
      md_type = EVP_blake2b512();
      *outLength = 64;
      break;
    case CRYPTO_BLAKE2S_256:
      md_type = EVP_blake2s256();
      *outLength = 32;
      break;
    case CRYPTO_MDC2:
      *outLength = MDC2_DIGEST_LENGTH;
      md = (uint8_t *)malloc(*outLength);
      if (!md) {
        error_func("Failed to allocate memory for hash", user_defined_data);
        return NULL;
      }
      if (!MDC2(data, length, md)) {
        error_func("Failed to compute MDC2 hash", user_defined_data);
        free(md);
        return NULL;
      }
      break;
    case CRYPTO_RIPEMD_160:
      md_type = EVP_ripemd160();
      *outLength = RIPEMD160_DIGEST_LENGTH;
      break;
    case CRYPTO_SHA512_224:
      md_type = EVP_sha512_224();
      *outLength = 28;
      break;
    default:
      error_func("Unsupported Algorithm", user_defined_data);
      break;
  }
  if (algorithm == CRYPTO_RIPEMD_160 || algorithm == CRYPTO_SHA512_224 || algorithm == CRYPTO_BLAKE2B_512 || algorithm == CRYPTO_BLAKE2S_256) {
    md = (uint8_t *)malloc(*outLength);
    if (!md) {
      error_func("Failed to allocate memory for hash", user_defined_data);
      return NULL;
    }
    mdctx = EVP_MD_CTX_new();
    if (!mdctx || !EVP_DigestInit_ex(mdctx, md_type, NULL) ||
      !EVP_DigestUpdate(mdctx, data, length) ||
      !EVP_DigestFinal_ex(mdctx, md, (unsigned int *)outLength)) {
      error_func("Failed to compute hash", user_defined_data);
      free(md);
      EVP_MD_CTX_free(mdctx);
      return NULL;
    }
    EVP_MD_CTX_free(mdctx);
  }
  else if (algorithm == CRYPTO_SHAKE_128 || algorithm == CRYPTO_SHAKE_256) {
    md = (uint8_t *)malloc(*outLength);
    if (!md) {
        perror("Failed to allocate memory for hash");
        return NULL;
    }
    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    if (!mdctx || !EVP_DigestInit_ex(mdctx, md_type, NULL) || !EVP_DigestUpdate(mdctx, data, length) || !EVP_DigestFinalXOF(mdctx, md, *outLength)) {
      error_func("Failed to compute SHAKE hash", user_defined_data);
      free(md);
      EVP_MD_CTX_free(mdctx);
      return NULL;
    }
    EVP_MD_CTX_free(mdctx);
  }
  else if (algorithm >= CRYPTO_SHA3_224 && algorithm <= CRYPTO_SHA3_512) {
    if ((mdctx = EVP_MD_CTX_new()) == NULL) {
      error_func("Failed to create EVP_MD_CTX", user_defined_data);
      free(md);
      return NULL;
    }
    if (1 != EVP_DigestInit_ex(mdctx, md_type, NULL) || 1 != EVP_DigestUpdate(mdctx, data, length) || 1 != EVP_DigestFinal_ex(mdctx, md_value, (unsigned int *)outLength)) {
      error_func("Failed to compute hash", user_defined_data);
      EVP_MD_CTX_free(mdctx);
      free(md);
      return NULL;
    }
    memcpy(md, md_value, *outLength);
    EVP_MD_CTX_free(mdctx);
  }
  return md;
}

void crypto_print_hash(const uint8_t *hash, size_t length) {
  if (hash == NULL) {
    error_func("No hash data to print", user_defined_data);
    return;
  }
  for (size_t i = 0; i < length; i++) {
    printf("%02x\n", hash[i]);
  }
}

void *crypto_des_encrypt(const uint8_t *plaintext, size_t len, const uint8_t *key, const uint8_t *iv, CryptoMode mode, size_t *out_len) {
  if (!out_len || !key || !plaintext) {
    return NULL;
  }
  size_t padded_len = (len + DES_BLOCK_SIZE - 1) & ~(DES_BLOCK_SIZE - 1);
  *out_len = padded_len;
  uint8_t *padded_plaintext = calloc(padded_len, sizeof(uint8_t));
  if (!padded_plaintext) {
    return NULL;
  }
  memcpy(padded_plaintext, plaintext, len);
  uint8_t *ciphertext = malloc(padded_len);
  if (!ciphertext) {
    free(padded_plaintext);
    return NULL;
  }
  DES_cblock keyBlock;
  memcpy(keyBlock, key, DES_BLOCK_SIZE);
  DES_key_schedule keysched;
  DES_set_odd_parity(&keyBlock);
  DES_set_key_checked(&keyBlock, &keysched);
  DES_cblock ivec;
  if (iv) {
    memcpy(ivec, iv, DES_BLOCK_SIZE);
  } 
  else {
    memset(ivec, 0, DES_BLOCK_SIZE);
  }
  for (size_t i = 0; i < padded_len; i += DES_BLOCK_SIZE) {
    DES_cblock block;
    memcpy(block, padded_plaintext + i, DES_BLOCK_SIZE);
    switch (mode) {
      case CRYPTO_MODE_ECB:
        DES_ecb_encrypt(&block, (DES_cblock*)(ciphertext + i), &keysched, DES_ENCRYPT);
        break;
      case CRYPTO_MODE_CBC:
        DES_ncbc_encrypt(block, ciphertext + i, DES_BLOCK_SIZE, &keysched, &ivec, DES_ENCRYPT);
        break;
      case CRYPTO_MODE_CFB:{
        DES_cblock ivec = {0};
        DES_cfb_encrypt(padded_plaintext, ciphertext, 8, padded_len, &keysched, &ivec, DES_ENCRYPT);
      }
      break;
      case CRYPTO_MODE_OFB:
        DES_ofb_encrypt(block, ciphertext + i, 8, DES_BLOCK_SIZE, &keysched, &ivec);
        break;
      default:
        free(ciphertext);
        free(padded_plaintext);
        return NULL;
    }
  }
  free(padded_plaintext);
  return ciphertext;
}

void *crypto_des_decrypt(const uint8_t *ciphertext, size_t len, const uint8_t *key, const uint8_t *iv, CryptoMode mode, size_t *out_len) {
  if (!out_len || !key || !ciphertext) {
    return NULL;
  }
  *out_len = len;
  uint8_t* plaintext = malloc(len);
  if (!plaintext) {
    return NULL;
  }
  DES_cblock keyBlock;
  memcpy(keyBlock, key, DES_BLOCK_SIZE);
  DES_key_schedule keysched;
  DES_set_odd_parity(&keyBlock);
  DES_set_key_checked(&keyBlock, &keysched);
  DES_cblock ivec;
  if (iv) {
    memcpy(ivec, iv, DES_BLOCK_SIZE);
  } 
  else {
    memset(ivec, 0, DES_BLOCK_SIZE);
  }
  for (size_t i = 0; i < len; i += DES_BLOCK_SIZE) {
    DES_cblock block;
    memcpy(block, ciphertext + i, DES_BLOCK_SIZE);
    switch (mode) {
      case CRYPTO_MODE_ECB:
        DES_ecb_encrypt(&block, (DES_cblock*)(plaintext + i), &keysched, DES_DECRYPT);
        break;
      case CRYPTO_MODE_CBC:
        DES_ncbc_encrypt(block, plaintext + i, DES_BLOCK_SIZE, &keysched, &ivec, DES_DECRYPT);
        break;
      case CRYPTO_MODE_CFB:{
        DES_cfb_encrypt(ciphertext, plaintext, 8, len, &keysched, &ivec, DES_ENCRYPT);
      }
      break;
      case CRYPTO_MODE_OFB:
        DES_ofb_encrypt(block, plaintext + i, 8, DES_BLOCK_SIZE, &keysched, &ivec);
        break;
      default:
        free(plaintext);
        return NULL;
    }
  }
  return plaintext;
}

#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#include <Wincrypt.h>

void crypto_generate_random_iv(uint8_t *iv, size_t length) {
  HCRYPTPROV hCryptProv;
  if (!CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) {
    error_func("CryptAcquireContext failed", user_defined_data);
    exit(-1);
  }
  if (!CryptGenRandom(hCryptProv, length, iv)) {
    error_func("CryptGenRandom failed", user_defined_data);
    CryptReleaseContext(hCryptProv, 0);
    exit(-1);
  }
  CryptReleaseContext(hCryptProv, 0);
}

#else
#include <fcntl.h>
#include <unistd.h>

void crypto_generate_random_iv(uint8_t *iv, size_t length) {
  int fd = open("/dev/urandom", O_RDONLY);
  if (fd == -1) {
    error_func("Open /dev/urandom", user_defined_data);
    exit(-1);
  }
  ssize_t read_bytes = read(fd, iv, length);
  if (read_bytes != (ssize_t)length) {
    error_func("read from /dev/urandom", user_defined_data);
    close(fd);
    exit(-1);
  }
  close(fd);
}

#endif