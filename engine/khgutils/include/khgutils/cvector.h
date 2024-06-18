#pragma once

#include <stdlib.h>
#include <assert.h>

typedef void (*cvector_elem_destructor_t)(void *elem_ptr);

typedef struct {
    size_t size;
    size_t capacity;
    cvector_elem_destructor_t elem_destructor;
} cvector_metadata_t;

#define cvector_vector_type(type) type *
#define cvector(type) cvector_vector_type(type)
#define cvector_iterator(type) cvector_vector_type(type)

#define cvector_vec_to_base(vec) (&((cvector_metadata_t *)(vec))[-1])
#define cvector_base_to_vec(ptr) ((void *)&((cvector_metadata_t *)(ptr))[1])
#define cvector_capacity(vec) ((vec) ? cvector_vec_to_base(vec)->capacity : (size_t)0)
#define cvector_size(vec) ((vec) ? cvector_vec_to_base(vec)->size : (size_t)0)
#define cvector_elem_destructor(vec) ((vec) ? cvector_vec_to_base(vec)->elem_destructor : NULL)

#define cvector_empty(vec) (cvector_size(vec) == 0)

#define cvector_reserve(vec, n) {\
  size_t cv_cap = cvector_capacity(vec);\
  if (cv_cap < (n)) {\
    cvector_grow((vec), (n));\
  }\
}

#define cvector_init(vec, capacity, elem_destructor_fn) {\
  if (!(vec)) {\
    cvector_reserve((vec), capacity);\
    cvector_set_elem_destructor((vec), (elem_destructor_fn));\
  }\
}

#define cvector_erase(vec, i) {\
  if (vec) {\
    const size_t cv_sz = cvector_size(vec);\
    if ((i) < cv_sz) {\
      cvector_elem_destructor_t elem_destructor = cvector_elem_destructor(vec);\
      if (elem_destructor) {\
        elem_destructor(&(vec)[i]);\
      }\
      cvector_set_size((vec), cv_sz - 1);\
      memmove((vec) + (i), (vec) + (i) + 1, sizeof(*(vec)) * (cv_sz - 1 - (i)));\
    }\
  }\
}

#define cvector_clear(vec) {\
  if (vec) {\
    cvector_elem_destructor_t elem_destructor = cvector_elem_destructor(vec);\
    if (elem_destructor) {\
      size_t i;\
      for (i = 0; i < cvector_size(vec); ++i) {\
        elem_destructor(&(vec)[i]);\
      }\
    }\
  cvector_set_size(vec, 0);\
  }\
}

#define cvector_free(vec) {\
  if (vec) {\
    void *p1 = cvector_vec_to_base(vec);\
    cvector_elem_destructor_t elem_destructor = cvector_elem_destructor(vec); \
    if (elem_destructor) {\
      size_t i;\
      for (i = 0; i < cvector_size(vec); ++i) {\
        elem_destructor(&(vec)[i]);\
      }\
    }\
    free(p1);\
  }\
}

#define cvector_begin(vec) (vec)
#define cvector_end(vec) ((vec) ? &((vec)[cvector_size(vec)]) : NULL)

#define cvector_compute_next_grow(size) ((size) + 1)

#define cvector_push_back(vec, value) {\
  size_t cv_cap = cvector_capacity(vec);\
  if (cv_cap <= cvector_size(vec)) {\
    cvector_grow((vec), cvector_compute_next_grow(cv_cap));\
  }\
  (vec)[cvector_size(vec)] = (value);\
  cvector_set_size((vec), cvector_size(vec) + 1);\
}

#define cvector_insert(vec, pos, val) {\
  size_t cv_cap = cvector_capacity(vec);\
  if (cv_cap <= cvector_size(vec)) {\
    cvector_grow((vec), cvector_compute_next_grow(cv_cap));\
  }\
  if ((pos) < cvector_size(vec)) {\
    memmove((vec) + (pos) + 1, (vec) + (pos), sizeof(*(vec)) * ((cvector_size(vec)) - (pos)));\
  }\
  (vec)[(pos)] = (val);\
  cvector_set_size((vec), cvector_size(vec) + 1);\
}

#define cvector_pop_back(vec) {\
  cvector_elem_destructor_t elem_destructor = cvector_elem_destructor(vec);\
  if (elem_destructor) {\
    elem_destructor(&(vec)[cvector_size(vec) - 1]);\
  }\
  cvector_set_size((vec), cvector_size(vec) - 1);\
}

#define cvector_copy(from, to) {\
  if ((from)) {\
    cvector_grow(to, cvector_size(from));\
    cvector_set_size(to, cvector_size(from));\
    memcpy((to), (from), cvector_size(from) * sizeof(*(from)));\
  }\
}

#define cvector_swap(vec, other, type) {\
  if (vec && other) {\
    cvector_vector_type(type) cv_swap = vec;\
    vec = other;\
    other = cv_swap;\
  }\
}

#define cvector_set_capacity(vec, size) {\
  if (vec) {\
    cvector_vec_to_base(vec)->capacity = (size);\
  }\
}

#define cvector_set_size(vec, _size) {\
  if (vec) {\
    cvector_vec_to_base(vec)->size = (_size);\
  }\
}

#define cvector_set_elem_destructor(vec, elem_destructor_fn) {\
  if (vec) {\
    cvector_vec_to_base(vec)->elem_destructor = (elem_destructor_fn);\
  }\
}

#define cvector_grow(vec, count) {\
  const size_t cv_sz__ = (count) * sizeof(*(vec)) + sizeof(cvector_metadata_t);\
  if (vec) {\
    void *cv_p1__ = cvector_vec_to_base(vec);\
    void *cv_p2__ = realloc(cv_p1__, cv_sz__);\
    assert(cv_p2__);\
    (vec) = cvector_base_to_vec(cv_p2__);\
  }\
  else {\
    void *cv_p__ = malloc(cv_sz__);\
    assert(cv_p__);\
    (vec) = cvector_base_to_vec(cv_p__);\
    cvector_set_size((vec), 0);\
    cvector_set_elem_destructor((vec), NULL);\
  }\
  cvector_set_capacity((vec), (count));\
}

#define cvector_shrink_to_fit(vec) {\
  if (vec) {\
    const size_t cv_sz___ = cvector_size(vec);\
    cvector_grow(vec, cv_sz___);\
  }\
}

#define cvector_at(vec, n) ((vec) ? (((int)(n) < 0 || (size_t)(n) >= cvector_size(vec)) ? NULL : &(vec)[n]) : NULL)
#define cvector_front(vec) ((vec) ? ((cvector_size(vec) > 0) ? cvector_at(vec, 0) : NULL) : NULL)
#define cvector_back(vec) ((vec) ? ((cvector_size(vec) > 0) ? cvector_at(vec, cvector_size(vec) - 1) : NULL) : NULL)

#define cvector_resize(vec, count, value) {\
  if (vec) {\
    size_t cv_sz_count__ = (size_t)(count);\
    size_t cv_sz__       = cvector_vec_to_base(vec)->size;\
    if (cv_sz_count__ > cv_sz__) {\
      cvector_reserve((vec), cv_sz_count__);\
      cvector_set_size((vec), cv_sz_count__);\
      do {\
        (vec)[cv_sz__++] = (value);\
      }\
      while (cv_sz__ < cv_sz_count__);\
    }\
    else {\
      while (cv_sz_count__ < cv_sz__--) {\
        cvector_pop_back(vec);\
      }\
    }\
  }\
}
