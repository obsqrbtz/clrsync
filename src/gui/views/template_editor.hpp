#ifndef CLRSYNC_GUI_TEMPLATE_EDITOR_HPP
#define CLRSYNC_GUI_TEMPLATE_EDITOR_HPP

#include "color_text_edit/TextEditor.h"
#include "core/palette/palette.hpp"
#include "gui/controllers/template_controller.hpp"
#include "gui/widgets/template_controls.hpp"
#include "gui/widgets/validation_message.hpp"
#include "imgui.h"
#include <string>
#include <vector>

namespace clrsync::gui
{
class ui_manager;
}

class template_editor
{
  public:
    template_editor(clrsync::gui::ui_manager* ui_mgr);
    void render();
    void apply_current_palette(const clrsync::core::palette &pal);

  private:
    void render_controls();
    void render_editor();
    void render_template_list();
    void render_autocomplete(const ImVec2 &editor_pos);
    void update_autocomplete_suggestions();

    void save_template();
    void load_template(const std::string &name);
    void new_template();
    void delete_template();
    void refresh_templates();
    void setup_callbacks();

    bool is_valid_path(const std::string &path);

    template_controller m_template_controller;
    TextEditor m_editor;

    clrsync::gui::widgets::template_control_state m_control_state;
    clrsync::gui::widgets::template_controls m_controls;
    clrsync::gui::widgets::template_control_callbacks m_callbacks;
    clrsync::gui::widgets::validation_message m_validation;

    std::string m_saved_content;
    bool m_has_unsaved_changes{false};
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
    clrsync::gui::ui_manager* m_ui_manager;
};

#endif // CLRSYNC_GUI_TEMPLATE_EDITOR_HPP