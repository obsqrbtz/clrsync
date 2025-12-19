#include "centered_text.hpp"

namespace clrsync::gui::widgets
{

void centered_text(const std::string& text, const ImVec4& color)
{
    float window_width = ImGui::GetContentRegionAvail().x;
    float text_width = ImGui::CalcTextSize(text.c_str()).x;
    ImGui::SetCursorPosX((window_width - text_width) * 0.5f);
    ImGui::TextColored(color, "%s", text.c_str());
}

void centered_buttons(float total_width, const std::function<void()>& render_buttons)
{
    float window_width = ImGui::GetContentRegionAvail().x;
    float start_pos = (window_width - total_width) * 0.5f;
    if (start_pos > 0)
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + start_pos);
    render_buttons();
}

} // namespace clrsync::gui::widgets
