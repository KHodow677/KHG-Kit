#define NAMESPACE_TASKING_IMPL

#include "tasking/namespace.h"
#include "tasking/resource_loading.h"
#include "tasking/resources/ovr_tile_loader.h"
#include "tasking/resources/texture_loader.h"
#include "tasking/task_management.h"
#include "tasking/texture_loader.h"
#include "tasking/tile_loader.h"

tasking_namespace NAMESPACE_TASKING_INTERNAL = {
  .load_thread_defer = load_thread_defer,
  .load_resources_defer = load_resources_defer,
  .get_ovr_tile_id_from_string = get_ovr_tile_id_from_string,
  .get_ovr_tile = get_ovr_tile,
  .get_ovr_tile_from_string = get_ovr_tile_from_string,
  .generate_ovr_tiles = generate_ovr_tiles,
  .load_ovr_tile_tick = load_ovr_tile_tick,
  .generate_tex_defs = generate_tex_defs,
  .emplace_tex_defs_tick = emplace_tex_defs_tick,
  .emplace_tex_defs = emplace_tex_defs,
  .free_tex_defs = free_tex_defs,
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
  .clear_tile_data = clear_tile_data,
  .RESOURCES_LOADED = false,
  .OVR_TILE_THREAD = { .enabled = true, .max = NUM_OVR_TILES, .loaded = false, .loading_started = false, .progress = 0 },
  .TEXTURE_ASSET_THREAD = { .enabled = true, .max = 0, .loaded = false, .loading_started = false, .progress = 0 },
  .TEXTURE_RAW_THREAD = { .enabled = true, .max = 0, .loaded = false, .loading_started = false, .progress = 0 },
  .TEXTURE_THREAD = { .enabled = false, .max = 0, .loaded = false, .loading_started = false, .progress = 0 }
};

tasking_namespace *NAMESPACE_TASKING(void) {
  return &NAMESPACE_TASKING_INTERNAL;
}

