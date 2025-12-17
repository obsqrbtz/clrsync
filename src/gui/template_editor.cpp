#include "template_editor.hpp"
#include "core/config/config.hpp"
#include "core/theme/theme_template.hpp"
#include "core/palette/color_keys.hpp"
#include "core/utils.hpp"
#include "imgui_helpers.hpp"
#include "file_browser.hpp"
#include "imgui.h"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <ranges>

namespace {
    const std::vector<std::string> COLOR_FORMATS = {
        "hex", "hex_stripped", "hexa", "hexa_stripped",
        "r", "g", "b", "a",
        "rgb", "rgba",
        "h", "s", "l",
        "hsl", "hsla"
    };
}

template_editor::template_editor() : m_template_name("new_template")
{
    m_autocomplete_bg_color = ImVec4(0.12f, 0.12f, 0.15f, 0.98f);
    m_autocomplete_border_color = ImVec4(0.4f, 0.4f, 0.45f, 1.0f);
    m_autocomplete_selected_color = ImVec4(0.25f, 0.45f, 0.75f, 0.9f);
    m_autocomplete_text_color = ImVec4(0.85f, 0.85f, 0.9f, 1.0f);
    m_autocomplete_selected_text_color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    m_autocomplete_dim_text_color = ImVec4(0.6f, 0.6f, 0.7f, 1.0f);
    
    TextEditor::LanguageDefinition lang;
    lang.mName = "Template";

    lang.mCommentStart = "/*";
    lang.mCommentEnd = "*/";
    lang.mSingleLineComment = "#";

    lang.mTokenRegexStrings.push_back(std::make_pair<std::string, TextEditor::PaletteIndex>(
        "\\{[a-zA-Z_][a-zA-Z0-9_]*(\\.[a-zA-Z_][a-zA-Z0-9_]*)?\\}",
        TextEditor::PaletteIndex::KnownIdentifier));

    lang.mTokenRegexStrings.push_back(std::make_pair<std::string, TextEditor::PaletteIndex>(
        "\"([^\"]*)\"", TextEditor::PaletteIndex::String));
    lang.mTokenRegexStrings.push_back(std::make_pair<std::string, TextEditor::PaletteIndex>(
        "'([^']*)'", TextEditor::PaletteIndex::String));

    m_editor.SetLanguageDefinition(lang);
    m_editor.SetText("# Enter your template here\n# Use {color_key} for color variables\n# "
                     "Examples: {color.hex}, {color.rgb}, {color.r}\n\n");
    m_editor.SetShowWhitespaces(false);
}

void template_editor::apply_current_palette(const clrsync::core::palette &pal)
{
    m_current_palette = pal;
    auto colors = pal.colors();
    if (colors.empty())
        return;
    auto get_color_u32 = [&](const std::string &key, const std::string &fallback = "") -> uint32_t {
        return palette_utils::get_color_u32(pal, key, fallback);
    };

    auto palette = m_editor.GetPalette();

    palette[int(TextEditor::PaletteIndex::Default)] = get_color_u32("editor_main", "foreground");
    palette[int(TextEditor::PaletteIndex::Keyword)] = get_color_u32("editor_command", "accent");
    palette[int(TextEditor::PaletteIndex::Number)] = get_color_u32("editor_warning", "warning");
    palette[int(TextEditor::PaletteIndex::String)] = get_color_u32("editor_string", "success");
    palette[int(TextEditor::PaletteIndex::CharLiteral)] = get_color_u32("editor_string", "success");
    palette[int(TextEditor::PaletteIndex::Punctuation)] =
        get_color_u32("editor_main", "foreground");
    palette[int(TextEditor::PaletteIndex::Preprocessor)] =
        get_color_u32("editor_emphasis", "accent");
    palette[int(TextEditor::PaletteIndex::Identifier)] = get_color_u32("editor_main", "foreground");
    palette[int(TextEditor::PaletteIndex::KnownIdentifier)] = get_color_u32("editor_link", "info");
    palette[int(TextEditor::PaletteIndex::PreprocIdentifier)] =
        get_color_u32("editor_link", "info");

    palette[int(TextEditor::PaletteIndex::Comment)] =
        get_color_u32("editor_comment", "editor_inactive");
    palette[int(TextEditor::PaletteIndex::MultiLineComment)] =
        get_color_u32("editor_comment", "editor_inactive");

    palette[int(TextEditor::PaletteIndex::Background)] =
        get_color_u32("editor_background", "background");
    palette[int(TextEditor::PaletteIndex::Cursor)] = get_color_u32("cursor", "accent");

    palette[int(TextEditor::PaletteIndex::Selection)] =
        get_color_u32("editor_selected", "surface_variant");
    palette[int(TextEditor::PaletteIndex::ErrorMarker)] = get_color_u32("editor_error", "error");
    palette[int(TextEditor::PaletteIndex::Breakpoint)] = get_color_u32("editor_error", "error");

    palette[int(TextEditor::PaletteIndex::LineNumber)] =
        get_color_u32("editor_line_number", "editor_inactive");
    palette[int(TextEditor::PaletteIndex::CurrentLineFill)] = get_color_u32("surface_variant");
    palette[int(TextEditor::PaletteIndex::CurrentLineFillInactive)] = get_color_u32("surface");

    palette[int(TextEditor::PaletteIndex::CurrentLineEdge)] =
        get_color_u32("border_focused", "border");

    m_editor.SetPalette(palette);
    
    m_autocomplete_bg_color = palette_utils::get_color(pal, "editor_background", "background");
    m_autocomplete_bg_color.w = 0.98f;
    m_autocomplete_border_color = palette_utils::get_color(pal, "border", "editor_inactive");
    m_autocomplete_selected_color = palette_utils::get_color(pal, "editor_selected", "surface_variant");
    m_autocomplete_text_color = palette_utils::get_color(pal, "editor_main", "foreground");
    m_autocomplete_selected_text_color = palette_utils::get_color(pal, "foreground", "editor_main");
    m_autocomplete_dim_text_color = palette_utils::get_color(pal, "editor_comment", "editor_inactive");
}

