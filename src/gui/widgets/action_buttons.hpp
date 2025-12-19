#ifndef CLRSYNC_GUI_WIDGETS_ACTION_BUTTONS_HPP
#define CLRSYNC_GUI_WIDGETS_ACTION_BUTTONS_HPP

#include "core/palette/palette.hpp"
#include <functional>
#include <string>
#include <vector>

namespace clrsync::gui::widgets
{

struct action_button
{
    std::string label;
    std::string tooltip;
    std::function<void()> on_click;
    bool enabled = true;
    
    bool use_error_style = false;
    bool use_success_style = false;
};

class action_buttons
{
  public:
    action_buttons();

    void add_button(const action_button &button);

    void clear();

    void render(const core::palette &theme_palette);

    void set_spacing(float spacing) { m_spacing = spacing; }

    void set_use_separator(bool use) { m_use_separator = use; }

  private:
    std::vector<action_button> m_buttons;
    float m_spacing = 8.0f;
    bool m_use_separator = false;
};

} // namespace clrsync::gui::widgets

#endif // CLRSYNC_GUI_WIDGETS_ACTION_BUTTONS_HPP