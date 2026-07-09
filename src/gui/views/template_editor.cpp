#include "template_editor.hpp"
#include "core/common/utils.hpp"
#include "core/config/config.hpp"
#include "core/palette/color_keys.hpp"
#include "core/theme/theme_template.hpp"
#include "gui/layout/ui_layout.hpp"
#include "gui/theme/app_theme.hpp"
#include "gui/ui_manager.hpp"
#include "gui/widgets/colors.hpp"
#include "gui/widgets/dialogs.hpp"
#include "imgui.h"
#include <algorithm>
#include <cctype>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <ranges>

namespace
{
const std::vector<std::string> COLOR_FORMATS = {
    "hex", "hex_stripped", "hexa", "hexa_stripped", "r", "g", "b", "a", "rgb", "rgba", "h", "s",
    "l",   "hsl",          "hsla"};
}

template_editor::template_editor(clrsync::gui::ui_manager *ui_mgr) : m_ui_manager(ui_mgr)
{
    m_control_state.name = "new_template";

    update_autocomplete_colors();

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

    setup_callbacks();
}

void template_editor::setup_callbacks()
{
    m_callbacks.on_new = [this]() { new_template(); };
    m_callbacks.on_save = [this]() { save_template(); };
    m_callbacks.on_delete = [this]() { delete_template(); };
    m_callbacks.on_enabled_changed = [this](bool enabled) {
        m_template_controller.set_template_enabled(m_control_state.name, enabled);
    };
    m_callbacks.on_browse_input = [this](const std::string &path) -> std::string {
        return m_ui_manager->open_file_dialog("Select Template File", path);
    };
    m_callbacks.on_browse_output = [this](const std::string &path) -> std::string {
        return m_ui_manager->save_file_dialog("Select Output File", path);
    };
    m_callbacks.on_input_path_changed = [this](const std::string &path) {
        m_template_controller.set_template_input_path(m_control_state.name, path);
    };
    m_callbacks.on_output_path_changed = [this](const std::string &path) {
        m_template_controller.set_template_output_path(m_control_state.name, path);
    };
    m_callbacks.on_reload_command_changed = [this](const std::string &cmd) {
        m_template_controller.set_template_reload_command(m_control_state.name, cmd);
    };
}

void template_editor::insert_token(const std::string &key)
{
    m_editor.InsertText("{" + key + "}");
}

void template_editor::update_autocomplete_colors()
{
    const auto &t = clrsync::gui::theme::current_theme();
    m_autocomplete_bg_color = t.autocomplete_bg();
    m_autocomplete_border_color = t.autocomplete_border();
    m_autocomplete_selected_color = t.autocomplete_selected();
    m_autocomplete_text_color = t.autocomplete_text();
    m_autocomplete_selected_text_color = t.autocomplete_selected_text();
    m_autocomplete_dim_text_color = t.autocomplete_dim_text();
}

