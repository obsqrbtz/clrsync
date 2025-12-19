#ifndef CLRSYNC_GUI_WIDGETS_STYLED_CHECKBOX_HPP
#define CLRSYNC_GUI_WIDGETS_STYLED_CHECKBOX_HPP

#include "core/palette/palette.hpp"
#include <functional>
#include <string>

namespace clrsync::gui::widgets
{

enum class checkbox_style
{
    normal,
    success,
    error,
    warning
};

class styled_checkbox
{
  public:
    styled_checkbox();

    bool render(const std::string &label, bool *value, const core::palette &theme_palette, 
                checkbox_style style = checkbox_style::normal);

    void set_tooltip(const std::string &tooltip) { m_tooltip = tooltip; }

    void set_on_changed(const std::function<void(bool)> &callback) { m_on_changed = callback; }

  private:
    std::string m_tooltip;
    std::function<void(bool)> m_on_changed;
};

} // namespace clrsync::gui::widgets

#endif // CLRSYNC_GUI_WIDGETS_STYLED_CHECKBOX_HPP