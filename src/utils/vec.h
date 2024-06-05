#ifndef vec_h
#define vec_h

#include <stdlib.h>
#include "bool.h"

typedef void* vector;
typedef size_t vec_size_t;
typedef size_t vec_type_t;

#if _MSC_VER == 0 || __STDC_VERSION__ >= 202311L || defined __cpp_decltype
#define vectorAddDST(vec_addr)((typeof(*vec_addr))(addDST((vector*)vec_addr, sizeof(**vec_addr))))
#define vectorInsertDST(vec_addr, pos)((typeof(*vec_addr))(insertDST((vector*)vec_addr, sizeof(**vec_addr), pos)))
#define vectorAdd(vec_addr, value)(*vector_add_dst(vec_addr) = value)
#define vectorInsert(vec_addr, pos, value)(*vector_insert_dst(vec_addr, pos) = value)
#else
#define vectorAddDST(vec_addr, type)((type*)_vector_add_dst((vector*)vec_addr, sizeof(type)))
#define vectorInsertDST(vec_addr, type, pos)((type*)_vector_insert_dst((vector*)vec_addr, sizeof(type), pos))
#define vectorAdd(vec_addr, type, value)(*vector_add_dst(vec_addr, type) = value)
#define vectorInsert(vec_addr, type, pos, value)(*vector_insert_dst(vec_addr, type, pos) = value)
#endif

#define vectorErase(vec, pos, len)(erase((vector)vec, sizeof(*vec), pos, len))
#define vectorRemove(vec, pos)(remove((vector)vec, sizeof(*vec), pos))
#define vectorReserve(vec_addr, capacity)(reserve((vector*)vec_addr, sizeof(**vec_addr), capacity))
#define vectorCopy(vec)(copy((vector)vec, sizeof(*vec)))

vector vectorCreate(void);
void vectorFree(vector vec);
void vectorPop(vector vec);
vec_size_t vectorSize(vector vec);
vec_size_t vectorCapacity(vector vec);
void *addDST(vector* vec_addr, vec_type_t type_size);
void *insertDST(vector* vec_addr, vec_type_t type_size, vec_size_t pos);
void erase(vector vec_addr, vec_type_t type_size, vec_size_t pos, vec_size_t len);
void remove(vector vec_addr, vec_type_t type_size, vec_size_t pos);
void reserve(vector* vec_addr, vec_type_t type_size, vec_size_t capacity);
vector copy(vector vec, vec_type_t type_size);

#endif
