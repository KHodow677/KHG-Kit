#include "khg_utl/encoding.h"
#include "khg_utl/error_func.h"
#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <locale.h>

bool (*b58_sha256_impl)(void *, const void *, unsigned int) = NULL;

#define UNI_REPLACEMENT_CHAR (unsigned int)0x0000FFFD
#define UNI_SUR_HIGH_START (unsigned int)0xD800
#define UNI_SUR_HIGH_END (unsigned int)0xDBFF
#define UNI_SUR_LOW_START (unsigned int)0xDC00
#define UNI_SUR_LOW_END (unsigned int)0xDFFF
#define UNI_MAX_BMP (unsigned int)0x0000FFFF
#define UNI_MAX_UTF16 (unsigned int)0x0010FFFF
#define UNI_MAX_UTF32 (unsigned int)0x7FFFFFFF
#define UNI_MAX_LEGAL_UTF32 (unsigned int)0x0010FFFF
#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof((arr)[0]))
#define b58_almostmaxint_bits (sizeof(unsigned int) * 8)

static const char base64_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static const char base32[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567=";
static const char base16_chars[] = "0123456789ABCDEF";
static const int halfShift = 10;
static const unsigned char firstByteMark[7] = { 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };
static const char trailingBytesForUTF8[256] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5
};
static const unsigned int offsetsFromUTF8[6] = { 0x00000000UL, 0x00003080UL, 0x000E2080UL, 0x03C82080UL, 0xFA082080UL, 0x82082080UL };
static const unsigned int halfBase = 0x0010000UL;
static const unsigned int halfMask = 0x3FFUL;
static const char b58digits_ordered[] = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";
static const int8_t b58digits_map[] = {
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, -1, -1, -1, -1, -1, -1,
  -1, 9, 10, 11, 12, 13, 14, 15, 16, -1, 17, 18, 19, 20, 21, -1,
  22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, -1, -1, -1, -1, -1,
  -1, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, -1, 44, 45, 46,
  47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, -1, -1, -1, -1, -1,
};
static const char BASE91_ALPHABET[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!#$%&()*+,./:;<=>?@[]^_`{|}~\"";

static int base91_decode_value(char c) {
  for (int i = 0; i < 91; ++i) {
    if (BASE91_ALPHABET[i] == c) {
      return i;
    }
  }
  return -1;
}

bool utl_encoding_is_utf8(const unsigned char *input, unsigned int length) {
  unsigned char a;
  const unsigned char *srcptr = input + length;
  switch (length) {
  default:
    utl_error_func("Invalid sequence length", utl_user_defined_data);
    return false;
  case 4:
    if ((a = (*--srcptr)) < 0x80 || a > 0xBF) {
      utl_error_func("Invalid continuation byte in 4-byte sequence", utl_user_defined_data);
      return false;
    }
  case 3:
    if ((a = (*--srcptr)) < 0x80 || a > 0xBF) {
      utl_error_func("Invalid continuation byte in 3-byte sequence", utl_user_defined_data);
      return false;
    }
  case 2:
    if ((a = (*--srcptr)) < 0x80 || a > 0xBF) {
      utl_error_func("Invalid continuation byte in 2-byte sequence", utl_user_defined_data);
      return false;
    }
    switch (*input) {
      case 0xE0:
        if (a < 0xA0) {
          utl_error_func("Overlong encoding for 0xE0", utl_user_defined_data);
          return false;
        }
        break;
      case 0xED:
        if (a > 0x9F) {
          utl_error_func("Invalid character for 0xED", utl_user_defined_data);
          return false;
        }
        break;
      case 0xF0:
        if (a < 0x90) {
          utl_error_func("Overlong encoding for 0xF0", utl_user_defined_data);
          return false;
        }
        break;
      case 0xF4:
        if (a > 0x8F) {
          utl_error_func("Invalid character for 0xF4", utl_user_defined_data);
          return false;
        }
        break;
      default:
        if (a < 0x80) {
          utl_error_func("Invalid leading byte in UTF-8 sequence", utl_user_defined_data);
          return false;
        }
    }
  case 1:
    if (*input >= 0x80 && *input < 0xC2) {
      utl_error_func("Overlong sequence detected", utl_user_defined_data);
      return false;
    }
  }
  if (*input > 0xF4) {
    utl_error_func("Invalid leading byte greater than 0xF4", utl_user_defined_data);
    return false;
  }
  return true;
}

static utl_conversion_result ConvertUTF16toUTF8(const unsigned short **sourceStart, const unsigned short *sourceEnd, unsigned char **targetStart, unsigned char *targetEnd, utl_conversion_flags flags) {
  utl_conversion_result result = utl_conversion_ok;
  const unsigned short *source = *sourceStart;
  unsigned char *target = *targetStart;
  while (source < sourceEnd) {
    unsigned int ch;
    unsigned short bytesToWrite = 0;
    const unsigned int byteMask = 0xBF;
    const unsigned int byteMark = 0x80; 
    const unsigned short* oldSource = source;
    ch = *source++;
    if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_HIGH_END) {
      if (source < sourceEnd) {
        unsigned int ch2 = *source;
        if (ch2 >= UNI_SUR_LOW_START && ch2 <= UNI_SUR_LOW_END) {
          ch = ((ch - UNI_SUR_HIGH_START) << halfShift) + (ch2 - UNI_SUR_LOW_START) + halfBase;
          ++source;
        } 
        else if (flags == utl_strict_conversion) {
          utl_error_func("Unpaired high surrogate in strict mode", utl_user_defined_data);
          --source;
          result = utl_source_illegal;
          break;
        }
      } 
      else {
        utl_error_func("Source exhausted, missing low surrogate", utl_user_defined_data);
        --source;
        result = utl_source_exhausted;
        break;
      }
    } 
    else if (flags == utl_strict_conversion) {
      if (ch >= UNI_SUR_LOW_START && ch <= UNI_SUR_LOW_END) {
        utl_error_func("Unpaired low surrogate in strict mode", utl_user_defined_data);
        --source;
        result = utl_source_illegal;
        break;
      }
    }
    if (ch < (unsigned int)0x80) {      
      bytesToWrite = 1;
    } 
    else if (ch < (unsigned int)0x800) {     
      bytesToWrite = 2;
    } 
    else if (ch < (unsigned int)0x10000) {   
      bytesToWrite = 3;
    } 
    else if (ch < (unsigned int)0x110000) {  
      bytesToWrite = 4;
    } 
    else {          
      utl_error_func("Illegal UTF-16 code point, replacing with replacement character", utl_user_defined_data);                  
      bytesToWrite = 3;
      ch = UNI_REPLACEMENT_CHAR;
    }
    target += bytesToWrite;
    if (target > targetEnd) {
      utl_error_func("Target buffer exhausted", utl_user_defined_data);
      source = oldSource;
      target -= bytesToWrite; result = utl_target_exhausted; 
      break;
    }
    switch (bytesToWrite) {
      case 4: 
        *--target = (unsigned char)((ch | byteMark) & byteMask); ch >>= 6;
      case 3: 
        *--target = (unsigned char)((ch | byteMark) & byteMask); ch >>= 6;
      case 2: 
        *--target = (unsigned char)((ch | byteMark) & byteMask); ch >>= 6;
      case 1: 
        *--target =  (unsigned char)(ch | firstByteMark[bytesToWrite]);
    }
    target += bytesToWrite;
  }
  *sourceStart = source;
  *targetStart = target;
  return result;
}

