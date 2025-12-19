#ifndef CLRSYNC_GUI_WIDGETS_CENTERED_TEXT_HPP
#define CLRSYNC_GUI_WIDGETS_CENTERED_TEXT_HPP

#include "imgui.h"
#include <functional>
#include <string>

namespace clrsync::gui::widgets
{

void centered_text(const std::string& text, const ImVec4& color = ImVec4(1, 1, 1, 1));
void centered_buttons(float total_width, const std::function<void()>& render_buttons);

} // namespace clrsync::gui::widgets

#endif // CLRSYNC_GUI_WIDGETS_CENTERED_TEXT_HPP
