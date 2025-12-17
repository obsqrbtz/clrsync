#include "about_window.hpp"
#include "core/version.hpp"
#include "imgui.h"

about_window::about_window()
{
}

void about_window::render()
{
    if (!m_visible)
        return;

    ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);

    if (ImGui::Begin("About clrsync", &m_visible, ImGuiWindowFlags_NoResize))
    {
        const float window_width = ImGui::GetContentRegionAvail().x;

        ImGui::PushFont(ImGui::GetFont());
        const char *title = "clrsync";
        const float title_size = ImGui::CalcTextSize(title).x;
        ImGui::SetCursorPosX((window_width - title_size) * 0.5f);
        ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "%s", title);
        ImGui::PopFont();

        std::string version = "Version " + clrsync::core::version_string();
        const float version_size = ImGui::CalcTextSize(version.c_str()).x;
        ImGui::SetCursorPosX((window_width - version_size) * 0.5f);
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "%s", version.c_str());

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::TextWrapped("A color scheme management tool.");

        ImGui::Spacing();
        ImGui::Spacing();

        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Text("Links:");

        if (ImGui::Button("GitHub Repository", ImVec2(200, 0)))
        {
#ifdef _WIN32
            system("start https://github.com/obsqrbtz/clrsync");
#elif __APPLE__
            system("open https://github.com/obsqrbtz/clrsync");
#else
            system("xdg-open https://github.com/obsqrbtz/clrsync");
#endif
        }
        ImGui::SameLine();

        if (ImGui::Button("Documentation", ImVec2(200, 0)))
        {
#ifdef _WIN32
            system("start https://binarygoose.dev/projects/clrsync/overview/");
#elif __APPLE__
            system("open https://binarygoose.dev/projects/clrsync/overview/");
#else
            system("xdg-open https://binarygoose.dev/projects/clrsync/overview/");
#endif
        }

        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "MIT License");
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

        ImGui::Spacing();
        ImGui::Spacing();

        const float button_width = 120.0f;
        ImGui::SetCursorPosX((window_width - button_width) * 0.5f);
        if (ImGui::Button("Close", ImVec2(button_width, 0)))
        {
            m_visible = false;
        }
    }
    ImGui::End();
}