static utl_conversion_result ConvertUTF32toUTF8(const unsigned int **sourceStart, const unsigned int *sourceEnd, unsigned char **targetStart, unsigned char *targetEnd, utl_conversion_flags flags) {
  utl_conversion_result result = utl_conversion_ok;
  const unsigned int *source = *sourceStart;
  unsigned char *target = *targetStart;
  while (source < sourceEnd) {
    unsigned int ch;
    unsigned short bytesToWrite = 0;
    const unsigned int byteMask = 0xBF;
    const unsigned int byteMark = 0x80; 
    ch = *source++;
    if (flags == utl_strict_conversion ) {
      if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_LOW_END) {
        utl_error_func("Illegal surrogate in strict mode", utl_user_defined_data);
        --source;
        result = utl_source_illegal;
        break;
      }
    }
    if (ch < (unsigned int)0x80) {      
      bytesToWrite = 1;
    } 
    else if (ch < (unsigned int)0x800) {     
      bytesToWrite = 2;
    } 
    else if (ch < (unsigned int)0x10000) {   
      bytesToWrite = 3;
    } 
    else if (ch <= UNI_MAX_LEGAL_UTF32) {  
      bytesToWrite = 4;
    } 
    else {                            
      bytesToWrite = 3;
      ch = UNI_REPLACEMENT_CHAR;
      result = utl_source_illegal;
      utl_error_func("Illegal UTF-32 code point, replaced with replacement char", utl_user_defined_data);
    }
    target += bytesToWrite;
    if (target > targetEnd) {
      utl_error_func("Target buffer exhausted", utl_user_defined_data);
      --source;
      target -= bytesToWrite; result = utl_target_exhausted; 
      break;
    }
    switch (bytesToWrite) {
      case 4: 
        *--target = (unsigned char)((ch | byteMark) & byteMask); ch >>= 6;
      case 3: 
        *--target = (unsigned char)((ch | byteMark) & byteMask); ch >>= 6;
      case 2: 
        *--target = (unsigned char)((ch | byteMark) & byteMask); ch >>= 6;
      case 1: 
        *--target = (unsigned char) (ch | firstByteMark[bytesToWrite]);
    }
    target += bytesToWrite;
  }
  *sourceStart = source;
  *targetStart = target;
  return result;
}

static utl_conversion_result ConvertUTF8toUTF16(const unsigned char **sourceStart, const unsigned char *sourceEnd, unsigned short **targetStart, unsigned short *targetEnd, utl_conversion_flags flags) {
  utl_conversion_result result = utl_conversion_ok;
  const unsigned char *source = *sourceStart;
  unsigned short *target = *targetStart;
  while (source < sourceEnd) {
    unsigned int ch = 0;
    unsigned short extraBytesToRead = trailingBytesForUTF8[*source];
    if (extraBytesToRead >= sourceEnd - source) {
      utl_error_func("Source exhausted", utl_user_defined_data);
      result = utl_source_exhausted; 
      break;
    }
    if (!utl_encoding_is_utf8(source, extraBytesToRead + 1)) {
      utl_error_func("Illegal UTF-8 sequence detected", utl_user_defined_data);
      result = utl_source_illegal;
      break;
    }
    switch (extraBytesToRead) {
      case 5: 
        ch += *source++;
        ch <<= 6;
      case 4: 
        ch += *source++;
        ch <<= 6;
      case 3: 
        ch += *source++;
        ch <<= 6;
      case 2: 
        ch += *source++;
        ch <<= 6;
      case 1: 
        ch += *source++;
        ch <<= 6;
      case 0: 
        ch += *source++;
    }
    ch -= offsetsFromUTF8[extraBytesToRead];
    if (target >= targetEnd) {
      utl_error_func("Target buffer exhausted", utl_user_defined_data);
      source -= (extraBytesToRead+1);
      result = utl_target_exhausted; break;
    }
    if (ch <= UNI_MAX_BMP) {
        if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_LOW_END) {
          if (flags == utl_strict_conversion) {
            utl_error_func("Illegal surrogate found in strict mode", utl_user_defined_data);
            source -= (extraBytesToRead+1);
            result = utl_source_illegal;
            break;
          } 
          else {
            utl_error_func("Replacing illegal surrogate with replacement character", utl_user_defined_data);
            *target++ = UNI_REPLACEMENT_CHAR;
          }
        } 
        else {
          *target++ = (unsigned short)ch;
        }
    } 
    else if (ch > UNI_MAX_UTF16) {
      if (flags == utl_strict_conversion) {
        utl_error_func("Illegal UTF-16 code point in strict mode", utl_user_defined_data);
        result = utl_source_illegal;
        source -= (extraBytesToRead + 1);
        break;
      } 
      else {
        *target++ = UNI_REPLACEMENT_CHAR;
      }
    } 
    else {
      if (target + 1 >= targetEnd) {
        source -= (extraBytesToRead + 1);
        utl_error_func("Target buffer exhausted for surrogate pair", utl_user_defined_data);
        result = utl_target_exhausted; break;
      }
      ch -= halfBase;
      *target++ = (unsigned short)((ch >> halfShift) + UNI_SUR_HIGH_START);
      *target++ = (unsigned short)((ch & halfMask) + UNI_SUR_LOW_START);
    }
  }
  *sourceStart = source;
  *targetStart = target;
  return result;
}

