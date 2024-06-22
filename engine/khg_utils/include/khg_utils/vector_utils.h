#pragma once

#define vector_for_each_in(it, vec) for (it = vector_begin(vec); it < vector_end(vec); it++)

#define vector_for_each(vec, func) {\
  if ((vec) && (func) != NULL) {\
      size_t i;\
      for (i = 0; i < vector_size(vec); i++) {\
        func((vec)[i]);\
      }\
  }\
}

