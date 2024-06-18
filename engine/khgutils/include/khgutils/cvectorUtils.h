#pragma once

#define cvector_for_each_in(it, vec) \
    for (it = cvector_begin(vec); it < cvector_end(vec); it++)

#define cvector_for_each(vec, func)                   \
    do {                                              \
        if ((vec) && (func) != NULL) {                \
            size_t i;                                 \
            for (i = 0; i < cvector_size(vec); i++) { \
                func((vec)[i]);                       \
            }                                         \
        }                                             \
    } while (0)

