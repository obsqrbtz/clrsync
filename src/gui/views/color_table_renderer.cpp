#include "gui/views/color_table_renderer.hpp"
#include "gui/layout/ui_layout.hpp"
#include "gui/widgets/colors.hpp"
#include "imgui.h"
#include <algorithm>
#include <cctype>
#include <vector>

bool color_table_renderer::matches_filter(const std::string &name) const
{
    if (m_filter_text[0] == '\0')
        return true;

    std::string filter_lower = m_filter_text;
    std::string name_lower = name;

    std::transform(filter_lower.begin(), filter_lower.end(), filter_lower.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    std::transform(name_lower.begin(), name_lower.end(), name_lower.begin(),
                   [](unsigned char c) { return std::tolower(c); });

    return name_lower.find(filter_lower) != std::string::npos;
}

void color_table_renderer::render_color_row(const std::string &name,
                                            const clrsync::core::palette &current,
                                            palette_controller &controller,
                                            const OnColorChangedCallback &on_changed,
                                            const OnInsertTokenCallback &on_insert_token)
{
    if (!matches_filter(name))
        return;

    const clrsync::core::color &col = current.get_color(name);

    ImGui::TableNextRow();

    ImGui::TableSetColumnIndex(0);
    const float key_col_width = ImGui::GetContentRegionAvail().x;

    ImVec4 text_color = clrsync::gui::widgets::palette_color(current, "info", "accent");
    ImGui::PushStyleColor(ImGuiCol_Text, text_color);
    const bool clicked = ImGui::Selectable(name.c_str(), false, 0, ImVec2(key_col_width, 0.0f));
    ImGui::PopStyleColor();

    const bool can_insert = static_cast<bool>(on_insert_token);

    if (ImGui::IsItemHovered())
    {
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
        if (can_insert)
            ImGui::SetTooltip("Click to insert {%s} into the template  ·  Right-click to copy",
                              name.c_str());
        else
            ImGui::SetTooltip("Click to copy: {%s.hex}", name.c_str());
    }

    if (clicked)
    {
        if (can_insert)
        {
            on_insert_token(name);
        }
        else
        {
            std::string template_var = "{" + name + ".hex}";
            ImGui::SetClipboardText(template_var.c_str());
        }
    }
    if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
    {
        std::string template_var = "{" + name + ".hex}";
        ImGui::SetClipboardText(template_var.c_str());
    }

    ImGui::TableSetColumnIndex(1);
    {
        std::string hex_str = col.to_hex_string();
        char buf[9];
        strncpy(buf, hex_str.c_str(), sizeof(buf));
        buf[8] = 0;

        ImGui::SetNextItemWidth(-FLT_MIN);
        if (ImGui::InputText(("##hex_" + name).c_str(), buf, sizeof(buf),
                             ImGuiInputTextFlags_CharsUppercase))
        {
            try
            {
                clrsync::core::color new_color;
                new_color.from_hex_string(buf);
                controller.set_color(name, new_color);
                if (on_changed)
                    on_changed();
            }
            catch (...)
            {
            }
        }
    }

    ImGui::TableSetColumnIndex(2);
    ImGui::PushID(name.c_str());
    float c[4] = {((col.hex() >> 24) & 0xFF) / 255.0f, ((col.hex() >> 16) & 0xFF) / 255.0f,
                  ((col.hex() >> 8) & 0xFF) / 255.0f, (col.hex() & 0xFF) / 255.0f};
    const ImVec4 color_vec(c[0], c[1], c[2], c[3]);
    const ImVec2 swatch_size(clrsync::gui::layout::COLOR_SWATCH_SIZE,
                             clrsync::gui::layout::COLOR_SWATCH_SIZE);
    const std::string popup_id = "##picker_" + name;
    const std::string button_id = "##swatch_" + name;

    if (ImGui::ColorButton(button_id.c_str(), color_vec,
                           ImGuiColorEditFlags_AlphaPreviewHalf | ImGuiColorEditFlags_NoTooltip,
                           swatch_size))
        ImGui::OpenPopup(popup_id.c_str());

    if (ImGui::BeginPopup(popup_id.c_str()))
    {
        if (ImGui::ColorPicker4(("##picker4_" + name).c_str(), c,
                                ImGuiColorEditFlags_AlphaBar |
                                    ImGuiColorEditFlags_AlphaPreviewHalf |
                                    ImGuiColorEditFlags_DisplayRGB))
        {
            uint32_t r = static_cast<uint32_t>(c[0] * 255.0f);
            uint32_t g = static_cast<uint32_t>(c[1] * 255.0f);
            uint32_t b = static_cast<uint32_t>(c[2] * 255.0f);
            uint32_t a = static_cast<uint32_t>(c[3] * 255.0f);
            uint32_t hex = (r << 24) | (g << 16) | (b << 8) | a;
            controller.set_color(name, clrsync::core::color(hex));
            if (on_changed)
                on_changed();
        }
        ImGui::EndPopup();
    }

    ImGui::PopID();
}

void color_table_renderer::render(const clrsync::core::palette &current,
                                  palette_controller &controller,
                                  const OnColorChangedCallback &on_changed,
                                  const OnInsertTokenCallback &on_insert_token)
{
    if (current.colors().empty())
    {
        ImVec4 warning_color = clrsync::gui::widgets::palette_color(current, "warning", "accent");
        ImGui::TextColored(warning_color, "No palette loaded");
        return;
    }

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 6));

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Filter:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(200);
    bool filter_changed = ImGui::InputTextWithHint("##color_filter", "Search colors...",
                                                   m_filter_text, sizeof(m_filter_text));

    if (m_filter_text[0] != '\0')
    {
        ImGui::SameLine();
        if (ImGui::SmallButton("X"))
        {
            m_filter_text[0] = '\0';
            filter_changed = true;
        }
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Clear filter");
    }

    ImGui::PopStyleVar();

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    auto draw_table = [&](const char *title, const char *id,
                          const std::vector<const char *> &keys) {
        bool has_matches = false;
        for (auto *k : keys)
        {
            if (matches_filter(k))
            {
                has_matches = true;
                break;
            }
        }

        if (!has_matches)
            return;

        ImGui::PushStyleColor(ImGuiCol_Text,
                              clrsync::gui::widgets::palette_color(current, "accent"));
        constexpr ImGuiTreeNodeFlags tree_flags =
            ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth |
            ImGuiTreeNodeFlags_DrawLinesFull | ImGuiTreeNodeFlags_Framed;
        bool header_open = ImGui::TreeNodeEx(title, tree_flags);
        ImGui::PopStyleColor();

        if (header_open)
        {
            if (ImGui::BeginTable(id, 3,
                                  ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg |
                                      ImGuiTableFlags_SizingFixedFit))
            {
                ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch, 0.0f, 2.0f);
                ImGui::TableSetupColumn("HEX", ImGuiTableColumnFlags_WidthFixed,
                                        clrsync::gui::layout::HEX_COLUMN_WIDTH);
                ImGui::TableSetupColumn("Color", ImGuiTableColumnFlags_WidthFixed,
                                        clrsync::gui::layout::COLOR_COLUMN_WIDTH);
                ImGui::TableHeadersRow();

                for (auto *k : keys)
                    render_color_row(k, current, controller, on_changed, on_insert_token);

                ImGui::EndTable();
            }
            ImGui::TreePop();
        }

        ImGui::Spacing();
    };

    draw_table("General UI", "##general_ui",
               {"background", "on_background", "surface", "on_surface", "surface_variant",
                "on_surface_variant", "border_focused", "border", "foreground", "cursor", "accent",
                "accent_secondary"});

    draw_table(
        "Semantic Colors", "##semantic",
        {"success", "info", "warning", "error", "on_success", "on_info", "on_warning", "on_error"});

    draw_table("Editor - Basic", "##editor_basic",
               {"editor_background", "editor_foreground", "editor_line_highlight",
                "editor_selection", "editor_selection_inactive", "editor_cursor",
                "editor_whitespace"});

    draw_table("Editor - Gutter", "##editor_gutter",
               {"editor_gutter_background", "editor_gutter_foreground", "editor_line_number",
                "editor_line_number_active"});

    draw_table("Editor - Syntax", "##editor_syntax",
               {"editor_comment",   "editor_string",          "editor_number",
                "editor_boolean",   "editor_keyword",         "editor_operator",
                "editor_function",  "editor_variable",        "editor_parameter",
                "editor_property",  "editor_constant",        "editor_type",
                "editor_class",     "editor_interface",       "editor_enum",
                "editor_namespace", "editor_attribute",       "editor_decorator",
                "editor_tag",       "editor_punctuation",     "editor_link",
                "editor_regex",     "editor_escape_character"});

    draw_table("Editor - Diagnostics", "##editor_diagnostics",
               {"editor_invalid", "editor_error", "editor_error_background", "editor_warning",
                "editor_warning_background", "editor_info", "editor_info_background", "editor_hint",
                "editor_hint_background"});

    draw_table("Editor - UI Elements", "##editor_ui",
               {"editor_active_line_border", "editor_indent_guide", "editor_indent_guide_active",
                "editor_bracket_match", "editor_search_match", "editor_search_match_active",
                "editor_find_range_highlight"});

    draw_table("Editor - Diff", "##editor_diff",
               {"editor_deleted", "editor_inserted", "editor_modified", "editor_ignored",
                "editor_folded_background"});

    draw_table("Terminal (Base16)", "##terminal",
               {"base00", "base01", "base02", "base03", "base04", "base05", "base06", "base07",
                "base08", "base09", "base0A", "base0B", "base0C", "base0D", "base0E", "base0F"});
}
