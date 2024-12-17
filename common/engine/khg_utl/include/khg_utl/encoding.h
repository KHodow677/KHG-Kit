#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#else
#include <wchar.h>
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

void utl_encoding_hex_dump(const void *data, size_t size);
void utl_encoding_initialize(void);

#if defined(_WIN32) || defined(_WIN64)
wchar_t *utl_encoding_utf8_to_wchar(const char *utf8Str);
#endif 

char *utl_encoding_wchar_to_utf8(const wchar_t* wstr);

char *utl_encoding_base64_encode(const char *input, size_t length);
char *utl_encoding_base64_decode(const char *input, size_t length);
char *utl_encoding_url_encode(const char *input, size_t length);
char *utl_encoding_url_decode(const char *input, size_t lenght);
char *utl_encoding_base32_encode(const char *input, size_t length);
char *utl_encoding_base32_decode(const char *input, size_t length);
char *utl_encoding_base16_encode(const char *input, size_t length);
char *utl_encoding_base16_decode(const char *input, size_t length);
char *utl_encoding_base58_encode(const void *data, size_t binsz);
char *utl_encoding_base58_decode(const char *b58, size_t *binszp);
char *utl_encoding_base91_encode(const uint8_t *data, size_t length);
char *utl_encoding_base85_encode(const uint8_t *input, size_t length);

uint16_t *utl_encoding_utf8_to_utf16(const uint8_t *input, size_t length);
uint16_t *utl_encoding_utf32_to_utf16(const uint32_t *input, size_t length);

uint32_t *utl_encoding_utf16_to_utf32(const uint16_t *input, size_t length);
uint32_t *utl_encoding_utf8_to_utf32(const uint8_t *input, size_t length);

uint8_t *utl_encoding_utf16_to_utf8(const uint16_t *input, size_t length);
uint8_t *utl_encoding_utf32_to_utf8(const uint32_t *input, size_t length);

uint8_t *utl_encoding_base85_decode(const char *input, size_t length);
uint8_t *utl_encoding_base91_decode(const char *encoded, size_t *decoded_length);

bool utl_encoding_is_utf8(const uint8_t *input, size_t length);
bool utl_encoding_is_utf8_string(const uint8_t **input, size_t length);