void template_editor::apply_current_palette(const clrsync::core::palette &pal)
{
    m_current_palette = pal;
    auto colors = pal.colors();
    if (colors.empty())
        return;

    using namespace clrsync::gui::theme;

    auto get_color_u32 = [&](const std::string &key, const std::string &fallback = "") -> uint32_t {
        return clrsync::gui::widgets::palette_color_u32(pal, key, fallback);
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

    update_autocomplete_colors();
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

    m_autocomplete_end_brace_pos = -1;
    for (int i = col; i < (int)line.length(); ++i)
    {
        if (line[i] == '}')
        {
            m_autocomplete_end_brace_pos = i;
            break;
        }
        else if (line[i] == '{' || line[i] == ' ' || line[i] == '\t')
        {
            break;
        }
    }

    if (m_autocomplete_dismissed)
    {
        bool should_reset_dismissal = false;

        if (cursor.mLine != m_dismiss_position.mLine || brace_pos != m_dismiss_brace_pos ||
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
                if (format_prefix.empty() || fmt.find(format_prefix) == 0)
                {
                    m_autocomplete_suggestions.push_back(color_key + "." + fmt);
                }
            }
        }
    }
    else
    {
        std::vector<std::pair<std::string, int>> scored_suggestions;

        for (size_t i = 0; i < clrsync::core::NUM_COLOR_KEYS; ++i)
        {
            std::string key = clrsync::core::COLOR_KEYS[i];

            if (m_autocomplete_prefix.empty())
            {
                scored_suggestions.push_back({key, 0});
            }
            else
            {
                if (key.find(m_autocomplete_prefix) == 0)
                {
                    scored_suggestions.push_back({key, 100});
                }
                else
                {
                    size_t pos = key.find("_" + m_autocomplete_prefix);
                    if (pos != std::string::npos)
                    {
                        scored_suggestions.push_back({key, 50});
                    }
                    else if (key.find(m_autocomplete_prefix) != std::string::npos)
                    {
                        scored_suggestions.push_back({key, 25});
                    }
                }
            }
        }

        std::sort(scored_suggestions.begin(), scored_suggestions.end(),
                  [](const auto &a, const auto &b) {
                      if (a.second != b.second)
                          return a.second > b.second;
                      return a.first < b.first;
                  });

        for (const auto &[key, score] : scored_suggestions)
        {
            m_autocomplete_suggestions.push_back(key);
        }
    }

    m_show_autocomplete = !m_autocomplete_suggestions.empty();
    if (m_show_autocomplete && m_autocomplete_selected >= (int)m_autocomplete_suggestions.size())
    {
        m_autocomplete_selected = 0;
    }
}

