#ifndef CLRSYNC_GUI_WIDGETS_PALETTE_SELECTOR_HPP
#define CLRSYNC_GUI_WIDGETS_PALETTE_SELECTOR_HPP

#include "gui/controllers/palette_controller.hpp"
#include <functional>
#include <string>

namespace clrsync::gui::widgets
{

class palette_selector
{
  public:
    palette_selector();

    bool render(const palette_controller &controller, float width = 200.0f);

    void set_on_selection_changed(const std::function<void(const std::string &)> &callback);

  private:
    std::function<void(const std::string &)> m_on_selection_changed;
};

} // namespace clrsync::gui::widgets

#endif // CLRSYNC_GUI_WIDGETS_PALETTE_SELECTOR_HPP