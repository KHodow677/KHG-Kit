#define NAMESPACE_TASKING_IMPL

#include "tasking/namespace.h"
#include "tasking/resource_loading.h"
#include "tasking/resources/ovr_tile_loader.h"
#include "tasking/resources/texture_loader.h"

tasking_namespace NAMESPACE_TASKING_INTERNAL = {
  .load_configs = load_configs,
  .close_config = close_configs,
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
  .get_tex_def_by_location = get_tex_def_by_location,
  .get_tex_def = get_tex_def,
  .get_location_tex_str = get_location_tex_str,
  .free_tex_defs = free_tex_defs,
  .RESOURCES_LOADED = false,
  .OVR_TILE_THREAD = { .enabled = true, .max = NUM_OVR_TILES, .loaded = false, .loading_started = false, .progress = 0 },
  .TEXTURE_ASSET_THREAD = { .enabled = true, .max = 0, .loaded = false, .loading_started = false, .progress = 0 },
  .TEXTURE_RAW_THREAD = { .enabled = true, .max = 0, .loaded = false, .loading_started = false, .progress = 0 },
  .TEXTURE_THREAD = { .enabled = false, .max = 0, .loaded = false, .loading_started = false, .progress = 0 }
};

tasking_namespace *NAMESPACE_TASKING(void) {
  return &NAMESPACE_TASKING_INTERNAL;
}

