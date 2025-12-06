#ifndef CLRSYNC_GUI_COLOR_SCHEME_EDITOR_HPP
#define CLRSYNC_GUI_COLOR_SCHEME_EDITOR_HPP

#include "color_text_edit/TextEditor.h"
#include "palette_controller.hpp"

class template_editor;

class color_scheme_editor
{
public:
    color_scheme_editor();
    
    void render_controls_and_colors();
    void render_preview();
    void set_template_editor(template_editor* editor) { m_template_editor = editor; }
    const palette_controller& controller() const { return m_controller; }

private:
    void render_controls();
    void render_color_table();
    void render_preview_content();
    
    void apply_palette_to_editor();
    void apply_palette_to_imgui() const;
    void notify_palette_changed();

    palette_controller m_controller;
    TextEditor m_editor;
    template_editor* m_template_editor{nullptr};
};

#endif // CLRSYNC_GUI_COLOR_SCHEME_EDITOR_HPP