utl_conversion_result ConvertUTF8toUTF32(const unsigned char **sourceStart, const unsigned char *sourceEnd, unsigned int **targetStart, unsigned int *targetEnd, utl_conversion_flags flags) {
  utl_conversion_result result = utl_conversion_ok;
  const unsigned char *source = *sourceStart;
  unsigned int *target = *targetStart;
  while (source < sourceEnd) {
    unsigned int ch = 0;
    unsigned short extraBytesToRead = trailingBytesForUTF8[*source];
    if (extraBytesToRead >= sourceEnd - source) {
      utl_error_func("Source exhausted", utl_user_defined_data);
      result = utl_source_exhausted; 
      break;
    }
    if (!utl_encoding_is_utf8(source, extraBytesToRead+1)) {
      utl_error_func("Illegal UTF-8 sequence detected", utl_user_defined_data);
      result = utl_source_illegal;
      break;
    }
    switch (extraBytesToRead) {
      case 5: 
        ch += *source++; ch <<= 6;
      case 4: 
        ch += *source++; ch <<= 6;
      case 3: 
        ch += *source++; ch <<= 6;
      case 2: 
        ch += *source++; ch <<= 6;
      case 1: 
        ch += *source++; ch <<= 6;
      case 0: 
        ch += *source++;
    }
    ch -= offsetsFromUTF8[extraBytesToRead];
    if (target >= targetEnd) {
      utl_error_func("Target buffer exhausted", utl_user_defined_data);
      source -= (extraBytesToRead+1);
      result = utl_target_exhausted; break;
    }
    if (ch <= UNI_MAX_LEGAL_UTF32) {
      if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_LOW_END) {
        if (flags == utl_strict_conversion) {
          utl_error_func("Illegal surrogate found in strict mode", utl_user_defined_data);
          source -= (extraBytesToRead + 1);
          result = utl_source_illegal;
          break;
        } 
        else {
          *target++ = UNI_REPLACEMENT_CHAR;
        }
      } 
      else {
        *target++ = ch;
      }
    } 
    else {
      utl_error_func("Invalid UTF-32 code point", utl_user_defined_data);
      result = utl_source_illegal;
      *target++ = UNI_REPLACEMENT_CHAR;
    }
  }
  *sourceStart = source;
  *targetStart = target;
  return result;
}

static int decode_char(unsigned char c) {
  char retval = -1;
  if (c >= 'A' && c <= 'Z') {
    retval = c - 'A';
  } 
  else if (c >= '2' && c <= '7') {
    retval = c - '2' + 26;
  }
  assert(retval == -1 || ((retval & 0x1F) == retval));
  if (retval == -1) {
    utl_error_func("Invalid character", utl_user_defined_data);
  }
  return retval;
}

static int get_octet(int block) {
  assert(block >= 0 && block < 8);
  int octet = (block * 5) / 8;
  return octet;
}

static int get_offset(int block) {
  assert(block >= 0 && block < 8);
  int offset = (8 - 5 - (5 * block) % 8);
  return offset;
}

static unsigned char shift_right(unsigned char byte, signed char offset) {
  unsigned char result;
  if (offset > 0) {
    result = byte >> offset;
  } 
  else {
    result = byte << -offset;
  }
  return result;
}

static unsigned char shift_left(unsigned char byte, signed char offset) {
  unsigned char result = shift_right(byte, -offset);
  return result;
}

static int decode_sequence(const unsigned char *coded, unsigned char *plain) {
  assert(CHAR_BIT == 8);
  assert(coded && plain);
  plain[0] = 0;
  for (int block = 0; block < 8; block++) {
    int offset = get_offset(block);
    int octet = get_octet(block);
    int c = decode_char(coded[block]);
    if (c < 0) {
      utl_error_func("Invalid character at block", utl_user_defined_data);
      return octet;
    }
    plain[octet] |= shift_left(c, offset);
    if (offset < 0) {
      assert(octet < 4);
      plain[octet + 1] = shift_left(c, 8 + offset);
      utl_error_func("Block overflows to next octet", utl_user_defined_data);
    }
  }
  return 5;
}

char *utl_encoding_base64_encode(const char *input, unsigned int length) {
  unsigned int output_length = 4 * ((length + 2) / 3);
  char *encoded = malloc(output_length + 1);
  if (!encoded) {
    utl_error_func("Memory allocation failed", utl_user_defined_data);
    return NULL;
  }
  unsigned int i, j;
  for (i = 0, j = 0; i < length;) {
    unsigned int octet_a = i < length ? (unsigned char)input[i++] : 0;
    unsigned int octet_b = i < length ? (unsigned char)input[i++] : 0;
    unsigned int octet_c = i < length ? (unsigned char)input[i++] : 0;
    unsigned int triple = (octet_a << 16) + (octet_b << 8) + octet_c;
    encoded[j++] = base64_chars[(triple >> 18) & 0x3F];
    encoded[j++] = base64_chars[(triple >> 12) & 0x3F];
    encoded[j++] = base64_chars[(triple >> 6) & 0x3F];
    encoded[j++] = base64_chars[triple & 0x3F];
  }
  for (unsigned int k = 0; k < (3 - length % 3) % 3; k++) {
    encoded[output_length - 1 - k] = '=';
  }
  encoded[output_length] = '\0';
  return encoded;
}

