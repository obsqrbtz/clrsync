#ifndef CLRSYNC_GUI_TEMPLATE_EDITOR_HPP
#define CLRSYNC_GUI_TEMPLATE_EDITOR_HPP

#include "template_controller.hpp"
#include <core/palette/palette.hpp>
#include "color_text_edit/TextEditor.h"
#include <string>

class template_editor
{
public:
    template_editor();
    void render();
    void apply_current_palette(const clrsync::core::palette& pal);

private:
    void render_controls();
    void render_editor();
    void render_template_list();

    void save_template();
    void load_template(const std::string &name);
    void new_template();
    void delete_template();
    void refresh_templates();

    bool is_valid_path(const std::string &path);

    template_controller m_template_controller;
    TextEditor m_editor;
    
    std::string m_template_name;
    std::string m_input_path;
    std::string m_output_path;
    std::string m_reload_command;
    std::string m_validation_error;
    std::string m_saved_content;
    bool m_has_unsaved_changes = false;
    
    bool m_enabled{true};
    bool m_is_editing_existing{false};
    bool m_show_delete_confirmation{false};
};

#endif // CLRSYNC_GUI_TEMPLATE_EDITOR_HPP