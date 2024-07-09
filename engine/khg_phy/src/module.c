#include "khg_phy/module.h"
#include "khg_phy/module_internal.h"

KHGPHYDEF void init_physics(void) {
  pthread_create(&physics_thread_id, NULL, &physics_loop, NULL);
  init_timer();
  accumulator = 0.0f;
}

KHGPHYDEF void run_physics_step(void) {
  current_time = get_current_time();
  const double delta = current_time - start_time;
  accumulator += delta;
  while (accumulator >= delta) {
    physics_step();
    accumulator -= delta_time;
  }
  start_time = current_time;
}

KHGPHYDEF void set_physics_time_step(double delta) {
  delta_time = delta;
}

KHGPHYDEF bool is_physics_enabled(void) {
  return physics_thread_enabled;
}