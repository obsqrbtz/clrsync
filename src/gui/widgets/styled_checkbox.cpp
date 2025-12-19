#include "styled_checkbox.hpp"
#include "colors.hpp"
#include "imgui.h"

namespace clrsync::gui::widgets
{

styled_checkbox::styled_checkbox() = default;

bool styled_checkbox::render(const std::string &label, bool *value, const core::palette &theme_palette, 
                            checkbox_style style)
{
    ImVec4 bg_color, hover_color, check_color;
    
    switch (style)
    {
        case checkbox_style::success:
            if (*value)
            {
                bg_color = palette_color(theme_palette, "success", "accent");
                bg_color.w = 0.5f;
                hover_color = ImVec4(bg_color.x * 1.2f, bg_color.y * 1.2f, bg_color.z * 1.2f, 0.6f);
                check_color = palette_color(theme_palette, "on_success", "on_surface");
            }
            else
            {
                bg_color = palette_color(theme_palette, "surface", "background");
                hover_color = palette_color(theme_palette, "surface_variant", "surface");
                check_color = palette_color(theme_palette, "on_surface", "foreground");
            }
            break;
            
        case checkbox_style::error:
            if (*value)
            {
                bg_color = palette_color(theme_palette, "error", "accent");
                bg_color.w = 0.5f;
                hover_color = ImVec4(bg_color.x * 1.2f, bg_color.y * 1.2f, bg_color.z * 1.2f, 0.6f);
                check_color = palette_color(theme_palette, "on_error", "on_surface");
            }
            else
            {
                bg_color = palette_color(theme_palette, "error", "accent");
                bg_color.w = 0.2f;
                hover_color = ImVec4(bg_color.x, bg_color.y, bg_color.z, 0.3f);
                check_color = palette_color(theme_palette, "on_error", "on_surface");
            }
            break;
            
        case checkbox_style::warning:
            if (*value)
            {
                bg_color = palette_color(theme_palette, "warning", "accent");
                bg_color.w = 0.5f;
                hover_color = ImVec4(bg_color.x * 1.2f, bg_color.y * 1.2f, bg_color.z * 1.2f, 0.6f);
                check_color = palette_color(theme_palette, "on_warning", "on_surface");
            }
            else
            {
                bg_color = palette_color(theme_palette, "surface", "background");
                hover_color = palette_color(theme_palette, "surface_variant", "surface");
                check_color = palette_color(theme_palette, "on_surface", "foreground");
            }
            break;
            
        case checkbox_style::normal:
        default:
            bg_color = palette_color(theme_palette, "surface", "background");
            hover_color = palette_color(theme_palette, "surface_variant", "surface");
            check_color = palette_color(theme_palette, "accent", "foreground");
            break;
    }

    ImGui::PushStyleColor(ImGuiCol_FrameBg, bg_color);
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, hover_color);
    ImGui::PushStyleColor(ImGuiCol_CheckMark, check_color);

    bool changed = ImGui::Checkbox(label.c_str(), value);

    ImGui::PopStyleColor(3);

    if (changed && m_on_changed)
    {
        m_on_changed(*value);
    }

    if (!m_tooltip.empty() && ImGui::IsItemHovered())
    {
        ImGui::SetTooltip("%s", m_tooltip.c_str());
    }

    return changed;
}

} // namespace clrsync::gui::widgets