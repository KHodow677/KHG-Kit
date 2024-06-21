#include "khgaudio/audio_buffer.h"
#include "khgaudio/device_management.h"
#include "khgaudio/callbacks.h"
#include "khgaudio/utils.h"
#include "miniaudio/miniaudio.h"
#include <string.h>

void init_audio_device(void) {
  ma_context_config ctx_config = ma_context_config_init();
  ctx_config.logCallback = on_log;
  ma_result result = ma_context_init(NULL, 0, &ctx_config, &AUDIO.system.context);
  if (result != MA_SUCCESS) {
    TRACELOG(LOG_ERROR, "AUDIO: Failed to initialize context");
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
  result = ma_device_init(&AUDIO.system.context, &config, &AUDIO.system.device);
  if (result != MA_SUCCESS) {
    TRACELOG(LOG_ERROR, "AUDIO: Failed to initialize playback device");
    ma_context_uninit(&AUDIO.system.context);
    return;
  }
  result = ma_device_start(&AUDIO.system.device);
  if (result != MA_SUCCESS) {
    TRACELOG(LOG_ERROR, "AUDIO: Failed to start playback device");
    ma_device_uninit(&AUDIO.system.device);
    ma_context_uninit(&AUDIO.system.context);
    return;
  }
  TRACELOG(LOG_INFO, "AUDIO: Device initialized successfully");
  TRACELOG(LOG_INFO, "    > Backend:       miniaudio / %s", ma_get_backend_name(AUDIO.System.context.backend));
  TRACELOG(LOG_INFO, "    > Format:        %s -> %s", ma_get_format_name(AUDIO.System.device.playback.format), ma_get_format_name(AUDIO.System.device.playback.internalFormat));
  TRACELOG(LOG_INFO, "    > Channels:      %d -> %d", AUDIO.System.device.playback.channels, AUDIO.System.device.playback.internalChannels);
  TRACELOG(LOG_INFO, "    > Sample rate:   %d -> %d", AUDIO.System.device.sampleRate, AUDIO.System.device.playback.internalSampleRate);
  TRACELOG(LOG_INFO, "    > Periods size:  %d", AUDIO.System.device.playback.internalPeriodSizeInFrames*AUDIO.System.device.playback.internalPeriods);
  init_audio_buffer_pool();
  AUDIO.system.is_ready = true;
}

void close_audio_device(void) {
  if (AUDIO.system.is_ready) {
    ma_mutex_uninit(&AUDIO.system.lock);
    ma_device_uninit(&AUDIO.system.device);
    ma_context_uninit(&AUDIO.system.context);
    close_audio_buffer_pool();
    TRACELOG(LOG_INFO, "AUDIO: Device closed successfully");
  }
  else TRACELOG(LOG_WARNING, "AUDIO: Device could not be closed, not currently initialized");
}

bool is_audio_device_ready(void) {
  return AUDIO.system.is_ready;
}

void set_master_volume(float volume) {
  ma_device_set_master_volume(&AUDIO.system.device, volume);
}
