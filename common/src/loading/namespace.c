#define NAMESPACE_LOADING_IMPL 

#include "loading/namespace.h"
#include "loading/resource_loading.h"
#include "loading/resources/ovr_tile_loader.h"
#include "loading/resources/texture_loader.h"

loading_namespace NAMESPACE_LOADING_INTERNAL = {
  .load_thread_defer = load_thread_defer,
  .load_resources_defer = load_resources_defer,
  .get_ovr_tile_id_from_string = get_ovr_tile_id_from_string,
  .get_ovr_tile = get_ovr_tile,
  .get_ovr_tile_from_string = get_ovr_tile_from_string,
  .generate_ovr_tiles = generate_ovr_tiles,
  .load_ovr_tile_tick = load_ovr_tile_tick,
  .get_tex_id_from_string = get_tex_id_from_string,
  .get_texture = get_texture,
  .get_texture_from_string = get_texture_from_string,
  .generate_textures = generate_textures,
  .load_texture_raw_tick = load_texture_raw_tick,
  .load_texture_tick = load_texture_tick,
  .RESOURCES_LOADED = false,
  .OVR_TILE_THREAD = { .enabled = true, .max = NUM_OVR_TILES },
  .TEXTURE_RAW_THREAD = { .enabled = true, .max = NUM_TEXTURES },
  .TEXTURE_THREAD = { .enabled = false, .max = NUM_TEXTURES }
};

loading_namespace *NAMESPACE_LOADING(void) {
  return &NAMESPACE_LOADING_INTERNAL;
}

