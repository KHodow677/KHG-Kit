#pragma once
#include "gfx/gfx.h"

LfTextProps text_render_simple(vec2s pos, const char* text, LfFont font, LfColor font_color, bool no_render);
LfTextProps text_render_simple_wide(vec2s pos, const wchar_t* text, LfFont font, LfColor font_color, bool no_render);

LfClickableItemState button(const char* file, int32_t line, vec2s pos, vec2s size, LfUIElementProps props, LfColor color, float border_width, bool click_color, bool hover_color);
LfClickableItemState button_ex(const char* file, int32_t line, vec2s pos, vec2s size, LfUIElementProps props, LfColor color, float border_width, bool click_color, bool hover_color, vec2s hitbox_override);
LfClickableItemState div_container(vec2s pos, vec2s size, LfUIElementProps props, LfColor color, float border_width, bool click_color, bool hover_color);
void next_line_on_overflow(vec2s size, float xoffset);
bool item_should_cull(LfAABB item);
void draw_scrollbar_on(LfDiv* div);
void input_field(LfInputField* input, InputFieldType type, const char* file, int32_t line);

LfClickableItemState button_element_loc(void* text, const char* file, int32_t line, bool wide);
LfClickableItemState button_fixed_element_loc(void* text, float width, float height, const char* file, int32_t line, bool wide);
LfClickableItemState checkbox_element_loc(void* text, bool* val, LfColor tick_color, LfColor tex_color, const char* file, int32_t line, bool wide);
void dropdown_menu_item_loc(void** items, void* placeholder, uint32_t item_count, float width, float height, int32_t* selected_index, bool* opened, const char* file, int32_t line, bool wide);
int32_t menu_item_list_item_loc(void** items, uint32_t item_count, int32_t selected_index, LfMenuItemCallback per_cb, bool vertical, const char* file, int32_t line, bool wide);
