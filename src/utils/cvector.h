#ifndef CVECTOR_H_
#define CVECTOR_H_

#ifndef cvector_clib_free
#include <stdlib.h> /* for free */
#define cvector_clib_free free
#endif
#ifndef cvector_clib_malloc
#include <stdlib.h> /* for malloc */
#define cvector_clib_malloc malloc
#endif
#ifndef cvector_clib_calloc
#include <stdlib.h> /* for calloc */
#define cvector_clib_calloc calloc
#endif
#ifndef cvector_clib_realloc
#include <stdlib.h> /* for realloc */
#define cvector_clib_realloc realloc
#endif
#ifndef cvector_clib_assert
#include <assert.h> /* for assert */
#define cvector_clib_assert assert
#endif
#ifndef cvector_clib_memcpy
#include <string.h> /* for memcpy */
#define cvector_clib_memcpy memcpy
#endif
#ifndef cvector_clib_memmove
#include <string.h> /* for memmove */
#define cvector_clib_memmove memmove
#endif

typedef void (*cvector_elem_destructor_t)(void *elem_ptr);

typedef struct cvector_metadata_t {
    size_t size;
    size_t capacity;
    cvector_elem_destructor_t elem_destructor;
} cvector_metadata_t;

#define cvector_vector_type(type) type *

#define cvector(type) cvector_vector_type(type)

#define cvector_iterator(type) cvector_vector_type(type)

#define cvector_vec_to_base(vec) \
    (&((cvector_metadata_t *)(vec))[-1])

#define cvector_base_to_vec(ptr) \
    ((void *)&((cvector_metadata_t *)(ptr))[1])

#define cvector_capacity(vec) \
    ((vec) ? cvector_vec_to_base(vec)->capacity : (size_t)0)

#define cvector_size(vec) \
    ((vec) ? cvector_vec_to_base(vec)->size : (size_t)0)

#define cvector_elem_destructor(vec) \
    ((vec) ? cvector_vec_to_base(vec)->elem_destructor : NULL)

#define cvector_empty(vec) \
    (cvector_size(vec) == 0)

#define cvector_reserve(vec, n)                  \
    do {                                         \
        size_t cv_cap__ = cvector_capacity(vec); \
        if (cv_cap__ < (n)) {                    \
            cvector_grow((vec), (n));            \
        }                                        \
    } while (0)

#define cvector_init(vec, capacity, elem_destructor_fn)               \
    do {                                                              \
        if (!(vec)) {                                                 \
            cvector_reserve((vec), capacity);                         \
            cvector_set_elem_destructor((vec), (elem_destructor_fn)); \
        }                                                             \
    } while (0)

#define cvector_erase(vec, i)                                                               \
    do {                                                                                    \
        if (vec) {                                                                          \
            const size_t cv_sz__ = cvector_size(vec);                                       \
            if ((i) < cv_sz__) {                                                            \
                cvector_elem_destructor_t elem_destructor__ = cvector_elem_destructor(vec); \
                if (elem_destructor__) {                                                    \
                    elem_destructor__(&(vec)[i]);                                           \
                }                                                                           \
                cvector_set_size((vec), cv_sz__ - 1);                                       \
                cvector_clib_memmove(                                                       \
                    (vec) + (i),                                                            \
                    (vec) + (i) + 1,                                                        \
                    sizeof(*(vec)) * (cv_sz__ - 1 - (i)));                                  \
            }                                                                               \
        }                                                                                   \
    } while (0)

#define cvector_clear(vec)                                                              \
    do {                                                                                \
        if (vec) {                                                                      \
            cvector_elem_destructor_t elem_destructor__ = cvector_elem_destructor(vec); \
            if (elem_destructor__) {                                                    \
                size_t i__;                                                             \
                for (i__ = 0; i__ < cvector_size(vec); ++i__) {                         \
                    elem_destructor__(&(vec)[i__]);                                     \
                }                                                                       \
            }                                                                           \
            cvector_set_size(vec, 0);                                                   \
        }                                                                               \
    } while (0)

#define cvector_free(vec)                                                               \
    do {                                                                                \
        if (vec) {                                                                      \
            void *p1__                                  = cvector_vec_to_base(vec);     \
            cvector_elem_destructor_t elem_destructor__ = cvector_elem_destructor(vec); \
            if (elem_destructor__) {                                                    \
                size_t i__;                                                             \
                for (i__ = 0; i__ < cvector_size(vec); ++i__) {                         \
                    elem_destructor__(&(vec)[i__]);                                     \
                }                                                                       \
            }                                                                           \
            cvector_clib_free(p1__);                                                    \
        }                                                                               \
    } while (0)

#define cvector_begin(vec) \
    (vec)

#define cvector_end(vec) \
    ((vec) ? &((vec)[cvector_size(vec)]) : NULL)

#ifdef CVECTOR_LOGARITHMIC_GROWTH

#else

#define cvector_compute_next_grow(size) \
    ((size) + 1)

#endif 

#define cvector_push_back(vec, value)                                 \
    do {                                                              \
        size_t cv_cap__ = cvector_capacity(vec);                      \
        if (cv_cap__ <= cvector_size(vec)) {                          \
            cvector_grow((vec), cvector_compute_next_grow(cv_cap__)); \
        }                                                             \
        (vec)[cvector_size(vec)] = (value);                           \
        cvector_set_size((vec), cvector_size(vec) + 1);               \
    } while (0)