char *utl_encoding_base64_decode(const char *input, unsigned int length) {
  if (length % 4 != 0) {
    utl_error_func("Invalid input length, length must be a multiple of 4", utl_user_defined_data);
    return NULL;
  }
  unsigned int output_length = length / 4 * 3;
  if (input[length - 1] == '=') {
    output_length--;
  }
  if (input[length - 2] == '=') {
    output_length--;
  }
  char *decoded = malloc(output_length + 1);
  if (!decoded) {
    utl_error_func("Memory allocation failed", utl_user_defined_data);
    return NULL;
  }
  static const unsigned char d[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 62, 0, 0, 0, 63, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 0, 0, 0, 0, 0, 0,
    0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 0, 0, 0, 0, 0,
    0, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51
  };
  for (unsigned int i = 0, j = 0; i < length;) {
    unsigned int sextet_a = input[i] == '=' ? 0 & i++ : d[(unsigned char)input[i++]];
    unsigned int sextet_b = input[i] == '=' ? 0 & i++ : d[(unsigned char)input[i++]];
    unsigned int sextet_c = input[i] == '=' ? 0 & i++ : d[(unsigned char)input[i++]];
    unsigned int sextet_d = input[i] == '=' ? 0 & i++ : d[(unsigned char)input[i++]];
    unsigned int triple = (sextet_a << 18) + (sextet_b << 12) + (sextet_c << 6) + sextet_d;
    if (j < output_length) {
      decoded[j++] = (triple >> 16) & 0xFF;
    }
    if (j < output_length) {
      decoded[j++] = (triple >> 8) & 0xFF;
    }
    if (j < output_length) {
      decoded[j++] = triple & 0xFF;
    }
  }
  decoded[output_length] = '\0';
  return decoded;
}

char *utl_encoding_url_encode(const char *input, unsigned int length) {
  char *result = malloc(3 * length + 1);
  if (!result) {
    utl_error_func("Memory allocation failed", utl_user_defined_data);
    return NULL;
  }
  unsigned int result_index = 0;
  for (unsigned int i = 0; i < length; ++i) {
    char ch = input[i];
    if (isalnum(ch) || ch == '-' || ch == '.' || ch == '_' || ch == '~') {
      result[result_index++] = ch;
    } 
    else {
      static const char hex[] = "0123456789ABCDEF";
      result[result_index++] = '%';
      result[result_index++] = hex[(ch >> 4) & 0x0F];
      result[result_index++] = hex[ch & 0x0F];
    }
  }
  result[result_index] = '\0';
  utl_error_func("Encoding completed successfully", utl_user_defined_data);
  return result;
}

char *utl_encoding_url_decode(const char *input, unsigned int length) {
  char *result = malloc(length + 1);
  if (!result) {
    utl_error_func("Memory allocation failed", utl_user_defined_data);
    return NULL;
  }
  unsigned int result_index = 0;
  for (unsigned int i = 0; i < length; ++i) {
    char ch = input[i];
    if (ch == '%') {
      if (i + 2 >= length) {
        utl_error_func("Incomplete percent-encoding", utl_user_defined_data);
        free(result);
        return NULL;
      }
      static const char hex[] = "0123456789ABCDEF";
      char hi = input[++i];
      char lo = input[++i];
      int hi_index = strchr(hex, toupper(hi)) - hex;
      int lo_index = strchr(hex, toupper(lo)) - hex;
      if (hi_index < 0 || hi_index >= 16 || lo_index < 0 || lo_index >= 16) {
        utl_error_func("Invalid hex characters in percent-encoding", utl_user_defined_data);
        free(result);
        return NULL;
      }
      result[result_index++] = (char)((hi_index << 4) + lo_index);
    } 
    else if (ch == '+') {
      result[result_index++] = ' ';
    } 
    else {
      result[result_index++] = ch;
    }
  }
  result[result_index] = '\0';
  return result;
}

char *utl_encoding_base32_encode(const char *input, unsigned int length) {
  unsigned int output_length = ((length + 4) / 5) * 8;
  char *encoded = malloc(output_length + 1);
  if (!encoded) {
    utl_error_func("Memory allocation failed", utl_user_defined_data);
    return NULL;
  }
  unsigned int input_index = 0;
  unsigned int output_index = 0;
  unsigned int bit_count = 0;
  unsigned int buffer = 0;
  while (input_index < length) {
    buffer = (buffer << 8) | (unsigned char)input[input_index++];
    bit_count += 8;
    while (bit_count >= 5) {
      encoded[output_index++] = base32[(buffer >> (bit_count - 5)) & 0x1F];
      bit_count -= 5;
    }
  }
  if (bit_count > 0) {
    encoded[output_index++] = base32[(buffer << (5 - bit_count)) & 0x1F];
  }
  while (output_index < output_length) {
    encoded[output_index++] = '=';
  }
  encoded[output_index] = '\0';
  return encoded;
}

char *utl_encoding_base32_decode(const char *input, unsigned int length) {
  if (length % 8 != 0) {
    utl_error_func("Invalid input length, length must be a multiple of 8", utl_user_defined_data);
    return NULL;
  }
  unsigned int olength = (length / 8) * 5;
  unsigned char *result = malloc(olength + 1);
  if (!result) {
    utl_error_func("Memory allocation failed", utl_user_defined_data);
    return NULL;
  }
  unsigned int i = 0, j = 0;
  while (i < length) {
    if (input[i] == '=') {
      break; 
    }
    int n = decode_sequence((const unsigned char *)&input[i], &result[j]);
    if (n < 5) {
      j += n;
      break;
    }
    i += 8;
    j += 5;
  }
  result[j] = '\0';
  return (char *)result;
}