void template_editor::update_autocomplete_suggestions()
{
    m_autocomplete_suggestions.clear();
    
    auto cursor = m_editor.GetCursorPosition();
    std::string line = m_editor.GetCurrentLineText();
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
        m_autocomplete_dismissed = false; 
        return;
    }
    
    if (m_autocomplete_dismissed)
    {
        bool should_reset_dismissal = false;
        
        if (cursor.mLine != m_dismiss_position.mLine ||
            brace_pos != m_dismiss_brace_pos ||
            abs(cursor.mColumn - m_dismiss_position.mColumn) > 3)
        {
            should_reset_dismissal = true;
        }
        
        if (should_reset_dismissal)
        {
            m_autocomplete_dismissed = false;
        }
        else
        {
            m_show_autocomplete = false;
            return;
        }
    }
    
    m_autocomplete_prefix = line.substr(brace_pos + 1, col - brace_pos - 1);
    m_autocomplete_start_pos = TextEditor::Coordinates(cursor.mLine, brace_pos + 1);
    
    size_t dot_pos = m_autocomplete_prefix.find('.');
    
    if (dot_pos != std::string::npos)
    {
        std::string color_key = m_autocomplete_prefix.substr(0, dot_pos);
        std::string format_prefix = m_autocomplete_prefix.substr(dot_pos + 1);
        
        bool valid_key = false;
        for (size_t i = 0; i < clrsync::core::NUM_COLOR_KEYS; ++i)
        {
            if (clrsync::core::COLOR_KEYS[i] == color_key)
            {
                valid_key = true;
                break;
            }
        }
        
        if (valid_key)
        {
            for (const auto &fmt : COLOR_FORMATS)
            {
                if (format_prefix.empty() || 
                    fmt.find(format_prefix) == 0 ||
                    fmt.find(format_prefix) != std::string::npos)
                {
                    m_autocomplete_suggestions.push_back(color_key + "." + fmt);
                }
            }
        }
    }
    else
    {
        for (size_t i = 0; i < clrsync::core::NUM_COLOR_KEYS; ++i)
        {
            std::string key = clrsync::core::COLOR_KEYS[i];
            if (m_autocomplete_prefix.empty() || 
                key.find(m_autocomplete_prefix) == 0 ||
                key.find(m_autocomplete_prefix) != std::string::npos)
            {
                m_autocomplete_suggestions.push_back(key);
            }
        }
    }
    
    std::sort(m_autocomplete_suggestions.begin(), m_autocomplete_suggestions.end(),
        [this](const std::string &a, const std::string &b) {
            bool a_prefix = a.find(m_autocomplete_prefix) == 0;
            bool b_prefix = b.find(m_autocomplete_prefix) == 0;
            if (a_prefix != b_prefix)
                return a_prefix;
            return a < b;
        });
    
    m_show_autocomplete = !m_autocomplete_suggestions.empty();
    if (m_show_autocomplete && m_autocomplete_selected >= (int)m_autocomplete_suggestions.size())
    {
        m_autocomplete_selected = 0;
    }
}

