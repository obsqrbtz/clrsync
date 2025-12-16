#include "preview_renderer.hpp"
#include "theme_applier.hpp"
#include "imgui.h"
#include <algorithm>

preview_renderer::preview_renderer()
{
    m_editor.SetLanguageDefinition(TextEditor::LanguageDefinition::CPlusPlus());
    m_editor.SetText(R"(#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <cstdlib>

namespace fs = std::filesystem;

// Expands ~ to the user's home directory
std::string expand_user(const std::string &path)
{
    if (path.empty()) return "";

    std::string result;
    if (path[0] == '~')
    {
#ifdef _WIN32
        const char* home = std::getenv("USERPROFILE");
#else
        const char* home = std::getenv("HOME");
#endif
        result = home ? std::string(home) : "~";
        result += path.substr(1);
    }
    else
    {
        result = path;
    }

    return result;
}

// Lists all files in a directory
std::vector<std::string> list_files(const std::string &dir_path)
{
    std::vector<std::string> files;
    try
    {
        for (const auto &entry : fs::directory_iterator(dir_path))
        {
            if (entry.is_regular_file())
                files.push_back(entry.path().string());
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return files;
}

int main()
{
    std::string path = expand_user("~/Documents");
    std::cout << "Listing files in: " << path << std::endl;

    auto files = list_files(path);
    for (const auto &f : files)
        std::cout << "  " << f << std::endl;

    return 0;
})");

    m_editor.SetShowWhitespaces(false);
}

void preview_renderer::apply_palette(const clrsync::core::palette& palette)
{
    theme_applier::apply_to_editor(m_editor, palette);
}

void preview_renderer::render_code_preview()
{
    const float avail_height = ImGui::GetContentRegionAvail().y;
    const float code_preview_height = std::max(250.0f, avail_height * 0.55f);

    ImGui::Text("Code Editor:");
    m_editor.Render("##CodeEditor", ImVec2(0, code_preview_height), true);
}

void preview_renderer::render_terminal_preview(const clrsync::core::palette& current)
{
    auto get_color = [&](const std::string &key) -> ImVec4 {
        auto it = current.colors().find(key);
        if (it != current.colors().end())
        {
            const auto &col = it->second;
            const uint32_t hex = col.hex();
            return {((hex >> 24) & 0xFF) / 255.0f, ((hex >> 16) & 0xFF) / 255.0f,
                    ((hex >> 8) & 0xFF) / 255.0f, ((hex) & 0xFF) / 255.0f};
        }
        return {1, 1, 1, 1};
    };

    const ImVec4 editor_bg = get_color("editor_background");
    const ImVec4 fg = get_color("foreground");
    const ImVec4 accent = get_color("accent");
    const ImVec4 border = get_color("border");
    const ImVec4 error = get_color("error");
    const ImVec4 warning = get_color("warning");
    const ImVec4 success = get_color("success");
    const ImVec4 info = get_color("info");

    ImGui::Spacing();
    ImGui::Text("Terminal Preview:");
    
    ImGui::PushStyleColor(ImGuiCol_ChildBg, editor_bg);
    ImGui::BeginChild("TerminalPreview", ImVec2(0, 0), true);
    ImGui::PushStyleColor(ImGuiCol_Border, border);

    struct term_line
    {
        const char *text{};
        ImVec4 col;
    };
    term_line term_lines[] = {
        {"$ ls -la", fg},
        {"drwxr-xr-x 5 user group 4096 Dec  2 10:30 .", accent},
        {"Build successful", success},
        {"Error: file not found", error},
        {"Warning: low disk space", warning},
        {"Info: update available", info},
    };

    for (auto &[text, col] : term_lines)
    {
        ImGui::TextColored(col, "%s", text);
    }

    ImGui::PopStyleColor(2);
    ImGui::EndChild();
}

void preview_renderer::render(const clrsync::core::palette& current)
{
    if (current.colors().empty())
    {
        ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "Current palette is empty");
        return;
    }

    render_code_preview();
    render_terminal_preview(current);
}