#define cvector_insert(vec, pos, val)                                 \
    do {                                                              \
        size_t cv_cap__ = cvector_capacity(vec);                      \
        if (cv_cap__ <= cvector_size(vec)) {                          \
            cvector_grow((vec), cvector_compute_next_grow(cv_cap__)); \
        }                                                             \
        if ((pos) < cvector_size(vec)) {                              \
            cvector_clib_memmove(                                     \
                (vec) + (pos) + 1,                                    \
                (vec) + (pos),                                        \
                sizeof(*(vec)) * ((cvector_size(vec)) - (pos)));      \
        }                                                             \
        (vec)[(pos)] = (val);                                         \
        cvector_set_size((vec), cvector_size(vec) + 1);               \
    } while (0)

#define cvector_pop_back(vec)                                                       \
    do {                                                                            \
        cvector_elem_destructor_t elem_destructor__ = cvector_elem_destructor(vec); \
        if (elem_destructor__) {                                                    \
            elem_destructor__(&(vec)[cvector_size(vec) - 1]);                       \
        }                                                                           \
        cvector_set_size((vec), cvector_size(vec) - 1);                             \
    } while (0)

#define cvector_copy(from, to)                                                       \
    do {                                                                             \
        if ((from)) {                                                                \
            cvector_grow(to, cvector_size(from));                                    \
            cvector_set_size(to, cvector_size(from));                                \
            cvector_clib_memcpy((to), (from), cvector_size(from) * sizeof(*(from))); \
        }                                                                            \
    } while (0)

#define cvector_swap(vec, other, type)                       \
    do {                                                     \
        if (vec && other) {                                  \
            cvector_vector_type(type) cv_swap__ = vec;       \
            vec                                 = other;     \
            other                               = cv_swap__; \
        }                                                    \
    } while (0)

#define cvector_set_capacity(vec, size)                  \
    do {                                                 \
        if (vec) {                                       \
            cvector_vec_to_base(vec)->capacity = (size); \
        }                                                \
    } while (0)

#define cvector_set_size(vec, _size)                  \
    do {                                              \
        if (vec) {                                    \
            cvector_vec_to_base(vec)->size = (_size); \
        }                                             \
    } while (0)

#define cvector_set_elem_destructor(vec, elem_destructor_fn)                  \
    do {                                                                      \
        if (vec) {                                                            \
            cvector_vec_to_base(vec)->elem_destructor = (elem_destructor_fn); \
        }                                                                     \
    } while (0)

#define cvector_grow(vec, count)                                                      \
    do {                                                                              \
        const size_t cv_sz__ = (count) * sizeof(*(vec)) + sizeof(cvector_metadata_t); \
        if (vec) {                                                                    \
            void *cv_p1__ = cvector_vec_to_base(vec);                                 \
            void *cv_p2__ = cvector_clib_realloc(cv_p1__, cv_sz__);                   \
            cvector_clib_assert(cv_p2__);                                             \
            (vec) = cvector_base_to_vec(cv_p2__);                                     \
        } else {                                                                      \
            void *cv_p__ = cvector_clib_malloc(cv_sz__);                              \
            cvector_clib_assert(cv_p__);                                              \
            (vec) = cvector_base_to_vec(cv_p__);                                      \
            cvector_set_size((vec), 0);                                               \
            cvector_set_elem_destructor((vec), NULL);                                 \
        }                                                                             \
        cvector_set_capacity((vec), (count));                                         \
    } while (0)

#define cvector_shrink_to_fit(vec)                     \
    do {                                               \
        if (vec) {                                     \
            const size_t cv_sz___ = cvector_size(vec); \
            cvector_grow(vec, cv_sz___);               \
        }                                              \
    } while (0)

#define cvector_at(vec, n) \
    ((vec) ? (((int)(n) < 0 || (size_t)(n) >= cvector_size(vec)) ? NULL : &(vec)[n]) : NULL)

#define cvector_front(vec) \
    ((vec) ? ((cvector_size(vec) > 0) ? cvector_at(vec, 0) : NULL) : NULL)

#define cvector_back(vec) \
    ((vec) ? ((cvector_size(vec) > 0) ? cvector_at(vec, cvector_size(vec) - 1) : NULL) : NULL)

#define cvector_resize(vec, count, value)                          \
    do {                                                           \
        if (vec) {                                                 \
            size_t cv_sz_count__ = (size_t)(count);                \
            size_t cv_sz__       = cvector_vec_to_base(vec)->size; \
            if (cv_sz_count__ > cv_sz__) {                         \
                cvector_reserve((vec), cv_sz_count__);             \
                cvector_set_size((vec), cv_sz_count__);            \
                do {                                               \
                    (vec)[cv_sz__++] = (value);                    \
                } while (cv_sz__ < cv_sz_count__);                 \
            } else {                                               \
                while (cv_sz_count__ < cv_sz__--) {                \
                    cvector_pop_back(vec);                         \
                }                                                  \
            }                                                      \
        }                                                          \
    } while (0)

#endif