void template_editor::render_autocomplete(const ImVec2& editor_pos)
{
    if (!m_show_autocomplete || m_autocomplete_suggestions.empty())
        return;
    
    float line_height = ImGui::GetTextLineHeightWithSpacing();
    float char_width = ImGui::GetFontSize() * 0.5f;
    auto cursor = m_editor.GetCursorPosition();
    
    const float line_number_width = 50.0f;
    
    ImVec2 popup_pos;
    popup_pos.x = editor_pos.x + line_number_width + (m_autocomplete_start_pos.mColumn * char_width);
    popup_pos.y = editor_pos.y + ((cursor.mLine + 1) * line_height);
    
    ImGui::SetNextWindowPos(popup_pos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(300, 0));
    
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                             ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings |
                             ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_AlwaysAutoResize;
    
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(6, 6));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 6.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 2));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, m_autocomplete_bg_color);
    ImGui::PushStyleColor(ImGuiCol_Border, m_autocomplete_border_color);
    
    if (ImGui::Begin("##autocomplete", nullptr, flags))
    {
        ImGui::PushStyleColor(ImGuiCol_Text, m_autocomplete_dim_text_color);
        if (m_autocomplete_prefix.find('.') != std::string::npos)
            ImGui::Text("Formats");
        else
            ImGui::Text("Color Keys");
        ImGui::PopStyleColor();
        ImGui::Separator();
        
        int max_items = std::min((int)m_autocomplete_suggestions.size(), 8);
        
        for (int i = 0; i < max_items; ++i)
        {
            const auto &suggestion = m_autocomplete_suggestions[i];
            bool is_selected = (i == m_autocomplete_selected);
            
            if (is_selected)
            {
                ImVec4 selected_hover = m_autocomplete_selected_color;
                selected_hover.w = std::min(selected_hover.w + 0.1f, 1.0f);
                ImGui::PushStyleColor(ImGuiCol_Header, m_autocomplete_selected_color);
                ImGui::PushStyleColor(ImGuiCol_HeaderHovered, selected_hover);
                ImGui::PushStyleColor(ImGuiCol_Text, m_autocomplete_selected_text_color);
            }
            else
            {
                ImVec4 normal_bg = m_autocomplete_bg_color;
                normal_bg.w = 0.5f;
                ImVec4 hover_bg = m_autocomplete_selected_color;
                hover_bg.w = 0.3f;
                ImGui::PushStyleColor(ImGuiCol_Header, normal_bg);
                ImGui::PushStyleColor(ImGuiCol_HeaderHovered, hover_bg);
                ImGui::PushStyleColor(ImGuiCol_Text, m_autocomplete_text_color);
            }
            
            std::string display_text = "  " + suggestion;
            
            if (ImGui::Selectable(display_text.c_str(), is_selected, 
                                  ImGuiSelectableFlags_None, ImVec2(0, 0)))
            {
                auto start = m_autocomplete_start_pos;
                auto end = m_editor.GetCursorPosition();
                m_editor.SetSelection(start, end);
                m_editor.Delete();
                m_editor.InsertText(suggestion + "}");
                m_show_autocomplete = false;
                m_autocomplete_dismissed = false;
            }
            
            ImGui::PopStyleColor(3);
            
            if (is_selected && ImGui::IsWindowAppearing())
            {
                ImGui::SetScrollHereY();
            }
        }
        
        if (m_autocomplete_suggestions.size() > 8)
        {
            ImGui::Separator();
            ImGui::PushStyleColor(ImGuiCol_Text, m_autocomplete_dim_text_color);
            ImGui::Text("  +%d more", (int)m_autocomplete_suggestions.size() - 8);
            ImGui::PopStyleColor();
        }
        
        ImGui::Separator();
        ImGui::PushStyleColor(ImGuiCol_Text, m_autocomplete_dim_text_color);
        ImGui::Text("  Tab/Enter: accept | Esc: dismiss");
        ImGui::PopStyleColor();
    }
    ImGui::End();
    
    ImGui::PopStyleColor(2);
    ImGui::PopStyleVar(3);
}