char *utl_encoding_base16_encode(const char *input, unsigned int length) {
  unsigned int output_length = length * 2;
  char *encoded = malloc(output_length + 1);
  if (!encoded) {
    utl_error_func("Memory allocation failed", utl_user_defined_data);
    return NULL;
  }
  for (unsigned int i = 0, j = 0; i < length; ++i) {
    unsigned char ch = (unsigned char)input[i];
    encoded[j++] = base16_chars[(ch & 0xF0) >> 4];
    encoded[j++] = base16_chars[ch & 0x0F];
  }
  encoded[output_length] = '\0';
  return encoded;
}

char *utl_encoding_base16_decode(const char *input, unsigned int length) {
  if (input == NULL) {
    utl_error_func("Invalid input parameter", utl_user_defined_data);
    return NULL;
  }
  static const unsigned char base16_decode[128] = {
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
  };
  if (length % 2 != 0) {
    utl_error_func("Invalid input length", utl_user_defined_data);
    return NULL; 
  }
  unsigned int olength = length / 2;
  char *decoded = malloc(olength + 1);
  if (!decoded) {
    utl_error_func("Memory allocation failed for decoded string", utl_user_defined_data);
    return NULL;
  }
  for (unsigned int i = 0, j = 0; i < length;) {
    unsigned char a = base16_decode[(unsigned char)input[i++]];
    unsigned char b = base16_decode[(unsigned char)input[i++]];
    if (a == 0xFF || b == 0xFF) {
      utl_error_func("Invalid character encountered", utl_user_defined_data);
      free(decoded);
      return NULL;
    }
    decoded[j++] = (a << 4) | b;
  }
  decoded[olength] = '\0';
  return decoded;
}

unsigned short *utl_encoding_utf32_to_utf16(const unsigned int *input, unsigned int length) {
  if (input == NULL || length == 0) {
    utl_error_func("Invalid input or length", utl_user_defined_data);
    return NULL;
  }
  unsigned short *output = malloc(sizeof(unsigned short) * (length * 2 + 1));
  if (!output) {
    utl_error_func("Memory allocation failed", utl_user_defined_data);
    return NULL;
  }
  unsigned int j = 0;
  for (unsigned int i = 0; i < length; ++i) {
    unsigned int ch = input[i];
    if (ch > UNI_MAX_LEGAL_UTF32) {
      utl_error_func("Invalid character encountered", utl_user_defined_data);
      free(output);
      return NULL;
    }
    if (ch <= UNI_MAX_BMP) {
      output[j++] = (unsigned short)ch;
    } 
    else if (ch > UNI_MAX_BMP && ch <= UNI_MAX_UTF16) {
      ch -= halfBase;
      output[j++] = (unsigned short)((ch >> halfShift) + UNI_SUR_HIGH_START);
      output[j++] = (unsigned short)((ch & halfMask) + UNI_SUR_LOW_START);
    }
  }
  output[j] = 0;
  return output;
}

unsigned int *utl_encoding_utf16_to_utf32(const unsigned short *input, unsigned int length) {
  if (input == NULL || length == 0) {
    utl_error_func("Invalid input or length", utl_user_defined_data);
    return NULL;
  }
  unsigned int *output = malloc(sizeof(unsigned int) * (length + 1));
  if (!output) {
    utl_error_func("Memory allocation failed", utl_user_defined_data);
    return NULL;
  }
  unsigned int j = 0;
  for (unsigned int i = 0; i < length; ++i) {
    unsigned int ch = input[i];
    if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_HIGH_END) {
        if (i + 1 < length) {
          unsigned int ch2 = input[i + 1];
          if (ch2 >= UNI_SUR_LOW_START && ch2 <= UNI_SUR_LOW_END) {
            ch = ((ch - UNI_SUR_HIGH_START) << 10) + (ch2 - UNI_SUR_LOW_START) + 0x10000;
            i++;
          } 
          else {
            utl_error_func("Invalid surrogate pair", utl_user_defined_data);
            free(output);
            return NULL;
          }
        } 
        else {
          utl_error_func("Lone high surrogate without a low surrogate", utl_user_defined_data);
          free(output);
          return NULL;
        }
    }
    output[j++] = ch;
  }
  output[j] = 0;
  return output;
}

unsigned char *utl_encoding_utf16_to_utf8(const unsigned short *input, unsigned int length) {
  if (input == NULL || length == 0) {
    utl_error_func("Invalid input or length", utl_user_defined_data);
    return NULL;
  }
  unsigned int maxOutLength = length * 4;
  unsigned char *output = (unsigned char *)malloc(maxOutLength);
  if (!output) {
    utl_error_func("Memory allocation failed", utl_user_defined_data);
    return NULL;
  }
  const unsigned short *sourceStart = input;
  const unsigned short *sourceEnd = input + length;
  unsigned char *targetStart = output;
  unsigned char *targetEnd = output + maxOutLength;
  utl_conversion_result result = ConvertUTF16toUTF8(&sourceStart, sourceEnd, &targetStart, targetEnd, utl_lenient_conversion);
  if (result != utl_conversion_ok) {
    utl_error_func("Conversion from UTF-16 to UTF-8 failed", utl_user_defined_data);
    free(output);
    return NULL;
  }
  unsigned int actualLength = targetStart - output;
  unsigned char* resizedOutput = (unsigned char*)realloc(output, actualLength + 1);
  if (!resizedOutput) {
    utl_error_func("Reallocation failed", utl_user_defined_data);
    free(output);
    return NULL;
  }
  resizedOutput[actualLength] = '\0';
  return resizedOutput;
}

