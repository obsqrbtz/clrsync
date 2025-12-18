#ifndef CLRSYNC_GUI_THEME_APPLIER_HPP
#define CLRSYNC_GUI_THEME_APPLIER_HPP

#include "color_text_edit/TextEditor.h"
#include "core/palette/palette.hpp"

namespace theme_applier
{
void apply_to_imgui(const clrsync::core::palette &pal);
void apply_to_editor(TextEditor &editor, const clrsync::core::palette &pal);
} // namespace theme_applier

#endif // CLRSYNC_GUI_THEME_APPLIER_HPP
