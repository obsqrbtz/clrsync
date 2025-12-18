#ifndef CLRSYNC_GUI_PREVIEW_RENDERER_HPP
#define CLRSYNC_GUI_PREVIEW_RENDERER_HPP

#include "color_text_edit/TextEditor.h"
#include "core/palette/palette.hpp"

class preview_renderer
{
  public:
    preview_renderer();

    void render(const clrsync::core::palette &palette);
    void apply_palette(const clrsync::core::palette &palette);

  private:
    void render_code_preview();
    void render_terminal_preview(const clrsync::core::palette &palette);

    TextEditor m_editor;
};

#endif // CLRSYNC_GUI_PREVIEW_RENDERER_HPP
