#include "section_header.hpp"
#include "colors.hpp"
#include "imgui.h"

namespace clrsync::gui::widgets
{

void section_header(const std::string &title, const core::palette &)
{
    ImGui::Spacing();
    ImGui::TextColored(theme().accent(), "%s", title.c_str());
    ImGui::Separator();
    ImGui::Spacing();
}

} // namespace clrsync::gui::widgets
