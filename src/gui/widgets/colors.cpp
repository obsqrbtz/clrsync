#include "colors.hpp"

namespace clrsync::gui::widgets
{

ImVec4 palette_color(const core::palette &pal, const std::string &key,
                     const std::string &fallback)
{
    auto colors = pal.colors();
    if (colors.empty())
        return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

    auto it = colors.find(key);
    if (it == colors.end() && !fallback.empty())
        it = colors.find(fallback);

    if (it != colors.end())
        return theme::color_utils::from_hex(it->second.hex());

    return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
}

uint32_t palette_color_u32(const core::palette &pal, const std::string &key,
                           const std::string &fallback)
{
    auto colors = pal.colors();
    if (colors.empty())
        return 0xFFFFFFFF;

    auto it = colors.find(key);
    if (it == colors.end() && !fallback.empty())
        it = colors.find(fallback);

    if (it != colors.end())
        return theme::color_utils::to_imgui_u32(it->second.hex());

    return 0xFFFFFFFF;
}

void push_success_button_style()
{
    const auto &t = theme::current_theme();
    ImGui::PushStyleColor(ImGuiCol_Button, t.success());
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, t.success_hovered());
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, t.success_active());
    ImGui::PushStyleColor(ImGuiCol_Text, t.on_success());
}

void push_error_button_style()
{
    const auto &t = theme::current_theme();
    ImGui::PushStyleColor(ImGuiCol_Button, t.error());
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, t.error_hovered());
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, t.error_active());
    ImGui::PushStyleColor(ImGuiCol_Text, t.on_error());
}

void push_warning_button_style()
{
    const auto &t = theme::current_theme();
    ImGui::PushStyleColor(ImGuiCol_Button, t.warning());
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, t.warning_hovered());
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, t.warning_active());
    ImGui::PushStyleColor(ImGuiCol_Text, t.on_warning());
}

void push_info_button_style()
{
    const auto &t = theme::current_theme();
    ImGui::PushStyleColor(ImGuiCol_Button, t.info());
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, t.info_hovered());
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, t.info_active());
    ImGui::PushStyleColor(ImGuiCol_Text, t.on_info());
}

void pop_button_style()
{
    ImGui::PopStyleColor(4);
}

} // namespace clrsync::gui::widgets