void template_editor::render_autocomplete(const ImVec2 &editor_pos)
{
    if (!m_show_autocomplete || m_autocomplete_suggestions.empty())
        return;

    float line_height = ImGui::GetTextLineHeightWithSpacing();
    float char_width = ImGui::GetFontSize() * 0.5f;
    auto cursor = m_editor.GetCursorPosition();

    const float line_number_width = 50.0f;

    ImVec2 popup_pos;
    popup_pos.x =
        editor_pos.x + line_number_width + (m_autocomplete_start_pos.mColumn * char_width);
    popup_pos.y = editor_pos.y + ((cursor.mLine + 1) * line_height);

    ImGui::SetNextWindowPos(popup_pos, ImGuiCond_Always);

    float max_width = 350.0f;
    float min_width = 250.0f;
    ImGui::SetNextWindowSize(ImVec2(max_width, 0));

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                             ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings |
                             ImGuiWindowFlags_NoFocusOnAppearing |
                             ImGuiWindowFlags_AlwaysAutoResize;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 3));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, m_autocomplete_bg_color);
    ImGui::PushStyleColor(ImGuiCol_Border, m_autocomplete_border_color);

    if (ImGui::Begin("##autocomplete", nullptr, flags))
    {
        ImGui::PushStyleColor(ImGuiCol_Text, m_autocomplete_dim_text_color);
        if (m_autocomplete_prefix.find('.') != std::string::npos)
            ImGui::Text("Color Formats");
        else
            ImGui::Text("Color Keys (%d)", (int)m_autocomplete_suggestions.size());
        ImGui::PopStyleColor();
        ImGui::Separator();

        int max_visible = std::min((int)m_autocomplete_suggestions.size(), 12);

        if (m_autocomplete_selected >= max_visible)
        {
            m_autocomplete_selected = max_visible - 1;
        }

        for (int i = 0; i < max_visible; ++i)
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

            std::string color_key = suggestion;
            size_t dot_pos = suggestion.find('.');
            if (dot_pos != std::string::npos)
            {
                color_key = suggestion.substr(0, dot_pos);
            }

            bool has_color = false;
            clrsync::core::color palette_color;
            if (m_current_palette.colors().count(color_key) > 0)
            {
                has_color = true;
                palette_color = m_current_palette.colors().at(color_key);
            }

            if (ImGui::Selectable(display_text.c_str(), is_selected, ImGuiSelectableFlags_None,
                                  ImVec2(0, 0)))
            {
                auto start = m_autocomplete_start_pos;
                auto cursor_pos = m_editor.GetCursorPosition();

                TextEditor::Coordinates end;
                if (m_autocomplete_end_brace_pos >= 0)
                {
                    end = TextEditor::Coordinates(cursor_pos.mLine, m_autocomplete_end_brace_pos);
                }
                else
                {
                    end = cursor_pos;
                }

                std::string insert_text = suggestion;
                if (m_autocomplete_end_brace_pos < 0)
                {
                    insert_text += "}";
                }

                const char *old_clipboard = ImGui::GetClipboardText();
                std::string saved_clipboard = old_clipboard ? old_clipboard : "";

                ImGui::SetClipboardText(insert_text.c_str());

                m_editor.SetSelection(start, end);
                m_editor.Paste();

                ImGui::SetClipboardText(saved_clipboard.c_str());

                m_show_autocomplete = false;
                m_autocomplete_dismissed = false;
            }

            if (has_color)
            {
                ImVec2 item_min = ImGui::GetItemRectMin();
                ImVec2 item_max = ImGui::GetItemRectMax();

                const float preview_size = ImGui::GetTextLineHeight() * 0.7f;
                const float padding = 4.0f;

                ImVec2 preview_pos;
                preview_pos.x = item_max.x - preview_size - padding;
                preview_pos.y = item_min.y + (item_max.y - item_min.y - preview_size) * 0.5f;

                ImDrawList *draw_list = ImGui::GetWindowDrawList();

                auto rgba = palette_color.to_rgba();
                ImU32 color_u32 = IM_COL32(rgba.r, rgba.g, rgba.b, rgba.a);

                float r = rgba.r / 255.0f;
                float g = rgba.g / 255.0f;
                float b = rgba.b / 255.0f;

                r = (r <= 0.03928f) ? r / 12.92f : std::pow((r + 0.055f) / 1.055f, 2.4f);
                g = (g <= 0.03928f) ? g / 12.92f : std::pow((g + 0.055f) / 1.055f, 2.4f);
                b = (b <= 0.03928f) ? b / 12.92f : std::pow((b + 0.055f) / 1.055f, 2.4f);

                float luminance = 0.2126f * r + 0.7152f * g + 0.0722f * b;

                ImVec4 bg_color =
                    is_selected ? m_autocomplete_selected_color : m_autocomplete_bg_color;
                float bg_r = bg_color.x;
                float bg_g = bg_color.y;
                float bg_b = bg_color.z;

                bg_r =
                    (bg_r <= 0.03928f) ? bg_r / 12.92f : std::pow((bg_r + 0.055f) / 1.055f, 2.4f);
                bg_g =
                    (bg_g <= 0.03928f) ? bg_g / 12.92f : std::pow((bg_g + 0.055f) / 1.055f, 2.4f);
                bg_b =
                    (bg_b <= 0.03928f) ? bg_b / 12.92f : std::pow((bg_b + 0.055f) / 1.055f, 2.4f);

                float bg_luminance = 0.2126f * bg_r + 0.7152f * bg_g + 0.0722f * bg_b;

                float contrast = (std::max(luminance, bg_luminance) + 0.05f) /
                                 (std::min(luminance, bg_luminance) + 0.05f);

                if (contrast < 2.0f)
                {
                    ImU32 contrast_bg = (bg_luminance > 0.5f) ? IM_COL32(0, 0, 0, 180)
                                                              : IM_COL32(255, 255, 255, 180);
                    const float bg_padding = 2.0f;
                    draw_list->AddRectFilled(
                        ImVec2(preview_pos.x - bg_padding, preview_pos.y - bg_padding),
                        ImVec2(preview_pos.x + preview_size + bg_padding,
                               preview_pos.y + preview_size + bg_padding),
                        contrast_bg, 2.0f);
                }

                draw_list->AddRectFilled(
                    preview_pos, ImVec2(preview_pos.x + preview_size, preview_pos.y + preview_size),
                    color_u32, 2.0f);

                ImU32 border_color = IM_COL32(255, 255, 255, 60);
                draw_list->AddRect(
                    preview_pos, ImVec2(preview_pos.x + preview_size, preview_pos.y + preview_size),
                    border_color, 2.0f, 0, 1.0f);
            }

            ImGui::PopStyleColor(3);

            if (is_selected && ImGui::IsWindowAppearing())
            {
                ImGui::SetScrollHereY();
            }
        }

        if (m_autocomplete_suggestions.size() > 12)
        {
            ImGui::Separator();
            ImGui::PushStyleColor(ImGuiCol_Text, m_autocomplete_dim_text_color);
            ImGui::Text("  +%d more (keep typing to filter)",
                        (int)m_autocomplete_suggestions.size() - 12);
            ImGui::PopStyleColor();
        }

        ImGui::Separator();
        ImGui::PushStyleColor(ImGuiCol_Text, m_autocomplete_dim_text_color);
        ImGui::Text("  Tab/Enter: accept  |  Esc: dismiss  |  \u2191\u2193: navigate");
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
    const float left_panel_width =
        std::max(clrsync::gui::layout::TEMPLATE_LIST_MIN_WIDTH,
                 panel_width * clrsync::gui::layout::TEMPLATE_LIST_WIDTH_RATIO);
    const float panel_gap = clrsync::gui::layout::BUTTON_SPACING;
    const float right_panel_width = panel_width - left_panel_width - panel_gap;

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

    clrsync::gui::widgets::delete_confirmation_dialog(
        "Delete Template?", m_control_state.name, "template", m_current_palette, [this]() {
            bool success = m_template_controller.remove_template(m_control_state.name);
            if (success)
            {
                new_template();
                refresh_templates();
            }
            else
            {
                m_validation.set("Failed to delete template");
            }
        });

    ImGui::End();
}

