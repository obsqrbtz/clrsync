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

void action_buttons::render(const core::palette &)
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
            ImGui::SameLine(0, m_spacing);
        first = false;

        bool has_style = false;
        if (button.use_error_style)
        {
            push_error_button_style();
            has_style = true;
        }
        else if (button.use_success_style)
        {
            push_success_button_style();
            has_style = true;
        }
        else if (button.use_info_style)
        {
            push_info_button_style();
            has_style = true;
        }

        bool disabled = !button.enabled;
        if (disabled)
            ImGui::BeginDisabled();

        if (ImGui::Button(button.label.c_str()))
        {
            if (button.on_click)
                button.on_click();
        }

        if (disabled)
            ImGui::EndDisabled();

        if (has_style)
            pop_button_style();

        if (!button.tooltip.empty() && ImGui::IsItemHovered())
            ImGui::SetTooltip("%s", button.tooltip.c_str());
    }

    ImGui::PopStyleVar();
}

} // namespace clrsync::gui::widgets