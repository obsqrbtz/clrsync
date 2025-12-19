#include "validation_message.hpp"
#include "colors.hpp"
#include "imgui.h"

namespace clrsync::gui::widgets
{

void validation_message::set(const std::string& message)
{
    m_message = message;
}

void validation_message::clear()
{
    m_message.clear();
}

bool validation_message::has_error() const
{
    return !m_message.empty();
}

const std::string& validation_message::get() const
{
    return m_message;
}

void validation_message::render(const core::palette& palette)
{
    if (m_message.empty())
        return;

    ImGui::Spacing();
    ImVec4 error_color = palette_color(palette, "error", "accent");
    ImGui::PushStyleColor(ImGuiCol_Text, error_color);
    ImGui::TextWrapped("%s", m_message.c_str());
    ImGui::PopStyleColor();
}

} // namespace clrsync::gui::widgets
