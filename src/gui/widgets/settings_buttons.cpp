#include "settings_buttons.hpp"
#include "imgui.h"

namespace clrsync::gui::widgets
{

void settings_buttons::render(const settings_buttons_callbacks& callbacks, bool apply_enabled)
{
    ImGui::Spacing();

    float spacing = ImGui::GetStyle().ItemSpacing.x;
    float window_width = ImGui::GetContentRegionAvail().x;
    float total_buttons_width = 4 * m_button_width + 3 * spacing;
    float start_pos = (window_width - total_buttons_width) * 0.5f;

    if (start_pos > 0)
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + start_pos);

    if (ImGui::Button("OK", ImVec2(m_button_width, 0)))
    {
        if (callbacks.on_ok)
            callbacks.on_ok();
    }

    ImGui::SameLine();

    if (!apply_enabled)
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.5f);

    if (ImGui::Button("Apply", ImVec2(m_button_width, 0)) && apply_enabled)
    {
        if (callbacks.on_apply)
            callbacks.on_apply();
    }

    if (!apply_enabled)
        ImGui::PopStyleVar();

    ImGui::SameLine();

    if (ImGui::Button("Reset", ImVec2(m_button_width, 0)))
    {
        if (callbacks.on_reset)
            callbacks.on_reset();
    }

    ImGui::SameLine();

    if (ImGui::Button("Cancel", ImVec2(m_button_width, 0)))
    {
        if (callbacks.on_cancel)
            callbacks.on_cancel();
    }
}

} // namespace clrsync::gui::widgets