void template_editor::render_controls()
{
    if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) &&
        ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_S))
    {
        save_template();
    }

    m_controls.render(m_control_state, m_callbacks, m_current_palette, m_validation);
}

void template_editor::render_editor_toolbar()
{
    if (ImGui::Button("Insert variable"))
        ImGui::OpenPopup("##insert_var");

    if (ImGui::BeginPopup("##insert_var"))
    {
        ImGui::SetNextItemWidth(240.0f);
        ImGui::InputTextWithHint("##var_filter", "Filter colors...", m_var_filter,
                                 sizeof(m_var_filter));

        std::string filter = m_var_filter;
        std::transform(filter.begin(), filter.end(), filter.begin(),
                       [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

        ImGui::BeginChild("##var_list", ImVec2(260, 280), true);
        for (size_t i = 0; i < clrsync::core::NUM_COLOR_KEYS; ++i)
        {
            const char *key = clrsync::core::COLOR_KEYS[i];

            if (!filter.empty())
            {
                std::string lower = key;
                std::transform(lower.begin(), lower.end(), lower.begin(),
                               [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
                if (lower.find(filter) == std::string::npos)
                    continue;
            }

            if (ImGui::Selectable(key))
            {
                insert_token(key);
                ImGui::CloseCurrentPopup();
            }
        }
        ImGui::EndChild();
        ImGui::EndPopup();
    }
}

void template_editor::render_editor()
{
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 4));

    if (!m_control_state.is_editing_existing)
    {
        ImVec4 success_color =
            clrsync::gui::widgets::palette_color(m_current_palette, "success", "accent");
        ImGui::PushStyleColor(ImGuiCol_Text, success_color);
        ImGui::Text("  New Template");
        ImGui::PopStyleColor();
    }
    else
    {
        ImGui::Text("  %s", m_control_state.name.c_str());
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
            ImVec4 warning_color =
                clrsync::gui::widgets::palette_color(m_current_palette, "warning", "accent");
            ImGui::PushStyleColor(ImGuiCol_Text, warning_color);
            ImGui::Text("(unsaved)");
            ImGui::PopStyleColor();
        }
    }

    ImGui::PopStyleVar();

    render_editor_toolbar();

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
        int max_visible = std::min((int)m_autocomplete_suggestions.size(), 12);

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
            if (m_autocomplete_selected >= 0 &&
                m_autocomplete_selected < (int)m_autocomplete_suggestions.size())
            {
                auto start = m_autocomplete_start_pos;
                auto cursor_pos = m_editor.GetCursorPosition();

                TextEditor::Coordinates end;
                if (m_autocomplete_end_brace_pos >= 0)
                {
                    end = TextEditor::Coordinates(cursor_pos.mLine, m_autocomplete_end_brace_pos);
                }
                else
                {
                    end = cursor_pos;
                }

                std::string insert_text = m_autocomplete_suggestions[m_autocomplete_selected];
                if (m_autocomplete_end_brace_pos < 0)
                {
                    insert_text += "}";
                }

                const char *old_clipboard = ImGui::GetClipboardText();
                std::string saved_clipboard = old_clipboard ? old_clipboard : "";

                ImGui::SetClipboardText(insert_text.c_str());

                m_editor.SetSelection(start, end);
                m_editor.Paste();

                ImGui::SetClipboardText(saved_clipboard.c_str());

                m_show_autocomplete = false;
                m_autocomplete_dismissed = false;
            }
            consume_keys = true;
        }
    }

    if (consume_keys)
    {
        m_editor.SetHandleKeyboardInputs(false);
    }

    ImVec2 editor_pos = ImGui::GetCursorScreenPos();
    const float editor_height = ImGui::GetContentRegionAvail().y;

    ImGui::BeginChild("TemplateEditorScroll", ImVec2(0, editor_height), true,
                      ImGuiWindowFlags_HorizontalScrollbar);
    m_editor.SetImGuiChildIgnored(true);
    m_editor.Render("##TemplateEditor", ImVec2(0, 0), false);
    m_editor.SetImGuiChildIgnored(false);
    ImGui::EndChild();

    if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && m_editor.HasSelection())
    {
        std::string selected_text = m_editor.GetSelectedText();

        if (!selected_text.empty())
        {
            bool starts_with_brace = selected_text.front() == '{';
            bool ends_with_brace = selected_text.back() == '}';

            if (starts_with_brace || ends_with_brace)
            {
                auto cursor = m_editor.GetCursorPosition();
                std::string line = m_editor.GetCurrentLineText();

                int brace_start = -1;
                int brace_end = -1;

                for (int i = cursor.mColumn - 1; i >= 0; --i)
                {
                    if (i < (int)line.length() && line[i] == '{')
                    {
                        brace_start = i;
                        break;
                    }
                }

                if (brace_start >= 0)
                {
                    for (int i = brace_start + 1; i < (int)line.length(); ++i)
                    {
                        if (line[i] == '}')
                        {
                            brace_end = i;
                            break;
                        }
                    }
                }

                if (brace_start >= 0 && brace_end > brace_start + 1)
                {
                    TextEditor::Coordinates sel_start(cursor.mLine, brace_start + 1);
                    TextEditor::Coordinates sel_end(cursor.mLine, brace_end);
                    m_editor.SetSelection(sel_start, sel_end);
                }
            }
        }
    }

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
    char count_label[32];
    const int template_count = static_cast<int>(m_template_controller.templates().size());
    snprintf(count_label, sizeof(count_label), "(%d)", template_count);
    const ImVec2 count_size = ImGui::CalcTextSize(count_label);
    const float count_x =
        ImGui::GetWindowContentRegionMax().x - count_size.x - ImGui::GetStyle().ItemInnerSpacing.x;
    ImGui::SameLine(count_x);
    ImGui::TextDisabled("%s", count_label);
    ImGui::Separator();

    if (!m_control_state.is_editing_existing)
    {
        ImVec4 success_color =
            clrsync::gui::widgets::palette_color(m_current_palette, "success", "accent");
        ImVec4 success_bg = ImVec4(success_color.x, success_color.y, success_color.z, 0.5f);
        ImGui::PushStyleColor(ImGuiCol_Text, success_color);
        ImGui::PushStyleColor(ImGuiCol_Header, success_bg);
        ImGui::Selectable("New Template", true);
        ImGui::PopStyleColor(2);
        ImGui::Separator();
    }

    const auto &templates = m_template_controller.templates();

    for (const auto &[key, tmpl] : templates)
    {
        const bool selected = (m_control_state.name == key && m_control_state.is_editing_existing);

        if (!tmpl.enabled())
        {
            ImVec4 disabled_color = clrsync::gui::widgets::palette_color(
                m_current_palette, "on_surface_variant", "editor_inactive");
            ImGui::PushStyleColor(ImGuiCol_Text, disabled_color);
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
    std::string trimmed_name = m_control_state.name;
    trimmed_name.erase(0, trimmed_name.find_first_not_of(" \t\n\r"));
    trimmed_name.erase(trimmed_name.find_last_not_of(" \t\n\r") + 1);

    std::string trimmed_input_path = m_control_state.input_path;
    trimmed_input_path.erase(0, trimmed_input_path.find_first_not_of(" \t\n\r"));
    trimmed_input_path.erase(trimmed_input_path.find_last_not_of(" \t\n\r") + 1);

    std::string trimmed_path = m_control_state.output_path;
    trimmed_path.erase(0, trimmed_path.find_first_not_of(" \t\n\r"));
    trimmed_path.erase(trimmed_path.find_last_not_of(" \t\n\r") + 1);

    if (trimmed_name.empty())
    {
        m_validation.set("Error: Template name cannot be empty!");
        return;
    }

    if (trimmed_input_path.empty())
    {
        m_validation.set("Error: Input path cannot be empty!");
        return;
    }

    if (trimmed_path.empty())
    {
        m_validation.set("Error: Output path cannot be empty!");
        return;
    }

    if (!is_valid_path(trimmed_path))
    {
        m_validation.set(
            "Error: Output path is invalid! Must be a valid file path with directory.");
        return;
    }

    m_validation.clear();

    auto &cfg = clrsync::core::config::instance();

    std::filesystem::path template_file = clrsync::core::normalize_path(trimmed_input_path);

    auto parent_dir = template_file.parent_path();
    if (!parent_dir.empty() && !std::filesystem::exists(parent_dir))
    {
        try
        {
            std::filesystem::create_directories(parent_dir);
        }
        catch (const std::exception &e)
        {
            m_validation.set("Error: Could not create directory for input path");
            return;
        }
    }

    std::string template_content = m_editor.GetText();

    std::ofstream out(template_file);
    if (!out.is_open())
    {
        m_validation.set("Failed to write template file");
        return;
    }

    out << template_content;
    out.close();

    clrsync::core::theme_template tmpl(trimmed_name, template_file.string(), trimmed_path);
    tmpl.set_reload_command(m_control_state.reload_command);
    tmpl.set_enabled(m_control_state.enabled);

    auto result = cfg.update_template(trimmed_name, tmpl);
    if (!result)
    {
        m_validation.set("Error saving template: " + result.error().description());
        return;
    }

    m_control_state.name = trimmed_name;
    m_control_state.input_path = trimmed_input_path;
    m_control_state.output_path = trimmed_path;
    m_control_state.is_editing_existing = true;
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
        m_control_state.name = name;
        m_control_state.input_path = tmpl.template_path();
        m_control_state.output_path = tmpl.output_path();
        m_control_state.reload_command = tmpl.reload_command();
        m_control_state.enabled = tmpl.enabled();
        m_control_state.is_editing_existing = true;
        m_validation.clear();

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
            m_validation.set("Error loading template: Failed to open file");
        }
    }
}

void template_editor::new_template()
{
    m_control_state.name = "new_template";
    std::string default_content =
        "# Enter your template here\n# Use {color_key} for color variables\n# "
        "Examples: {color.hex}, {color.rgb}, {color.r}\n\n";
    m_editor.SetText(default_content);
    m_saved_content = default_content;
    m_control_state.input_path = "";
    m_control_state.output_path = "";
    m_control_state.reload_command = "";
    m_control_state.enabled = true;
    m_control_state.is_editing_existing = false;
    m_validation.clear();
    m_has_unsaved_changes = false;
}

void template_editor::delete_template()
{
    if (!m_control_state.is_editing_existing || m_control_state.name.empty())
        return;

    m_show_delete_confirmation = true;
}

void template_editor::refresh_templates()
{
    m_template_controller.refresh();
}
