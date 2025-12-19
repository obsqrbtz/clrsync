#include "section_header.hpp"
#include "colors.hpp"
#include "imgui.h"

namespace clrsync::gui::widgets
{

void section_header(const std::string& title, const core::palette& palette)
{
    ImGui::Spacing();
    auto accent_color = palette_color(palette, "accent");
    ImGui::TextColored(accent_color, "%s", title.c_str());
    ImGui::Separator();
    ImGui::Spacing();
}

} // namespace clrsync::gui::widgets
