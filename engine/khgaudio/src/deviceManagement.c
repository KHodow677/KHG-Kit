#include "khgaudio/deviceManagement.h"
#include "khgaudio/utils.h"
#include "miniaudio/miniaudio.h"

static void OnLog(ma_context *pContext, ma_device *pDevice, ma_uint32 logLevel, const char *message) {
  (void)pContext;
  (void)pDevice;
  TRACELOG(LOG_ERROR, "miniaudio: %s", message);
}

void initAudioDevice(void) {
  ma_context_config ctxConfig = ma_context_config_init();
  ctxConfig.logCallback = OnLog;
  ma_result result = ma_context_init(NULL, 0, &ctxConfig, &AUDIO.system.context);
  if (result != MA_SUCCESS) {
    TRACELOG(LOG_ERROR, "AUDIO: Failed to initialize context");
    return;
  }
}