unsigned char *utl_encoding_utf32_to_utf8(const unsigned int *input, unsigned int length) {
  if (input == NULL || length == 0) {
    utl_error_func("Invalid input or length", utl_user_defined_data);
    return NULL;
  }
  unsigned int maxOutLength = length * 4;
  unsigned char *output = (unsigned char *)malloc(maxOutLength);
  if (!output) {
    utl_error_func("Memory allocation failed", utl_user_defined_data);
    return NULL;
  }
  const unsigned int *sourceStart = input;
  const unsigned int *sourceEnd = input + length;
  unsigned char *targetStart = output;
  unsigned char *targetEnd = output + maxOutLength;
  utl_conversion_result result = ConvertUTF32toUTF8(&sourceStart, sourceEnd, &targetStart, targetEnd, utl_lenient_conversion);
  if (result != utl_conversion_ok) {
    utl_error_func("Conversion from UTF-32 to UTF-8 failed", utl_user_defined_data);
    free(output);
    return NULL;
  }
  unsigned int actualLength = targetStart - output;
  unsigned char *resizedOutput = (unsigned char*)realloc(output, actualLength + 1);
  if (!resizedOutput) {
    utl_error_func("Reallocation failed", utl_user_defined_data);
    free(output);
    return NULL;
  }
  resizedOutput[actualLength] = '\0';
  return resizedOutput;
}

bool utl_encoding_is_utf8_string(const unsigned char **input, unsigned int length) {
  if (input == NULL || *input == NULL || length == 0) {
    utl_error_func("Invalid input or length", utl_user_defined_data);
    return false;
  }
  const unsigned char *source = *input;
  const unsigned char *sourceEnd = source + length;
  while (source < sourceEnd) {
    int trailLength = trailingBytesForUTF8[*source] + 1;
    if (trailLength > sourceEnd - source || !utl_encoding_is_utf8(source, trailLength)) {
      utl_error_func("Invalid UTF-8 encoding detected", utl_user_defined_data);
      return false;
    }
    source += trailLength;
  }
  *input = source;
  return true;
}

unsigned short *utl_encoding_utf8_to_utf16(const unsigned char *input, unsigned int length) {
  if (input == NULL || length == 0) {
    utl_error_func("Invalid input or length", utl_user_defined_data);
    return NULL;
  }
  unsigned int maxOutLength = length * 2;
  unsigned short *output = malloc(maxOutLength * sizeof(unsigned short));
  if (!output) {
    utl_error_func("Memory allocation failed for output", utl_user_defined_data);
    return NULL;
  }
  const unsigned char *sourceStart = input;
  const unsigned char *sourceEnd = input + length;
  unsigned short *targetStart = output;
  unsigned short *targetEnd = output + maxOutLength;
  utl_conversion_result result = ConvertUTF8toUTF16(&sourceStart, sourceEnd, &targetStart, targetEnd, utl_lenient_conversion);
  if (result != utl_conversion_ok) {
    utl_error_func("Conversion from UTF-8 to UTF-16 failed", utl_user_defined_data);
    free(output);
    return NULL;
  }
  unsigned int actualLength = targetStart - output;
  unsigned short *resizedOutput = realloc(output, actualLength * sizeof(unsigned short) + 1);
  if (!resizedOutput) {
    utl_error_func("Reallocation failed", utl_user_defined_data);
    free(output);
    return NULL;
  }
  resizedOutput[actualLength] = '\0';
  return resizedOutput;
}

unsigned int *utl_encoding_utf8_to_utf32(const unsigned char *input, unsigned int length) {
  if (input == NULL || length == 0) {
    utl_error_func("Invalid input or length", utl_user_defined_data);
    return NULL;
  }
  unsigned int maxOutLength = length;
  unsigned int *output = malloc(maxOutLength * sizeof(unsigned int));
  if (!output) {
    utl_error_func("Memory allocation failed", utl_user_defined_data);
    return NULL;
  }
  const unsigned char *sourceStart = input;
  const unsigned char *sourceEnd = input + length;
  unsigned int *targetStart = output;
  unsigned int *targetEnd = output + maxOutLength;
  utl_conversion_result result = ConvertUTF8toUTF32(&sourceStart, sourceEnd, &targetStart, targetEnd, utl_lenient_conversion);
  if (result != utl_conversion_ok) {
    utl_error_func("Conversion to UTF-32 failed", utl_user_defined_data);
    free(output);
    return NULL;
  }
  unsigned int actualLength = targetStart - output;
  unsigned int *resizedOutput = (unsigned int *)realloc(output, (actualLength + 1) * sizeof(unsigned int));
  if (resizedOutput) {
    resizedOutput[actualLength] = 0;
    return resizedOutput;
  }
  return output;
}

void utl_encoding_hex_dump(const void *data, unsigned int size) {
  if (!data || size == 0) {
    utl_error_func("Invalid input data or size", utl_user_defined_data);
    return;
  }
  const unsigned char *byte = (const unsigned char *)data;
  unsigned int i, j;
  for (i = 0; i < size; i += 16) {
    printf("%i  ", i);
    for (j = 0; j < 16; j++) {
      if (i + j < size) {
        printf("%i ", byte[i + j]);
      } 
      else {
        printf("   ");
      }
    }
    printf(" |");
    for (j = 0; j < 16; j++) {
      if (i + j < size) {
        printf("%c", isprint(byte[i + j]) ? byte[i + j] : '.');
      }
    }
    printf("|\n");
  }
}

