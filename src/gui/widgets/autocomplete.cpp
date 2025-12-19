#include "autocomplete.hpp"
#include "colors.hpp"
#include "imgui.h"
#include <algorithm>

namespace clrsync::gui::widgets
{

autocomplete_widget::autocomplete_widget() 
    : m_selected_index(0), m_show_autocomplete(false), m_dismissed(false), m_dismiss_brace_pos(-1)
{
    m_bg_color = ImVec4(0.12f, 0.12f, 0.15f, 0.98f);
    m_border_color = ImVec4(0.4f, 0.4f, 0.45f, 1.0f);
    m_selected_color = ImVec4(0.25f, 0.45f, 0.75f, 0.9f);
    m_text_color = ImVec4(0.85f, 0.85f, 0.9f, 1.0f);
    m_selected_text_color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    m_dim_text_color = ImVec4(0.6f, 0.6f, 0.7f, 1.0f);
}

void autocomplete_widget::update_suggestions(const TextEditor& editor, 
                                            const std::vector<std::string>& available_keys,
                                            const std::vector<std::string>& available_formats)
{
    m_suggestions.clear();

    auto cursor = editor.GetCursorPosition();
    std::string line = editor.GetCurrentLineText();
    int col = cursor.mColumn;

    // Check if inside '{'
    int brace_pos = -1;
    for (int i = col - 1; i >= 0; --i)
    {
        if (i < (int)line.length())
        {
            if (line[i] == '{')
            {
                brace_pos = i;
                break;
            }
            else if (line[i] == '}' || line[i] == ' ' || line[i] == '\t')
            {
                break;
            }
        }
    }

    if (brace_pos < 0)
    {
        m_show_autocomplete = false;
        m_dismissed = false;
        return;
    }

    if (m_dismissed)
    {
        bool should_reset_dismissal = false;

        if (cursor.mLine != m_dismiss_position.mLine || brace_pos != m_dismiss_brace_pos ||
            abs(cursor.mColumn - m_dismiss_position.mColumn) > 3)
        {
            should_reset_dismissal = true;
        }

        if (should_reset_dismissal)
        {
            m_dismissed = false;
        }
        else
        {
            m_show_autocomplete = false;
            return;
        }
    }

    m_prefix = line.substr(brace_pos + 1, col - brace_pos - 1);
    m_start_pos = TextEditor::Coordinates(cursor.mLine, brace_pos + 1);

    size_t dot_pos = m_prefix.find('.');

    if (dot_pos != std::string::npos)
    {
        std::string color_key = m_prefix.substr(0, dot_pos);
        std::string format_prefix = m_prefix.substr(dot_pos + 1);

        bool valid_key = std::find(available_keys.begin(), available_keys.end(), color_key) != available_keys.end();

        if (valid_key)
        {
            for (const auto &fmt : available_formats)
            {
                if (format_prefix.empty() || fmt.find(format_prefix) == 0 ||
                    fmt.find(format_prefix) != std::string::npos)
                {
                    m_suggestions.push_back(color_key + "." + fmt);
                }
            }
        }
    }
    else
    {
        for (const auto& key : available_keys)
        {
            if (m_prefix.empty() || key.find(m_prefix) == 0 ||
                key.find(m_prefix) != std::string::npos)
            {
                m_suggestions.push_back(key);
            }
        }
    }

    std::sort(m_suggestions.begin(), m_suggestions.end(),
              [this](const std::string &a, const std::string &b) {
                  bool a_prefix = a.find(m_prefix) == 0;
                  bool b_prefix = b.find(m_prefix) == 0;
                  if (a_prefix != b_prefix)
                      return a_prefix;
                  return a < b;
              });

    m_show_autocomplete = !m_suggestions.empty();
    if (m_show_autocomplete && m_selected_index >= (int)m_suggestions.size())
    {
        m_selected_index = 0;
    }
}

void autocomplete_widget::render(const ImVec2& editor_pos, TextEditor& editor)
{
    if (!m_show_autocomplete || m_suggestions.empty())
        return;

    float line_height = ImGui::GetTextLineHeightWithSpacing();
    float char_width = ImGui::GetFontSize() * 0.5f;
    auto cursor = editor.GetCursorPosition();

    const float line_number_width = 50.0f;

    ImVec2 popup_pos;
    popup_pos.x = editor_pos.x + line_number_width + (m_start_pos.mColumn * char_width);
    popup_pos.y = editor_pos.y + ((cursor.mLine + 1) * line_height);

    ImGui::SetNextWindowPos(popup_pos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(300, 0));

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                             ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings |
                             ImGuiWindowFlags_NoFocusOnAppearing |
                             ImGuiWindowFlags_AlwaysAutoResize;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(6, 6));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 6.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 2));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, m_bg_color);
    ImGui::PushStyleColor(ImGuiCol_Border, m_border_color);

    if (ImGui::Begin("##autocomplete", nullptr, flags))
    {
        ImGui::PushStyleColor(ImGuiCol_Text, m_dim_text_color);
        if (m_prefix.find('.') != std::string::npos)
            ImGui::Text("Formats");
        else
            ImGui::Text("Color Keys");
        ImGui::PopStyleColor();
        ImGui::Separator();

        int max_items = std::min((int)m_suggestions.size(), 8);

        for (int i = 0; i < max_items; ++i)
        {
            const auto &suggestion = m_suggestions[i];
            bool is_selected = (i == m_selected_index);

            if (is_selected)
            {
                ImVec4 selected_hover = m_selected_color;
                selected_hover.w = std::min(selected_hover.w + 0.1f, 1.0f);
                ImGui::PushStyleColor(ImGuiCol_Header, m_selected_color);
                ImGui::PushStyleColor(ImGuiCol_HeaderHovered, selected_hover);
                ImGui::PushStyleColor(ImGuiCol_Text, m_selected_text_color);
            }
            else
            {
                ImVec4 normal_bg = m_bg_color;
                normal_bg.w = 0.5f;
                ImVec4 hover_bg = m_selected_color;
                hover_bg.w = 0.3f;
                ImGui::PushStyleColor(ImGuiCol_Header, normal_bg);
                ImGui::PushStyleColor(ImGuiCol_HeaderHovered, hover_bg);
                ImGui::PushStyleColor(ImGuiCol_Text, m_text_color);
            }

            std::string display_text = "  " + suggestion;

            if (ImGui::Selectable(display_text.c_str(), is_selected, ImGuiSelectableFlags_None, ImVec2(0, 0)))
            {
                auto start = m_start_pos;
                auto end = editor.GetCursorPosition();
                editor.SetSelection(start, end);
                editor.Delete();
                editor.InsertText(suggestion + "}");
                m_show_autocomplete = false;
                m_dismissed = false;
            }

            ImGui::PopStyleColor(3);

            if (is_selected && ImGui::IsWindowAppearing())
            {
                ImGui::SetScrollHereY();
            }
        }

        if (m_suggestions.size() > 8)
        {
            ImGui::Separator();
            ImGui::PushStyleColor(ImGuiCol_Text, m_dim_text_color);
            ImGui::Text("  +%d more", (int)m_suggestions.size() - 8);
            ImGui::PopStyleColor();
        }

        ImGui::Separator();
        ImGui::PushStyleColor(ImGuiCol_Text, m_dim_text_color);
        ImGui::Text("  Tab/Enter: accept | Esc: dismiss");
        ImGui::PopStyleColor();
    }
    ImGui::End();

    ImGui::PopStyleColor(2);
    ImGui::PopStyleVar(3);
}

