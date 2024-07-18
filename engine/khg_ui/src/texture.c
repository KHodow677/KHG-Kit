#include "khg_ui/internal.h"
#include "khg_ui/texture.h"

ui_font ui_load_font(const char *filepath, uint32_t size) {
  return load_font(filepath, size, 1024, 1024, 0);
}

ui_font ui_load_font_ex(const char *filepath, uint32_t size, uint32_t bitmap_w, uint32_t bitmap_h) {
  return load_font(filepath, size, bitmap_w, bitmap_h, 0);
}