char *utl_encoding_base85_encode(const unsigned char *input, unsigned int length) {
  if (input == NULL || length == 0) {
    utl_error_func("Invalid input or length", utl_user_defined_data);
    return NULL;
  }
  unsigned int encoded_max_length = ((length + 3) / 4) * 5 + 2;
  char* encoded = malloc(encoded_max_length);
  if (!encoded) {
    utl_error_func("Memory allocation failed for encoded string", utl_user_defined_data);
    return NULL;
  }
  unsigned int input_index = 0;
  unsigned int encoded_index = 0;
  while (input_index < length) {
    unsigned int acc = 0;
    unsigned int chunk_len = (length - input_index < 4) ? (length - input_index) : 4;
    for (unsigned int i = 0; i < chunk_len; ++i) {
      acc = (acc << 8) | input[input_index++];
    }
    if (chunk_len < 4) {
      acc <<= (4 - chunk_len) * 8;
    }
    if (acc == 0 && chunk_len == 4) {
      encoded[encoded_index++] = 'z';
    } 
    else {
      for (int i = 4; i >= 0; --i) {
        encoded[encoded_index + i] = (acc % 85) + 33;
        acc /= 85;
      }
      encoded_index += 5;
      if (chunk_len < 4) {
        encoded_index -= (4 - chunk_len);  
        break;
      }
    }
  }
  encoded[encoded_index] = '\0';
  return encoded;
}

unsigned char *utl_encoding_base85_decode(const char *input, unsigned int length) {
  if (input == NULL || length == 0) {
    utl_error_func("Invalid input or length", utl_user_defined_data);
    return NULL;
  }
  unsigned int decoded_max_length = (length / 5) * 4;
  unsigned char *decoded = malloc(decoded_max_length);
  if (!decoded) {
    utl_error_func("Memory allocation failed for decoded string", utl_user_defined_data);
    return NULL;
  }
  unsigned int input_index = 0;
  unsigned int decoded_index = 0;
  while (input_index < length) {
    if (isspace(input[input_index])) {
      input_index++;
      continue;
    }
    if (input[input_index] == 'z') {
      memset(decoded + decoded_index, 0, 4);
      decoded_index += 4;
      input_index++;
      continue;
    }
    unsigned int acc = 0;
    int count = 0;
    for (int i = 0; i < 5 && input_index < length; ++i) {
      if (isspace(input[input_index])) {
        input_index++;
        continue;
      }
      char ch = input[input_index++];
      if (ch < 33 || ch > 117) {
        utl_error_func("Invalid character encountered", utl_user_defined_data);
        free(decoded);
        return NULL; 
      }
      acc = acc * 85 + (ch - 33);
      count++;
    }
    int padding = 0;
    if (count < 5) {
      padding = 5 - count;
      for (int i = 0; i < padding; i++) {
        acc = acc * 85 + 84;
      }
    }
    for (int i = 3; i >= 0; --i) {
      if (i < padding) {
        break;
      }
      decoded[decoded_index++] = (acc >> (i * 8)) & 0xFF;
    }
    if (count < 5) {
        break;
    }
  }
  unsigned char *resized_decoded = realloc(decoded, decoded_index + 1);
  if (!resized_decoded) {
    utl_error_func("Reallocation failed", utl_user_defined_data);
    free(decoded);
    return NULL;
  }
  resized_decoded[decoded_index] = '\0'; 
  return resized_decoded;
}

char *utl_encoding_base58_encode(const void *data, unsigned int binsz) {
  if (!data) {
    utl_error_func("Invalid input data", utl_user_defined_data);
    return NULL;
  }
  const unsigned char *bin = data;
  int carry;
  unsigned int i, j, high, zcount = 0;
  unsigned int size;
  while (zcount < binsz && !bin[zcount]) {
    ++zcount;
  }
  size = (binsz - zcount) * 138 / 100 + 1;
  unsigned char *buf = malloc(size * sizeof(unsigned char));
  if (!buf) {
    utl_error_func("Memory allocation failed for buffer", utl_user_defined_data);
    return NULL;
  }
  memset(buf, 0, size);
  for (i = zcount, high = size - 1; i < binsz; ++i, high = j) {
    for (carry = bin[i], j = size - 1; (j > high) || carry; --j) {
      carry += 256 * buf[j];
      buf[j] = carry % 58;
      carry /= 58;
      if (!j) {
        break;
      }
    }
  }
  for (j = 0; j < size && !buf[j]; ++j) {}
  unsigned int b58sz = zcount + size - j + 1;
  char *b58 = malloc(b58sz);
  if (!b58) {
    utl_error_func("Memory allocation failed for Base58 encoded result", utl_user_defined_data);
    free(buf);
    return NULL;
  }
  if (zcount) {
    memset(b58, '1', zcount);
  }
  for (i = zcount; j < size; ++i, ++j) {
    b58[i] = b58digits_ordered[buf[j]];
  }
  b58[i] = '\0';
  free(buf);
  return b58;
}

char *utl_encoding_base58_decode(const char *b58, unsigned int *binszp) {
  if (b58 == NULL || binszp == NULL) {
    utl_error_func("Invalid input or binszp pointer", utl_user_defined_data);
    return NULL;
  }
  unsigned int b58sz = strlen(b58);
  unsigned int binsz = b58sz * 733 / 1000 + 1;
  unsigned char *bin = malloc(binsz);
  if (!bin) {
    utl_error_func("Memory allocation failed", utl_user_defined_data);
    return NULL;
  }
  memset(bin, 0, binsz);
  unsigned int i, j;
  int carry;
  unsigned int high = binsz - 1;
  for (i = 0; i < b58sz; ++i) {
    if (b58[i] & 0x80 || b58digits_map[(unsigned char)b58[i]] == -1) {
      utl_error_func("Invalid Base58 character encountered", utl_user_defined_data);
      free(bin);
      return NULL;
    }
    for (carry = b58digits_map[(unsigned char)b58[i]], j = binsz - 1; (j > high) || carry; --j) {
      carry += 58 * bin[j];
      bin[j] = carry % 256;
      carry /= 256;
      if (!j) {
        break;
      }
    }
    high = j;
  }
  for (j = 0; j < binsz && !bin[j]; ++j) {}
  *binszp = binsz - j;
  char *result = malloc(*binszp);
  if (!result) {
    utl_error_func("Memory allocation failed for result", utl_user_defined_data);
    free(bin);
    return NULL;
  }
  memcpy(result, bin + j, *binszp);
  free(bin);
  return result;
}