void template_editor::render()
{
    ImGui::Begin("Templates");

    render_controls();
    ImGui::Separator();

    const float panel_width = ImGui::GetContentRegionAvail().x;
    constexpr float left_panel_width = 200.0f;
    const float right_panel_width = panel_width - left_panel_width - 10;

    ImGui::BeginChild("TemplateList", ImVec2(left_panel_width, 0), true);
    render_template_list();
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild("EditorPanel", ImVec2(right_panel_width, 0), false);
    render_editor();
    ImGui::EndChild();

    if (m_show_delete_confirmation)
    {
        ImGui::OpenPopup("Delete Template?");
        m_show_delete_confirmation = false;
    }

    palette_utils::render_delete_confirmation_popup(
        "Delete Template?", m_template_name, "template", m_current_palette,
        [this]() {
            bool success = m_template_controller.remove_template(m_template_name);
            if (success)
            {
                new_template();
                refresh_templates();
            }
            else
            {
                m_validation_error = "Failed to delete template";
            }
        });

    ImGui::End();
}

void template_editor::render_controls()
{
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 8));
    
    if (ImGui::Button(" + New "))
    {
        new_template();
    }
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Create a new template");

    if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) &&
        ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_S))
    {
        save_template();
    }

    ImGui::SameLine();
    if (ImGui::Button(" Save "))
    {
        save_template();
    }
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Save template (Ctrl+S)");

    if (m_is_editing_existing)
    {
        ImGui::SameLine();
        auto error = palette_utils::get_color(m_current_palette, "error");
        auto error_hover = ImVec4(error.x * 1.1f, error.y * 1.1f, error.z * 1.1f,
                                error.w);
        auto error_active = ImVec4(error.x * 0.8f, error.y * 0.8f, error.z * 0.8f,
                                error.w);
        auto on_error = palette_utils::get_color(m_current_palette, "on_error");
        ImGui::PushStyleColor(ImGuiCol_Button, error);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, error_hover);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, error_active);
        ImGui::PushStyleColor(ImGuiCol_Text, on_error);
        if (ImGui::Button(" Delete "))
        {
            delete_template();
        }
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Delete this template");
        ImGui::PopStyleColor(4);
    }

    ImGui::SameLine();
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
    
    bool enabled_changed = false;
    if (m_enabled)
    {
        ImVec4 success_color = palette_utils::get_color(m_current_palette, "success", "accent");
        ImVec4 success_hover = ImVec4(success_color.x * 1.2f, success_color.y * 1.2f, success_color.z * 1.2f, 0.6f);
        ImVec4 success_check = ImVec4(success_color.x * 1.5f, success_color.y * 1.5f, success_color.z * 1.5f, 1.0f);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(success_color.x, success_color.y, success_color.z, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, success_hover);
        ImGui::PushStyleColor(ImGuiCol_CheckMark, success_check);
    }
    else
    {
        ImVec4 error_color = palette_utils::get_color(m_current_palette, "error", "accent");
        ImVec4 error_hover = ImVec4(error_color.x * 1.2f, error_color.y * 1.2f, error_color.z * 1.2f, 0.6f);
        ImVec4 error_check = ImVec4(error_color.x * 1.5f, error_color.y * 1.5f, error_color.z * 1.5f, 1.0f);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(error_color.x, error_color.y, error_color.z, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, error_hover);
        ImGui::PushStyleColor(ImGuiCol_CheckMark, error_check);
    }
    
    enabled_changed = ImGui::Checkbox("Enabled", &m_enabled);
    ImGui::PopStyleColor(3);
    
    if (enabled_changed && m_is_editing_existing)
    {
        m_template_controller.set_template_enabled(m_template_name, m_enabled);
    }
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Enable/disable this template for theme application");
    
    ImGui::PopStyleVar();
    
    ImGui::Spacing();
    
    ImGui::AlignTextToFramePadding();
    ImGui::Text("Name:");
    ImGui::SameLine(80);
    ImGui::SetNextItemWidth(180.0f);
    char name_buf[256] = {0};
    snprintf(name_buf, sizeof(name_buf), "%s", m_template_name.c_str());
    if (ImGui::InputText("##template_name", name_buf, sizeof(name_buf)))
    {
        m_template_name = name_buf;
        if (!m_template_name.empty())
        {
            m_validation_error = "";
        }
    }
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Unique name for this template");

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Input:");
    ImGui::SameLine(80);
    ImGui::SetNextItemWidth(-120.0f);
    char input_path_buf[512] = {0};
    snprintf(input_path_buf, sizeof(input_path_buf), "%s", m_input_path.c_str());
    if (ImGui::InputTextWithHint("##input_path", "Path to template file...", 
                                  input_path_buf, sizeof(input_path_buf)))
    {
        m_input_path = input_path_buf;
        if (!m_input_path.empty())
        {
            m_validation_error = "";
        }
        if (m_is_editing_existing)
        {
            m_template_controller.set_template_input_path(m_template_name, m_input_path);
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Browse##input"))
    {
        std::string selected_path = file_dialogs::open_file_dialog("Select Template File", m_input_path);
        if (!selected_path.empty()) {
            m_input_path = selected_path;
            if (m_is_editing_existing) {
                m_template_controller.set_template_input_path(m_template_name, m_input_path);
            }
            m_validation_error = "";
        }
    }
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Path where the template source file is stored");

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Output:");
    ImGui::SameLine(80);
    ImGui::SetNextItemWidth(-120.0f);
    char path_buf[512] = {0};
    snprintf(path_buf, sizeof(path_buf), "%s", m_output_path.c_str());
    if (ImGui::InputTextWithHint("##output_path", "Path for generated config...", 
                                 path_buf, sizeof(path_buf)))
    {
        m_output_path = path_buf;
        if (!m_output_path.empty())
        {
            m_validation_error = "";
        }
        if (m_is_editing_existing)
        {
            m_template_controller.set_template_output_path(m_template_name, m_output_path);
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Browse##output"))
    {
        std::string selected_path = file_dialogs::save_file_dialog("Select Output File", m_output_path);
        if (!selected_path.empty()) {
            m_output_path = selected_path;
            if (m_is_editing_existing) {
                m_template_controller.set_template_output_path(m_template_name, m_output_path);
            }
            m_validation_error = "";
        }
    }
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Path where the processed config will be written");

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Reload:");
    ImGui::SameLine(80);
    ImGui::SetNextItemWidth(-FLT_MIN);
    char reload_buf[512] = {0};
    snprintf(reload_buf, sizeof(reload_buf), "%s", m_reload_command.c_str());
    if (ImGui::InputTextWithHint("##reload_cmd", "Command to reload app (optional)...",
                                 reload_buf, sizeof(reload_buf)))
    {
        m_reload_command = reload_buf;
        if (m_is_editing_existing)
        {
            m_template_controller.set_template_reload_command(m_template_name, m_reload_command);
        }
    }
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Shell command to run after applying theme (e.g., 'pkill -USR1 kitty')");

    if (!m_validation_error.empty())
    {
        ImGui::Spacing();
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.4f, 0.4f, 1.0f));
        ImGui::TextWrapped("%s", m_validation_error.c_str());
        ImGui::PopStyleColor();
    }
}

void template_editor::render_editor()
{
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 4));
    
    if (!m_is_editing_existing)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 0.8f, 0.4f, 1.0f));
        ImGui::Text("  New Template");
        ImGui::PopStyleColor();
    }
    else
    {
        ImGui::Text("  %s", m_template_name.c_str());
        auto trim_right = [](const std::string &s) -> std::string {
            size_t end = s.find_last_not_of("\r\n");
            return (end == std::string::npos) ? "" : s.substr(0, end + 1);
        };

        std::string current_content = trim_right(m_editor.GetText());
        std::string saved_content = trim_right(m_saved_content);

        m_has_unsaved_changes = (current_content != saved_content);
        if (m_has_unsaved_changes)
        {
            ImGui::SameLine();
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.6f, 0.2f, 1.0f));
            ImGui::Text("(unsaved)");
            ImGui::PopStyleColor();
        }
    }
        
    ImGui::PopStyleVar();
    ImGui::Separator();

    bool consume_keys = false;
    
    if (m_show_autocomplete && ImGui::IsKeyPressed(ImGuiKey_Escape, false))
    {
        m_show_autocomplete = false;
        m_autocomplete_dismissed = true;
        
        m_dismiss_position = m_editor.GetCursorPosition();
        
        std::string line = m_editor.GetCurrentLineText();
        m_dismiss_brace_pos = -1;
        for (int i = m_dismiss_position.mColumn - 1; i >= 0; --i)
        {
            if (i < (int)line.length() && line[i] == '{')
            {
                m_dismiss_brace_pos = i;
                break;
            }
        }
        
        consume_keys = true;
    }
    else if (m_show_autocomplete && !m_autocomplete_suggestions.empty())
    {
        int max_visible = std::min((int)m_autocomplete_suggestions.size(), 8);
        
        if (ImGui::IsKeyPressed(ImGuiKey_DownArrow, false))
        {
            m_autocomplete_selected = (m_autocomplete_selected + 1) % max_visible;
            consume_keys = true;
        }
        else if (ImGui::IsKeyPressed(ImGuiKey_UpArrow, false))
        {
            m_autocomplete_selected = (m_autocomplete_selected - 1 + max_visible) % max_visible;
            consume_keys = true;
        }
        else if (ImGui::IsKeyPressed(ImGuiKey_Tab, false) || 
                 ImGui::IsKeyPressed(ImGuiKey_Enter, false))
        {
            auto start = m_autocomplete_start_pos;
            auto end = m_editor.GetCursorPosition();
            m_editor.SetSelection(start, end);
            m_editor.Delete();
            m_editor.InsertText(m_autocomplete_suggestions[m_autocomplete_selected] + "}");
            m_show_autocomplete = false;
            m_autocomplete_dismissed = false; 
            consume_keys = true;
        }
    }
    
    if (consume_keys)
    {
        m_editor.SetHandleKeyboardInputs(false);
    }

    ImVec2 editor_pos = ImGui::GetCursorScreenPos();
    
    m_editor.Render("##TemplateEditor", ImVec2(0, 0), true);
    
    if (consume_keys)
    {
        m_editor.SetHandleKeyboardInputs(true);
    }
    
    update_autocomplete_suggestions();
    render_autocomplete(editor_pos);
}

