#ifndef CLRSYNC_GUI_WIDGETS_COLORS_HPP
#define CLRSYNC_GUI_WIDGETS_COLORS_HPP

#include "core/palette/palette.hpp"
#include "gui/theme/app_theme.hpp"
#include "imgui.h"
#include <string>

namespace clrsync::gui::widgets
{

ImVec4 palette_color(const core::palette &pal, const std::string &key,
                     const std::string &fallback = "");

uint32_t palette_color_u32(const core::palette &pal, const std::string &key,
                           const std::string &fallback = "");

inline const theme::app_theme &theme() { return theme::current_theme(); }

void push_success_button_style();

void push_error_button_style();

void push_warning_button_style();

void push_info_button_style();

void pop_button_style();

} // namespace clrsync::gui::widgets

#endif // CLRSYNC_GUI_WIDGETS_COLORS_HPP
