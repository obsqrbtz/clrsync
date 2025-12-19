#include "gui/views/about_window.hpp"
#include "core/common/version.hpp"
#include "gui/widgets/centered_text.hpp"
#include "gui/widgets/colors.hpp"
#include "gui/widgets/link_button.hpp"
#include "imgui.h"

about_window::about_window() = default;

void about_window::render(const clrsync::core::palette &pal)
{
    if (!m_visible)
        return;

    ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);

    if (ImGui::Begin("About clrsync", &m_visible, ImGuiWindowFlags_NoResize))
    {
        using namespace clrsync::gui::widgets;

        ImVec4 title_color = palette_color(pal, "info", "accent");
        centered_text("clrsync", title_color);

        ImVec4 subtitle_color = palette_color(pal, "editor_inactive", "foreground");
        centered_text("Version " + clrsync::core::version_string(), subtitle_color);

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::TextWrapped("A color scheme management tool.");

        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Text("Links:");

        constexpr float button_width = 200.0f;
        float spacing = ImGui::GetStyle().ItemSpacing.x;
        float total_width = 2.0f * button_width + spacing;

        centered_buttons(total_width, [button_width]() {
            link_button("GitHub Repository", "https://github.com/obsqrbtz/clrsync", button_width);
            ImGui::SameLine();
            link_button("Documentation", "https://binarygoose.dev/projects/clrsync/overview/", button_width);
        });

        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::TextColored(subtitle_color, "MIT License");
        ImGui::TextWrapped(
            "Copyright (c) 2025 Daniel Dada\n\n"
            "Permission is hereby granted, free of charge, to any person obtaining a copy "
            "of this software and associated documentation files (the \"Software\"), to deal "
            "in the Software without restriction, including without limitation the rights "
            "to use, copy, modify, merge, publish, distribute, sublicense, and/or sell "
            "copies of the Software, and to permit persons to whom the Software is "
            "furnished to do so, subject to the following conditions:\n\n"
            "The above copyright notice and this permission notice shall be included in all "
            "copies or substantial portions of the Software.");
    }
    ImGui::End();
}