void template_editor::render_template_list()
{
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 6));
    
    ImGui::Text("Templates");
    ImGui::SameLine(ImGui::GetContentRegionAvail().x - 20);
    ImGui::TextDisabled("(%d)", (int)m_template_controller.templates().size());
    ImGui::Separator();

    if (!m_is_editing_existing)
    {
        ImVec4 success_color = palette_utils::get_color(m_current_palette, "success", "accent");
        ImVec4 success_bg = ImVec4(success_color.x, success_color.y, success_color.z, 0.5f);
        ImGui::PushStyleColor(ImGuiCol_Text, success_color);
        ImGui::PushStyleColor(ImGuiCol_Header, success_bg);
        ImGui::Selectable("+ New Template", true);
        ImGui::PopStyleColor(2);
        ImGui::Separator();
    }

    const auto &templates = m_template_controller.templates();

    for (const auto &[key, tmpl] : templates)
    {
        const bool selected = (m_template_name == key && m_is_editing_existing);
        
        if (!tmpl.enabled())
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
        }
        
        if (ImGui::Selectable(key.c_str(), selected))
        {
            load_template(key);
        }
        
        if (!tmpl.enabled())
        {
            ImGui::PopStyleColor();
        }
        
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::Text("Template: %s", key.c_str());
            ImGui::Separator();
            ImGui::Text("Status: %s", tmpl.enabled() ? "Enabled" : "Disabled");
            if (!tmpl.output_path().empty())
                ImGui::Text("Output: %s", tmpl.output_path().c_str());
            ImGui::EndTooltip();
        }
    }
    
    ImGui::PopStyleVar();
}

