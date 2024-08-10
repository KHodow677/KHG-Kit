#include "khg_aud/audio_buffer.h"
#include "khg_aud/device_management.h"
#include "khg_aud/callbacks.h"
#include "khg_aud/utils.h"
#include "khg_utl/error_func.h"
#include "miniaudio/miniaudio.h"
#include <string.h>

void aud_init_audio_device(void) {
  ma_context_config ctx_config = ma_context_config_init();
  ctx_config.logCallback = aud_on_log;
  ma_result result = ma_context_init(NULL, 0, &ctx_config, &audio.system.context);
  if (result != MA_SUCCESS) {
    utl_error_func("Failed to initialize context", utl_user_defined_data);
    return;
  }
  ma_device_config config = ma_device_config_init(ma_device_type_playback);
  config.playback.pDeviceID = NULL;
  config.playback.format = AUD_AUDIO_DEVICE_FORMAT;
  config.playback.channels = AUD_AUDIO_DEVICE_CHANNELS;
  config.capture.pDeviceID = NULL;
  config.capture.format = ma_format_s16;
  config.capture.channels = 1;
  config.sampleRate = AUD_AUDIO_DEVICE_SAMPLE_RATE;
  config.dataCallback = aud_on_send_audio_data_to_device;
  config.pUserData = NULL;
  result = ma_device_init(&audio.system.context, &config, &audio.system.device);
  if (result != MA_SUCCESS) {
    utl_error_func("Failed to initialize playback device", utl_user_defined_data);
    ma_context_uninit(&audio.system.context);
    return;
  }
  result = ma_device_start(&audio.system.device);
  if (result != MA_SUCCESS) {
    utl_error_func("Failed to start playback device", utl_user_defined_data);
    ma_device_uninit(&audio.system.device);
    ma_context_uninit(&audio.system.context);
    return;
  }
  aud_init_audio_buffer_pool();
  audio.system.is_ready = true;
}

void aud_close_audio_device(void) {
  if (audio.system.is_ready) {
    ma_mutex_uninit(&audio.system.lock);
    ma_device_uninit(&audio.system.device);
    ma_context_uninit(&audio.system.context);
    aud_close_audio_buffer_pool();
  }
  else {
    utl_error_func("Device could not be closed, not currently initialized", utl_user_defined_data);
  }
}

bool aud_is_audio_device_ready(void) {
  return audio.system.is_ready;
}

void aud_set_master_volume(float volume) {
  ma_device_set_master_volume(&audio.system.device, volume);
}
