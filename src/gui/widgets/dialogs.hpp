#ifndef CLRSYNC_GUI_WIDGETS_DIALOGS_HPP
#define CLRSYNC_GUI_WIDGETS_DIALOGS_HPP

#include "core/palette/palette.hpp"
#include <functional>
#include <string>

namespace clrsync::gui::widgets
{

bool delete_confirmation_dialog(const std::string &popup_title, const std::string &item_name,
                                const std::string &item_type, const core::palette &theme_palette,
                                const std::function<void()> &on_delete);

} // namespace clrsync::gui::widgets

#endif // CLRSYNC_GUI_WIDGETS_DIALOGS_HPP