void autocomplete_widget::apply_palette(const core::palette& palette)
{
    m_bg_color = palette_color(palette, "surface", "background");
    m_bg_color.w = 0.98f;
    m_border_color = palette_color(palette, "border", "surface_variant");
    m_selected_color = palette_color(palette, "accent", "surface_variant");
    m_text_color = palette_color(palette, "on_surface", "foreground");
    m_selected_text_color = palette_color(palette, "on_surface", "foreground");
    m_dim_text_color = palette_color(palette, "on_surface_variant", "editor_inactive");
}

bool autocomplete_widget::handle_input(TextEditor& editor)
{
    if (!m_show_autocomplete || m_suggestions.empty())
        return false;

    bool handled = false;

    if (ImGui::IsKeyPressed(ImGuiKey_DownArrow))
    {
        m_selected_index = (m_selected_index + 1) % (int)m_suggestions.size();
        handled = true;
    }
    else if (ImGui::IsKeyPressed(ImGuiKey_UpArrow))
    {
        m_selected_index = (m_selected_index - 1 + (int)m_suggestions.size()) % (int)m_suggestions.size();
        handled = true;
    }
    else if (ImGui::IsKeyPressed(ImGuiKey_Tab) || ImGui::IsKeyPressed(ImGuiKey_Enter))
    {
        if (m_selected_index >= 0 && m_selected_index < (int)m_suggestions.size())
        {
            auto start = m_start_pos;
            auto end = editor.GetCursorPosition();
            editor.SetSelection(start, end);
            editor.Delete();
            editor.InsertText(m_suggestions[m_selected_index] + "}");
            m_show_autocomplete = false;
            m_dismissed = false;
        }
        handled = true;
    }
    else if (ImGui::IsKeyPressed(ImGuiKey_Escape))
    {
        m_dismissed = true;
        m_dismiss_position = editor.GetCursorPosition();
        m_dismiss_brace_pos = -1;
        m_show_autocomplete = false;
        handled = true;
    }

    return handled;
}

} // namespace clrsync::gui::widgets