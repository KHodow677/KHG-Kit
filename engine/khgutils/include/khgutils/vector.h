#pragma once

#include <assert.h>
#include <stdlib.h>

typedef void (*vector_elem_destructor_t)(void *elem_ptr);

typedef struct {
    size_t size;
    size_t capacity;
    vector_elem_destructor_t elem_destructor;
} vector_metadata_t;

#define vector_type(type) type *
#define vector(type) vector_type(type)
#define vector_iterator(type) vector_type(type)

#define vector_vec_to_base(vec) (&((vector_metadata_t *)(vec))[-1])
#define vector_base_to_vec(ptr) ((void *)&((vector_metadata_t *)(ptr))[1])
#define vector_capacity(vec) ((vec) ? vector_vec_to_base(vec)->capacity : (size_t)0)
#define vector_size(vec) ((vec) ? vector_vec_to_base(vec)->size : (size_t)0)
#define vector_elem_destructor(vec) ((vec) ? vector_vec_to_base(vec)->elem_destructor : NULL)

#define vector_empty(vec) (vector_size(vec) == 0)

#define vector_reserve(vec, n) {\
  size_t cv_cap = vector_capacity(vec);\
  if (cv_cap < (n)) {\
    vector_grow((vec), (n));\
  }\
}

#define vector_init(vec, capacity, elem_destructor_fn) {\
  if (!(vec)) {\
    vector_reserve((vec), capacity);\
    vector_set_elem_destructor((vec), (elem_destructor_fn));\
  }\
}

#define vector_erase(vec, i) {\
  if (vec) {\
    const size_t cv_sz = vector_size(vec);\
    if ((i) < cv_sz) {\
      vector_elem_destructor_t elem_destructor = vector_elem_destructor(vec);\
      if (elem_destructor) {\
        elem_destructor(&(vec)[i]);\
      }\
      vector_set_size((vec), cv_sz - 1);\
      memmove((vec) + (i), (vec) + (i) + 1, sizeof(*(vec)) * (cv_sz - 1 - (i)));\
    }\
  }\
}

#define vector_clear(vec) {\
  if (vec) {\
    vector_elem_destructor_t elem_destructor = vector_elem_destructor(vec);\
    if (elem_destructor) {\
      size_t i;\
      for (i = 0; i < vector_size(vec); ++i) {\
        elem_destructor(&(vec)[i]);\
      }\
    }\
  vector_set_size(vec, 0);\
  }\
}

#define vector_free(vec) {\
  if (vec) {\
    void *p1 = vector_vec_to_base(vec);\
    vector_elem_destructor_t elem_destructor = vector_elem_destructor(vec); \
    if (elem_destructor) {\
      size_t i;\
      for (i = 0; i < vector_size(vec); ++i) {\
        elem_destructor(&(vec)[i]);\
      }\
    }\
    free(p1);\
  }\
}

#define vector_begin(vec) (vec)
#define vector_end(vec) ((vec) ? &((vec)[vector_size(vec)]) : NULL)

#define vector_compute_next_grow(size) ((size) + 1)

#define vector_push_back(vec, value) {\
  size_t cv_cap = vector_capacity(vec);\
  if (cv_cap <= vector_size(vec)) {\
    vector_grow((vec), vector_compute_next_grow(cv_cap));\
  }\
  (vec)[vector_size(vec)] = (value);\
  vector_set_size((vec), vector_size(vec) + 1);\
}

#define vector_insert(vec, pos, val) {\
  size_t cv_cap = vector_capacity(vec);\
  if (cv_cap <= vector_size(vec)) {\
    vector_grow((vec), vector_compute_next_grow(cv_cap));\
  }\
  if ((pos) < vector_size(vec)) {\
    memmove((vec) + (pos) + 1, (vec) + (pos), sizeof(*(vec)) * ((vector_size(vec)) - (pos)));\
  }\
  (vec)[(pos)] = (val);\
  vector_set_size((vec), vector_size(vec) + 1);\
}

#define vector_pop_back(vec) {\
  vector_elem_destructor_t elem_destructor = vector_elem_destructor(vec);\
  if (elem_destructor) {\
    elem_destructor(&(vec)[vector_size(vec) - 1]);\
  }\
  vector_set_size((vec), vector_size(vec) - 1);\
}

#define vector_copy(from, to) {\
  if ((from)) {\
    vector_grow(to, vector_size(from));\
    vector_set_size(to, vector_size(from));\
    memcpy((to), (from), vector_size(from) * sizeof(*(from)));\
  }\
}

#define vector_swap(vec, other, type) {\
  if (vec && other) {\
    vector_vector_type(type) cv_swap = vec;\
    vec = other;\
    other = cv_swap;\
  }\
}

#define vector_set_capacity(vec, size) {\
  if (vec) {\
    vector_vec_to_base(vec)->capacity = (size);\
  }\
}

#define vector_set_size(vec, _size) {\
  if (vec) {\
    vector_vec_to_base(vec)->size = (_size);\
  }\
}

#define vector_set_elem_destructor(vec, elem_destructor_fn) {\
  if (vec) {\
    vector_vec_to_base(vec)->elem_destructor = (elem_destructor_fn);\
  }\
}

#define vector_grow(vec, count) {\
  const size_t cv_sz__ = (count) * sizeof(*(vec)) + sizeof(vector_metadata_t);\
  if (vec) {\
    void *cv_p1__ = vector_vec_to_base(vec);\
    void *cv_p2__ = realloc(cv_p1__, cv_sz__);\
    assert(cv_p2__);\
    (vec) = vector_base_to_vec(cv_p2__);\
  }\
  else {\
    void *cv_p__ = malloc(cv_sz__);\
    assert(cv_p__);\
    (vec) = vector_base_to_vec(cv_p__);\
    vector_set_size((vec), 0);\
    vector_set_elem_destructor((vec), NULL);\
  }\
  vector_set_capacity((vec), (count));\
}

#define vector_shrink_to_fit(vec) {\
  if (vec) {\
    const size_t cv_sz___ = vector_size(vec);\
    vector_grow(vec, cv_sz___);\
  }\
}

#define vector_at(vec, n) ((vec) ? (((int)(n) < 0 || (size_t)(n) >= vector_size(vec)) ? NULL : &(vec)[n]) : NULL)
#define vector_front(vec) ((vec) ? ((vector_size(vec) > 0) ? vector_at(vec, 0) : NULL) : NULL)
#define vector_back(vec) ((vec) ? ((vector_size(vec) > 0) ? vector_at(vec, vector_size(vec) - 1) : NULL) : NULL)

#define vector_resize(vec, count, value) {\
  if (vec) {\
    size_t cv_sz_count__ = (size_t)(count);\
    size_t cv_sz__       = vector_vec_to_base(vec)->size;\
    if (cv_sz_count__ > cv_sz__) {\
      vector_reserve((vec), cv_sz_count__);\
      vector_set_size((vec), cv_sz_count__);\
      do {\
        (vec)[cv_sz__++] = (value);\
      }\
      while (cv_sz__ < cv_sz_count__);\
    }\
    else {\
      while (cv_sz_count__ < cv_sz__--) {\
        vector_pop_back(vec);\
      }\
    }\
  }\
}
