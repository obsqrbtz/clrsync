#ifndef CLRSYNC_GUI_WIDGETS_COLORS_HPP
#define CLRSYNC_GUI_WIDGETS_COLORS_HPP

#include "core/palette/palette.hpp"
#include "imgui.h"
#include <string>

namespace clrsync::gui::widgets
{

ImVec4 palette_color(const core::palette &pal, const std::string &key,
                     const std::string &fallback = "");

uint32_t palette_color_u32(const core::palette &pal, const std::string &key,
                           const std::string &fallback = "");

} // namespace clrsync::gui::widgets

#endif // CLRSYNC_GUI_WIDGETS_COLORS_HPP
