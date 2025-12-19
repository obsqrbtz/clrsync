#ifndef CLRSYNC_GUI_WIDGETS_AUTOCOMPLETE_HPP
#define CLRSYNC_GUI_WIDGETS_AUTOCOMPLETE_HPP

#include "core/palette/palette.hpp"
#include "color_text_edit/TextEditor.h"
#include "imgui.h"
#include <string>
#include <vector>

namespace clrsync::gui::widgets
{

class autocomplete_widget
{
  public:
    autocomplete_widget();

    void update_suggestions(const TextEditor& editor, 
                           const std::vector<std::string>& available_keys,
                           const std::vector<std::string>& available_formats);

    void render(const ImVec2& editor_pos, TextEditor& editor);

    void apply_palette(const core::palette& palette);

    bool handle_input(TextEditor& editor);

    bool is_visible() const { return m_show_autocomplete; }

    void dismiss() { m_dismissed = true; m_show_autocomplete = false; }

  private:
    std::vector<std::string> m_suggestions;
    std::string m_prefix;
    TextEditor::Coordinates m_start_pos;
    int m_selected_index;
    bool m_show_autocomplete;
    bool m_dismissed;
    
    TextEditor::Coordinates m_dismiss_position;
    int m_dismiss_brace_pos;
    
    ImVec4 m_bg_color;
    ImVec4 m_border_color;
    ImVec4 m_selected_color;
    ImVec4 m_text_color;
    ImVec4 m_selected_text_color;
    ImVec4 m_dim_text_color;
};

} // namespace clrsync::gui::widgets

#endif // CLRSYNC_GUI_WIDGETS_AUTOCOMPLETE_HPP