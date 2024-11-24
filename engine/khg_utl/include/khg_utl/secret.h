#pragma once

#include <stddef.h>

int utl_secret_randbelow(int n);
int utl_secret_compare_digest(const unsigned char *a, const unsigned char *b, size_t length);

void utl_secret_token_bytes(unsigned char *buffer, size_t nbytes);
void utl_secret_token_hex(char *buffer, size_t nbytes);
void utl_secret_token_urlsafe(char *buffer, size_t nbytes);

void *utl_secret_choice(const void *seq, size_t size, size_t elem_size);
unsigned int utl_secret_randbits(int k);

