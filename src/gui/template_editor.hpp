#ifndef CLRSYNC_GUI_TEMPLATE_EDITOR_HPP
#define CLRSYNC_GUI_TEMPLATE_EDITOR_HPP

#include "template_controller.hpp"
#include <core/palette/palette.hpp>
#include "color_text_edit/TextEditor.h"
#include "imgui.h"
#include <string>
#include <vector>

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
    void render_autocomplete(const ImVec2& editor_pos);
    void update_autocomplete_suggestions();

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

    bool m_show_autocomplete{false};
    bool m_autocomplete_dismissed{false};
    TextEditor::Coordinates m_dismiss_position;
    int m_dismiss_brace_pos{-1};
    std::vector<std::string> m_autocomplete_suggestions;
    int m_autocomplete_selected{0};
    std::string m_autocomplete_prefix;
    TextEditor::Coordinates m_autocomplete_start_pos;
    
    ImVec4 m_autocomplete_bg_color;
    ImVec4 m_autocomplete_border_color;
    ImVec4 m_autocomplete_selected_color;
    ImVec4 m_autocomplete_text_color;
    ImVec4 m_autocomplete_selected_text_color;
    ImVec4 m_autocomplete_dim_text_color;
    
    clrsync::core::palette m_current_palette;
};

#endif // CLRSYNC_GUI_TEMPLATE_EDITOR_HPP