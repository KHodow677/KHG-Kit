#pragma once

#include <stdbool.h>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

enum utl_ascii85_errs_e {
  utl_ascii85_err_out_buf_too_small = -255,
  utl_ascii85_err_in_buf_too_large,
  utl_ascii85_err_bad_decode_char,
  utl_ascii85_err_decode_overflow
};

typedef enum {
  utl_conversion_ok,
  utl_source_exhausted,
  utl_target_exhausted,
  utl_source_illegal
} utl_conversion_result;

typedef enum {
  utl_strict_conversion,
  utl_lenient_conversion
} utl_conversion_flags;

void utl_encoding_hex_dump(const void *data, unsigned int size);
void utl_encoding_initialize(void);

char *utl_encoding_base64_encode(const char *input, unsigned int length);
char *utl_encoding_base64_decode(const char *input, unsigned int length);
char *utl_encoding_url_encode(const char *input, unsigned int length);
char *utl_encoding_url_decode(const char *input, unsigned int lenght);
char *utl_encoding_base32_encode(const char *input, unsigned int length);
char *utl_encoding_base32_decode(const char *input, unsigned int length);
char *utl_encoding_base16_encode(const char *input, unsigned int length);
char *utl_encoding_base16_decode(const char *input, unsigned int length);
char *utl_encoding_base58_encode(const void *data, unsigned int binsz);
char *utl_encoding_base58_decode(const char *b58, unsigned int *binszp);
char *utl_encoding_base91_encode(const unsigned char *data, unsigned int length);
char *utl_encoding_base85_encode(const unsigned char *input, unsigned int length);

unsigned short *utl_encoding_utf8_to_utf16(const unsigned char *input, unsigned int length);
unsigned short *utl_encoding_utf32_to_utf16(const unsigned int *input, unsigned int length);

unsigned int *utl_encoding_utf16_to_utf32(const unsigned short *input, unsigned int length);
unsigned int *utl_encoding_utf8_to_utf32(const unsigned char *input, unsigned int length);

unsigned char *utl_encoding_utf16_to_utf8(const unsigned short *input, unsigned int length);
unsigned char *utl_encoding_utf32_to_utf8(const unsigned int *input, unsigned int length);

unsigned char *utl_encoding_base85_decode(const char *input, unsigned int length);
unsigned char *utl_encoding_base91_decode(const char *encoded, unsigned int *decoded_length);

bool utl_encoding_is_utf8(const unsigned char *input, unsigned int length);
bool utl_encoding_is_utf8_string(const unsigned char **input, unsigned int length);

