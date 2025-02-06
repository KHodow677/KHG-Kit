#define NAMESPACE_TASKING_IMPL

#include "tasking/namespace.h"
#include "tasking/task_management.h"
#include "tasking/texture_loader.h"
#include "tasking/tile_loader.h"

tasking_namespace NAMESPACE_TASKING_INTERNAL = {
  .task_enqueue = task_enqueue,
  .task_dequeue = task_dequeue,
  .task_worker = task_worker,
  .initialize_thread_pool = initialize_thread_pool,
  .shutdown_thread_pool = shutdown_thread_pool,
  .populate_texture_data = populate_texture_data,
  .load_texture_data = load_texture_data,
  .get_texture_data = get_texture_data,
  .get_texture_id = get_texture_id,
  .clear_texture_data = clear_texture_data,
  .populate_tile_data = populate_tile_data,
  .load_tile_data = load_tile_data,
  .get_tile_data = get_tile_data,
  .get_tile_id = get_tile_id,
  .get_random_tile_name = get_random_tile_name,
  .clear_tile_data = clear_tile_data,
};

tasking_namespace *NAMESPACE_TASKING(void) {
  return &NAMESPACE_TASKING_INTERNAL;
}

