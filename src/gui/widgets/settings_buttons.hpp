#ifndef CLRSYNC_GUI_WIDGETS_SETTINGS_BUTTONS_HPP
#define CLRSYNC_GUI_WIDGETS_SETTINGS_BUTTONS_HPP

#include <functional>

namespace clrsync::gui::widgets
{

struct settings_buttons_callbacks
{
    std::function<void()> on_ok;
    std::function<void()> on_apply;
    std::function<void()> on_reset;
    std::function<void()> on_cancel;
};

class settings_buttons
{
  public:
    void render(const settings_buttons_callbacks& callbacks, bool apply_enabled);

    void set_button_width(float width) { m_button_width = width; }

  private:
    float m_button_width = 100.0f;
};

} // namespace clrsync::gui::widgets

#endif // CLRSYNC_GUI_WIDGETS_SETTINGS_BUTTONS_HPP
