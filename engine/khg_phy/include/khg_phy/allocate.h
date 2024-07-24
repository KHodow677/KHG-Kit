#pragma once

#include <stdint.h>

void *b2Alloc(uint32_t size);
void b2Free(void *mem, uint32_t size);
