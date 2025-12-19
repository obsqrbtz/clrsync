#include "palette_selector.hpp"
#include "imgui.h"
#include <ranges>

namespace clrsync::gui::widgets
{

palette_selector::palette_selector() = default;

bool palette_selector::render(const palette_controller &controller, float width)
{
    const auto &current = controller.current_palette();
    const auto &palettes = controller.palettes();
    bool selection_changed = false;

    ImGui::SetNextItemWidth(width);
    if (ImGui::BeginCombo("##palette", current.name().c_str()))
    {
        for (const auto &name : palettes | std::views::keys)
        {
            const bool selected = current.name() == name;
            if (ImGui::Selectable(name.c_str(), selected))
            {
                if (m_on_selection_changed)
                {
                    m_on_selection_changed(name);
                }
                selection_changed = true;
            }
            if (selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Select a color palette to edit");

    return selection_changed;
}

void palette_selector::set_on_selection_changed(const std::function<void(const std::string &)> &callback)
{
    m_on_selection_changed = callback;
}

} // namespace clrsync::gui::widgets