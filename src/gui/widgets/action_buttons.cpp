#include "action_buttons.hpp"
#include "colors.hpp"
#include "imgui.h"

namespace clrsync::gui::widgets
{

action_buttons::action_buttons() = default;

void action_buttons::add_button(const action_button &button)
{
    m_buttons.push_back(button);
}

void action_buttons::clear()
{
    m_buttons.clear();
}

void action_buttons::render(const core::palette &theme_palette)
{
    if (m_buttons.empty())
        return;

    if (m_use_separator)
    {
        ImGui::Separator();
        ImGui::Spacing();
    }

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(m_spacing, 8));
    
    bool first = true;
    for (const auto &button : m_buttons)
    {
        if (!first)
            ImGui::SameLine();
        first = false;

        int style_colors_pushed = 0;
        if (button.use_error_style)
        {
            auto error = palette_color(theme_palette, "error");
            auto error_hover = ImVec4(error.x * 1.1f, error.y * 1.1f, error.z * 1.1f, error.w);
            auto error_active = ImVec4(error.x * 0.8f, error.y * 0.8f, error.z * 0.8f, error.w);
            auto on_error = palette_color(theme_palette, "on_error");
            ImGui::PushStyleColor(ImGuiCol_Button, error);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, error_hover);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, error_active);
            ImGui::PushStyleColor(ImGuiCol_Text, on_error);
            style_colors_pushed = 4;
        }
        else if (button.use_success_style)
        {
            auto success = palette_color(theme_palette, "success", "accent");
            auto success_hover = ImVec4(success.x * 1.1f, success.y * 1.1f, success.z * 1.1f, success.w);
            auto success_active = ImVec4(success.x * 0.8f, success.y * 0.8f, success.z * 0.8f, success.w);
            auto on_success = palette_color(theme_palette, "on_success", "on_surface");
            ImGui::PushStyleColor(ImGuiCol_Button, success);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, success_hover);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, success_active);
            ImGui::PushStyleColor(ImGuiCol_Text, on_success);
            style_colors_pushed = 4;
        }

        bool disabled = !button.enabled;
        if (disabled)
            ImGui::BeginDisabled();

        if (ImGui::Button(button.label.c_str()))
        {
            if (button.on_click)
            {
                button.on_click();
            }
        }

        if (disabled)
            ImGui::EndDisabled();

        if (style_colors_pushed > 0)
            ImGui::PopStyleColor(style_colors_pushed);

        if (!button.tooltip.empty() && ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("%s", button.tooltip.c_str());
        }
    }

    ImGui::PopStyleVar();
}

} // namespace clrsync::gui::widgets