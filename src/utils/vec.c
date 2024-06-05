#include "vec.h"
#include <string.h>

typedef struct {
	vec_size_t size;
	vec_size_t capacity;
	unsigned char *data; 
} header;

header* getHeader(vector vec) { 
  return &((header*)vec)[-1];
}

header* reallocate(header* h, vec_type_t type_size) {
	vec_size_t new_capacity = (h->capacity == 0) ? 1 : h->capacity * 2;
	header* new_h = (header*)realloc(h, sizeof(header) + new_capacity * type_size);
	new_h->capacity = new_capacity;
	return new_h;
}

bool hasSpace(header* h) {
	return h->capacity - h->size > 0;
}

vector vectorCreate(void) {
	header* h = (header*)malloc(sizeof(header));
	h->capacity = 0;
	h->size = 0;
	return &h->data;
}

void vectorFree(vector vec) { 
  free(getHeader(vec)); 
}

void vectorPop(vector vec) { 
  --getHeader(vec)->size; 
}

vec_size_t vectorSize(vector vec) { 
  return getHeader(vec)->size; 
}

vec_size_t vectorCapacity(vector vec) { 
  return getHeader(vec)->capacity; 
}

void* addDST(vector* vec_addr, vec_type_t type_size) {
	header* h = getHeader(*vec_addr);
	if (!hasSpace(h)) {
		h = reallocate(h, type_size);
		*vec_addr = h->data;
	}
	return &h->data[type_size * h->size++];
}

void* insertDST(vector* vec_addr, vec_type_t type_size, vec_size_t pos) {
	header* h = getHeader(*vec_addr);
	vec_size_t new_length = h->size + 1;
	if (!hasSpace(h)) {
		h = reallocate(h, type_size);
		*vec_addr = h->data;
	}
	memmove(&h->data[(pos + 1) * type_size],
		&h->data[pos * type_size],
		(h->size - pos) * type_size);
	h->size = new_length;
	return &h->data[pos * type_size];
}

void erase(vector vec, vec_type_t type_size, vec_size_t pos, vec_size_t len) {
	header* h = getHeader(vec);
	memmove(&h->data[pos * type_size],
		&h->data[(pos + len) * type_size],
		(h->size - pos - len) * type_size);
	h->size -= len;
}

void remove(vector vec, vec_type_t type_size, vec_size_t pos) {
	erase(vec, type_size, pos, 1);
}

void reserve(vector* vec_addr, vec_type_t type_size, vec_size_t capacity) {
	header* h = getHeader(*vec_addr);
	if (h->capacity >= capacity) {
		return;
	}
	h = (header*)realloc(h, sizeof(header) + capacity * type_size);
	h->capacity = capacity;
	*vec_addr = &h->data;
}

vector copy(vector vec, vec_type_t type_size) {
	header* h = getHeader(vec);
	size_t alloc_size = sizeof(header) + h->size * type_size;
	header* copy_h = (header*)malloc(alloc_size);
	memcpy(copy_h, h, alloc_size);
	copy_h->capacity = copy_h->size;
	return &copy_h->data;
}