bool template_editor::is_valid_path(const std::string &path)
{
    const std::string invalid_chars = "<>|\"";
    for (const char c : invalid_chars)
    {
        if (path.find(c) != std::string::npos)
        {
            return false;
        }
    }

    if (path.find_first_not_of(" \t\n\r./\\") == std::string::npos)
    {
        return false;
    }

    try
    {
        std::filesystem::path fs_path(path);

        const auto parent = fs_path.parent_path();

        if (parent.empty())
        {
            return false;
        }

        if (!parent.empty() && !std::filesystem::exists(parent))
        {
            if (parent.string().find_first_of("<>|\"") != std::string::npos)
            {
                return false;
            }
        }

        const auto filename = fs_path.filename().string();
        if (filename.empty() || filename == "." || filename == "..")
        {
            return false;
        }

        return true;
    }
    catch (...)
    {
        return false;
    }
}

void template_editor::save_template()
{
    std::string trimmed_name = m_template_name;
    trimmed_name.erase(0, trimmed_name.find_first_not_of(" \t\n\r"));
    trimmed_name.erase(trimmed_name.find_last_not_of(" \t\n\r") + 1);

    std::string trimmed_input_path = m_input_path;
    trimmed_input_path.erase(0, trimmed_input_path.find_first_not_of(" \t\n\r"));
    trimmed_input_path.erase(trimmed_input_path.find_last_not_of(" \t\n\r") + 1);

    std::string trimmed_path = m_output_path;
    trimmed_path.erase(0, trimmed_path.find_first_not_of(" \t\n\r"));
    trimmed_path.erase(trimmed_path.find_last_not_of(" \t\n\r") + 1);

    if (trimmed_name.empty())
    {
        m_validation_error = "Error: Template name cannot be empty!";
        return;
    }

    if (trimmed_input_path.empty())
    {
        m_validation_error = "Error: Input path cannot be empty!";
        return;
    }

    if (trimmed_path.empty())
    {
        m_validation_error = "Error: Output path cannot be empty!";
        return;
    }

    if (!is_valid_path(trimmed_path))
    {
        m_validation_error =
            "Error: Output path is invalid! Must be a valid file path with directory.";
        return;
    }

    m_validation_error = "";

    auto &cfg = clrsync::core::config::instance();

    std::filesystem::path template_file = clrsync::core::normalize_path(trimmed_input_path);
    
    auto parent_dir = template_file.parent_path();
    if (!parent_dir.empty() && !std::filesystem::exists(parent_dir))
    {
        try
        {
            std::filesystem::create_directories(parent_dir);
        }
        catch (const std::exception& e)
        {
            m_validation_error = "Error: Could not create directory for input path";
            return;
        }
    }

    std::string template_content = m_editor.GetText();

    std::ofstream out(template_file);
    if (!out.is_open())
    {
        m_validation_error = "Failed to write template file";
        return;
    }
    
    out << template_content;
    out.close();

    clrsync::core::theme_template tmpl(trimmed_name, template_file.string(), trimmed_path);
    tmpl.set_reload_command(m_reload_command);
    tmpl.set_enabled(m_enabled);

    auto result = cfg.update_template(trimmed_name, tmpl);
    if (!result)
    {
        m_validation_error = "Error saving template: " + result.error().description();
        return;
    }

    m_template_name = trimmed_name;
    m_input_path = trimmed_input_path;
    m_output_path = trimmed_path;
    m_is_editing_existing = true;
    m_saved_content = m_editor.GetText();
    m_has_unsaved_changes = false;

    refresh_templates();
}

