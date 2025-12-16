#ifndef CLRSYNC_GUI_THEME_APPLIER_HPP
#define CLRSYNC_GUI_THEME_APPLIER_HPP

#include "core/palette/palette.hpp"
#include "color_text_edit/TextEditor.h"

namespace theme_applier
{
    void apply_to_imgui(const clrsync::core::palette& pal);
    void apply_to_editor(TextEditor& editor, const clrsync::core::palette& pal);
}

#endif // CLRSYNC_GUI_THEME_APPLIER_HPP
