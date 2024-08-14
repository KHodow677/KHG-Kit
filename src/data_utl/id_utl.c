#include "data_utl/id_utl.h"

int generate_unique_id() {
  static int id = 0;
  return ++id;
}

