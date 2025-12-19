#ifndef CLRSYNC_GUI_WIDGETS_VALIDATION_MESSAGE_HPP
#define CLRSYNC_GUI_WIDGETS_VALIDATION_MESSAGE_HPP

#include "core/palette/palette.hpp"
#include <string>

namespace clrsync::gui::widgets
{

class validation_message
{
  public:
    void set(const std::string& message);
    void clear();
    bool has_error() const;
    const std::string& get() const;
    void render(const core::palette& palette);

  private:
    std::string m_message;
};

} // namespace clrsync::gui::widgets

#endif // CLRSYNC_GUI_WIDGETS_VALIDATION_MESSAGE_HPP
