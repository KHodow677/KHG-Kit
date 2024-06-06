#pragma once

#include <stdlib.h>

typedef void* vector;
typedef size_t vec_size_t;
typedef size_t vec_type_t;

#define vectorAddDST(vec_addr, type)((type*)(addDST((vector*)vec_addr, sizeof(type))))
#define vectorInsertDST(vec_addr, type, pos)((type*)(insertDST((vector*)vec_addr, sizeof(type), pos)))
#define vectorAdd(vec_addr, type, value)(*vectorAddDST(vec_addr, type) = value)
#define vectorInsert(vec_addr, type, pos, value)(*vectorInsertDST(vec_addr, type, pos) = value)
#define vectorErase(vec, type, pos, len)(erase((vector)vec, sizeof(type), pos, len))
#define vectorRemove(vec, type, pos)(removeAt((vector)vec, sizeof(type), pos))
#define vectorReserve(vec_addr, type, capacity)(reserve((vector*)vec_addr, sizeof(type), capacity))
#define vectorCopy(vec, type)(copy((vector)vec, sizeof(type)))


vector vectorCreate(void);
void vectorFree(vector vec);
void vectorPop(vector vec);
vec_size_t vectorSize(vector vec);
vec_size_t vectorCapacity(vector vec);
void vectorClear(vector vec);
void *addDST(vector* vec_addr, vec_type_t type_size);
void *insertDST(vector* vec_addr, vec_type_t type_size, vec_size_t pos);
void erase(vector vec_addr, vec_type_t type_size, vec_size_t pos, vec_size_t len);
void removeAt(vector vec_addr, vec_type_t type_size, vec_size_t pos);
void reserve(vector* vec_addr, vec_type_t type_size, vec_size_t capacity);
vector copy(vector vec, vec_type_t type_size);