void template_editor::load_template(const std::string &name)
{
    const auto &templates = m_template_controller.templates();
    auto it = templates.find(name);

    if (it != templates.end())
    {
        const auto &tmpl = it->second;
        m_template_name = name;
        m_input_path = tmpl.template_path();
        m_output_path = tmpl.output_path();
        m_reload_command = tmpl.reload_command();
        m_enabled = tmpl.enabled();
        m_is_editing_existing = true;
        m_validation_error = "";

        std::ifstream in(tmpl.template_path());
        if (in.is_open())
        {
            std::string content;
            std::string line;
            while (std::getline(in, line))
            {
                content += line + "\n";
            }
            in.close();

            m_editor.SetText(content);
            m_saved_content = content;
            m_has_unsaved_changes = false;
        }
        else
        {
            m_validation_error = "Error loading template: Failed to open file";
        }
    }
}

void template_editor::new_template()
{
    m_template_name = "new_template";
    std::string default_content =
        "# Enter your template here\n# Use {color_key} for color variables\n# "
        "Examples: {color.hex}, {color.rgb}, {color.r}\n\n";
    m_editor.SetText(default_content);
    m_saved_content = default_content;
    m_input_path = "";
    m_output_path = "";
    m_reload_command = "";
    m_enabled = true;
    m_is_editing_existing = false;
    m_validation_error = "";
    m_has_unsaved_changes = false;
}

void template_editor::delete_template()
{
    if (!m_is_editing_existing || m_template_name.empty())
        return;
    
    m_show_delete_confirmation = true;
}

void template_editor::refresh_templates()
{
    m_template_controller.refresh();
}