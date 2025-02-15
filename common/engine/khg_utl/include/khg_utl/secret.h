#pragma once

#include <stddef.h>

int utl_secret_randbelow(int n);
int utl_secret_compare_digest(const unsigned char *a, const unsigned char *b, unsigned int length);

void utl_secret_token_bytes(unsigned char *buffer, unsigned int nbytes);
void utl_secret_token_hex(char *buffer, unsigned int nbytes);
void utl_secret_token_urlsafe(char *buffer, unsigned int nbytes);

void *utl_secret_choice(const void *seq, unsigned int size, unsigned int elem_size);
unsigned int utl_secret_randbits(int k);