unsigned char *utl_encoding_base91_decode(const char *encoded, unsigned int *decoded_length) {
  if (!encoded || !decoded_length) {
    utl_error_func("Invalid input or decoded_length pointer", utl_user_defined_data);
    return NULL;
  }
  unsigned int len = strlen(encoded);
  *decoded_length = 0;
  unsigned char *decoded = malloc(len);
  if (!decoded) {
    utl_error_func("Memory allocation failed", utl_user_defined_data);
    return NULL;
  }
  int v = -1;
  int b = 0;
  int n = 0;
  unsigned int index = 0;
  for (unsigned int i = 0; i < len; ++i) {
    int c = base91_decode_value(encoded[i]);
    if (c == -1) {
      utl_error_func("Invalid character encountered", utl_user_defined_data);
      free(decoded);
      return NULL;
    }
    if (v < 0) {
      v = c;
    } 
    else {
      v += c * 91;
      b |= v << n;
      n += (v & 8191) > 88 ? 13 : 14;
      while (n > 7) {
        if (index >= len) {
          utl_error_func("Decoded index out of bounds", utl_user_defined_data);
          free(decoded);
          return NULL;
        }
        decoded[index++] = (unsigned char)(b & 255);
        b >>= 8;
        n -= 8;
      }
      v = -1;
    }
  }
  if (v != -1) {
    if (index >= len) {
      utl_error_func("Decoded index out of bounds", utl_user_defined_data);
      free(decoded);
      return NULL;
    }
    decoded[index++] = (unsigned char)((b | v << n) & 255);
  }
  *decoded_length = index;
  return decoded;
}

char *utl_encoding_base91_encode(const unsigned char *data, unsigned int length) {
  if (!data || length == 0) {
    utl_error_func("Invalid input data or length", utl_user_defined_data);
    return NULL;
  }
  unsigned int estimated_length = length * 1.23 + 2;
  char *encoded = malloc(estimated_length);
  if (!encoded) {
    utl_error_func("Memory allocation failed", utl_user_defined_data);
    return NULL;
  }
  unsigned int index = 0;
  int b = 0;
  int n = 0;
  int v;
  for (unsigned int i = 0; i < length; ++i) {
    b |= (data[i] << n);
    n += 8;
    if (n > 13) {
      v = b & 8191;
      if (v > 88) {
        b >>= 13;
        n -= 13;
      } 
      else {
        v = b & 16383;
        b >>= 14;
        n -= 14;
      }
      if (index + 2 < estimated_length) {
        encoded[index++] = BASE91_ALPHABET[v % 91];
        encoded[index++] = BASE91_ALPHABET[v / 91];
      } 
      else {
        utl_error_func("Encoding index out of bounds", utl_user_defined_data);
        free(encoded);
        return NULL;
      }
    }
  }
  if (n) {
    if (index + 1 < estimated_length) {
      encoded[index++] = BASE91_ALPHABET[b % 91];
    }
    if (n > 7 || b > 90) {
      if (index + 1 < estimated_length) {
        encoded[index++] = BASE91_ALPHABET[b / 91];
      }
    }
  }
  encoded[index] = '\0';
  return encoded;
}
#if defined(_WIN32) || defined(_WIN64)
wchar_t *utl_encoding_utf8_to_wchar(const char *utf8Str) {
  if (utf8Str == NULL) {
    utl_error_func("Input string is NULL", utl_user_defined_data);
    return NULL;
  }
  int size_needed = MultiByteToWideChar(CP_UTF8, 0, utf8Str, -1, NULL, 0);
  if (size_needed == 0) {
    return NULL;
  }
  wchar_t* wstr = (wchar_t*)malloc(size_needed * sizeof(wchar_t));
  if (!wstr) {
    utl_error_func("Cannot allocate memory for wchar string", utl_user_defined_data);
    return NULL;
  }
  int result = MultiByteToWideChar(CP_UTF8, 0, utf8Str, -1, wstr, size_needed);
  if (result == 0) {
    utl_error_func("Conversion from UTF-8 to wchar failed", utl_user_defined_data);
    free(wstr);
    return NULL;
  }
  return wstr;
}
#endif
char *utl_encoding_wchar_to_utf8(const wchar_t *wstr) {
  if (wstr == NULL) {
    utl_error_func("Input wchar string is NULL", utl_user_defined_data);
    return NULL;
  }
#if defined(_WIN32) || defined(_WIN64)
  int utf8Length = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
  if (utf8Length == 0) {
    utl_error_func("WideCharToMultiByte failed to calculate length", utl_user_defined_data);
    return NULL;
  }
  char *utf8Str = (char *)malloc(utf8Length * sizeof(char));
  if (!utf8Str) {
    utl_error_func("Cannot allocate memory for UTF-8 string", utl_user_defined_data);
    return NULL;
  }
  if (WideCharToMultiByte(CP_UTF8, 0, wstr, -1, utf8Str, utf8Length, NULL, NULL) == 0) {
    utl_error_func("Conversion from wchar to UTF-8 failed", utl_user_defined_data);
    free(utf8Str);
    return NULL;
  }
#else
  setlocale(LC_ALL, "en_US.UTF-8");
  unsigned int utf8Length = wcstombs(NULL, wstr, 0);
  if (utf8Length == (unsigned int)-1) {
    utl_error_func("Wcstombs failed", utl_user_defined_data);
    return NULL;
  }
  char *utf8Str = (char *)malloc((utf8Length + 1) * sizeof(char));
  if (!utf8Str) {
    utl_error_func("Cannot allocate memory for UTF-8 string", utl_user_defined_data);
    return NULL;
  }
  if (wcstombs(utf8Str, wstr, utf8Length + 1) == (unsigned int)-1) {
    utl_error_func("Conversion from wchar to UTF-8 failed", utl_user_defined_data);
    free(utf8Str);
    return NULL;
  }
#endif
  return utf8Str;
}

void utl_encoding_initialize(void) {
  setlocale(LC_ALL, "");
}

