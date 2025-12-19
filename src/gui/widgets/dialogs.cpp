#include "dialogs.hpp"
#include "colors.hpp"
#include "imgui.h"

namespace clrsync::gui::widgets
{

bool delete_confirmation_dialog(const std::string &popup_title, const std::string &item_name,
                                const std::string &item_type, const core::palette &theme_palette,
                                const std::function<void()> &on_delete)
{
    bool result = false;
    if (ImGui::BeginPopupModal(popup_title.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImVec4 warning_color = palette_color(theme_palette, "warning", "accent");
        ImGui::TextColored(warning_color, "Are you sure you want to delete '%s'?",
                           item_name.c_str());
        ImGui::Text("This action cannot be undone.");

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        float button_width = 120.0f;
        float total_width = 2.0f * button_width + ImGui::GetStyle().ItemSpacing.x;
        float window_width = ImGui::GetContentRegionAvail().x;
        ImGui::SetCursorPosX((window_width - total_width) * 0.5f);

        if (ImGui::Button("Delete", ImVec2(button_width, 0)))
        {
            on_delete();
            result = true;
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(button_width, 0)))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
    return result;
}

} // namespace clrsync::gui::widgets
