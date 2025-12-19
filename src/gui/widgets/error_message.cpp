#include "error_message.hpp"
#include "colors.hpp"
#include "imgui.h"

namespace clrsync::gui::widgets
{

void error_message::set(const std::string& message)
{
    m_message = message;
}

void error_message::clear()
{
    m_message.clear();
}

bool error_message::has_error() const
{
    return !m_message.empty();
}

const std::string& error_message::get() const
{
    return m_message;
}

void error_message::render(const core::palette& palette)
{
    if (m_message.empty())
        return;

    ImGui::Spacing();

    auto error_bg_color = palette_color(palette, "error");
    auto error_text_color = palette_color(palette, "on_error");

    ImGui::PushStyleColor(ImGuiCol_ChildBg, error_bg_color);
    ImGui::PushStyleColor(ImGuiCol_Border, error_bg_color);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 4.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 1.0f);

    if (ImGui::BeginChild("##error_box", ImVec2(0, 0),
                          ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_Borders))
    {
        ImGui::PushStyleColor(ImGuiCol_Text, error_text_color);
        ImGui::TextWrapped("Error: %s", m_message.c_str());
        ImGui::PopStyleColor();

        ImGui::Spacing();

        ImGui::PushStyleColor(ImGuiCol_Button,
                              ImVec4(error_bg_color.x * 0.8f, error_bg_color.y * 0.8f,
                                     error_bg_color.z * 0.8f, error_bg_color.w));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                              ImVec4(error_bg_color.x * 0.6f, error_bg_color.y * 0.6f,
                                     error_bg_color.z * 0.6f, error_bg_color.w));
        ImGui::PushStyleColor(ImGuiCol_Text, error_text_color);

        if (ImGui::Button("Dismiss##error"))
            m_message.clear();

        ImGui::PopStyleColor(3);
    }
    ImGui::EndChild();

    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(2);
}

} // namespace clrsync::gui::widgets
