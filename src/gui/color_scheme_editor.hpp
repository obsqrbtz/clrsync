#ifndef CLRSYNC_GUI_COLOR_SCHEME_EDITOR_HPP
#define CLRSYNC_GUI_COLOR_SCHEME_EDITOR_HPP

#include "palette_controller.hpp"
#include "color_table_renderer.hpp"
#include "preview_renderer.hpp"

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
    void apply_themes();
    void notify_palette_changed();

    palette_controller m_controller;
    color_table_renderer m_color_table;
    preview_renderer m_preview;
    template_editor* m_template_editor{nullptr};
    bool m_show_delete_confirmation{false};
};

#endif // CLRSYNC_GUI_COLOR_SCHEME_EDITOR_HPP