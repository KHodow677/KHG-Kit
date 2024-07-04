#include "khg_audio/audio_buffer.h"
#include "khg_audio/device_management.h"
#include "khg_audio/callbacks.h"
#include "khg_audio/utils.h"
#include "khg_utils/error_func.h"
#include "miniaudio/miniaudio.h"
#include <string.h>

void init_audio_device(void) {
  ma_context_config ctx_config = ma_context_config_init();
  ctx_config.logCallback = on_log;
  ma_result result = ma_context_init(NULL, 0, &ctx_config, &audio.system.context);
  if (result != MA_SUCCESS) {
    error_func("Failed to initialize context", user_defined_data);
    return;
  }
  ma_device_config config = ma_device_config_init(ma_device_type_playback);
  config.playback.pDeviceID = NULL;  // NULL for the default playback AUDIO.System.device.
  config.playback.format = AUDIO_DEVICE_FORMAT;
  config.playback.channels = AUDIO_DEVICE_CHANNELS;
  config.capture.pDeviceID = NULL;  // NULL for the default capture AUDIO.System.device.
  config.capture.format = ma_format_s16;
  config.capture.channels = 1;
  config.sampleRate = AUDIO_DEVICE_SAMPLE_RATE;
  config.dataCallback = on_send_audio_data_to_device;
  config.pUserData = NULL;
  result = ma_device_init(&audio.system.context, &config, &audio.system.device);
  if (result != MA_SUCCESS) {
    error_func("Failed to initialize playback device", user_defined_data);
    ma_context_uninit(&audio.system.context);
    return;
  }
  result = ma_device_start(&audio.system.device);
  if (result != MA_SUCCESS) {
    error_func("Failed to start playback device", user_defined_data);
    ma_device_uninit(&audio.system.device);
    ma_context_uninit(&audio.system.context);
    return;
  }
  init_audio_buffer_pool();
  audio.system.is_ready = true;
}

void close_audio_device(void) {
  if (audio.system.is_ready) {
    ma_mutex_uninit(&audio.system.lock);
    ma_device_uninit(&audio.system.device);
    ma_context_uninit(&audio.system.context);
    close_audio_buffer_pool();
  }
  else {
    error_func("Device could not be closed, not currently initialized", user_defined_data);
  }
}

bool is_audio_device_ready(void) {
  return audio.system.is_ready;
}

void set_master_volume(float volume) {
  ma_device_set_master_volume(&audio.system.device, volume);
}