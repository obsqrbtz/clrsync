#ifndef CLRSYNC_GUI_WIDGETS_LINK_BUTTON_HPP
#define CLRSYNC_GUI_WIDGETS_LINK_BUTTON_HPP

#include <string>

namespace clrsync::gui::widgets
{

void open_url(const std::string& url);
bool link_button(const std::string& label, const std::string& url, float width = 0.0f);

} // namespace clrsync::gui::widgets

#endif // CLRSYNC_GUI_WIDGETS_LINK_BUTTON_HPP
