#include "gui/views/preview_renderer.hpp"
#include "gui/controllers/theme_applier.hpp"
#include "gui/widgets/colors.hpp"
#include "imgui.h"
#include <algorithm>
#include <array>

preview_renderer::preview_renderer()
{
    m_editor.SetLanguageDefinition(TextEditor::LanguageDefinition::CPlusPlus());
    m_editor.SetText(R"(#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <cstdlib>

namespace fs = std::filesystem;

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

static ImVec4 hex_to_imvec4(uint32_t hex)
{
    return {((hex >> 24) & 0xFF) / 255.0f, ((hex >> 16) & 0xFF) / 255.0f,
            ((hex >> 8) & 0xFF) / 255.0f, (hex & 0xFF) / 255.0f};
}

void preview_renderer::apply_palette(const clrsync::core::palette &palette)
{
    theme_applier::apply_to_editor(m_editor, palette);
}

void preview_renderer::render_code_preview()
{
    const float avail_height = ImGui::GetContentRegionAvail().y;
    const float code_preview_height = std::max(250.0f, avail_height * 0.50f);

    ImGui::Text("Code Editor Preview:");
    ImGui::SameLine();
    ImGui::TextDisabled("(editor_* colors)");
    m_editor.Render("##CodeEditor", ImVec2(0, code_preview_height), true);
}

void preview_renderer::render_terminal_preview(const clrsync::core::palette &current)
{
    auto get_color = [&](const std::string &key) -> ImVec4 {
        const auto &col = current.get_color(key);
        return hex_to_imvec4(col.hex());
    };
    const ImVec4 bg = get_color("base00");
    const ImVec4 fg = get_color("base07");
    const ImVec4 cursor_col = get_color("cursor");
    const ImVec4 border_col = get_color("border");

    const ImVec4 black = get_color("base00");
    const ImVec4 red = get_color("base01");
    const ImVec4 green = get_color("base02");
    const ImVec4 yellow = get_color("base03");
    const ImVec4 blue = get_color("base04");
    const ImVec4 magenta = get_color("base05");
    const ImVec4 cyan = get_color("base06");
    const ImVec4 white = get_color("base07");

    const ImVec4 bright_black = get_color("base08");
    const ImVec4 bright_red = get_color("base09");
    const ImVec4 bright_green = get_color("base0A");
    const ImVec4 bright_yellow = get_color("base0B");
    const ImVec4 bright_blue = get_color("base0C");
    const ImVec4 bright_magenta = get_color("base0D");
    const ImVec4 bright_cyan = get_color("base0E");
    const ImVec4 bright_white = get_color("base0F");

    ImGui::Spacing();
    ImGui::Text("Terminal Preview:");
    ImGui::SameLine();
    ImGui::TextDisabled("(base00-base0F colors)");

    ImGui::PushStyleColor(ImGuiCol_ChildBg, bg);
    ImGui::PushStyleColor(ImGuiCol_Border, border_col);

    const float terminal_height = std::max(200.0f, ImGui::GetContentRegionAvail().y - 10.0f);
    ImGui::BeginChild("TerminalPreview", ImVec2(0, terminal_height), true);

    ImGui::TextColored(green, "user@host");
    ImGui::SameLine(0, 0);
    ImGui::TextColored(fg, ":");
    ImGui::SameLine(0, 0);
    ImGui::TextColored(blue, "~/projects");
    ImGui::SameLine(0, 0);
    ImGui::TextColored(fg, "$ ");
    ImGui::SameLine(0, 0);
    ImGui::TextColored(fg, "ls -la");

    ImGui::TextColored(fg, "total 48");
    ImGui::TextColored(blue, "drwxr-xr-x");
    ImGui::SameLine();
    ImGui::TextColored(fg, " 5 user group  4096 Dec  2 10:30 ");
    ImGui::SameLine(0, 0);
    ImGui::TextColored(blue, ".");

    ImGui::TextColored(blue, "drwxr-xr-x");
    ImGui::SameLine();
    ImGui::TextColored(fg, " 3 user group  4096 Dec  1 09:15 ");
    ImGui::SameLine(0, 0);
    ImGui::TextColored(blue, "..");

    ImGui::TextColored(fg, "-rw-r--r--");
    ImGui::SameLine();
    ImGui::TextColored(fg, " 1 user group  1234 Dec  2 10:30 ");
    ImGui::SameLine(0, 0);
    ImGui::TextColored(fg, "README.md");

    ImGui::TextColored(fg, "-rwxr-xr-x");
    ImGui::SameLine();
    ImGui::TextColored(fg, " 1 user group  8192 Dec  2 10:28 ");
    ImGui::SameLine(0, 0);
    ImGui::TextColored(green, "build.sh");

    ImGui::TextColored(cyan, "lrwxrwxrwx");
    ImGui::SameLine();
    ImGui::TextColored(fg, " 1 user group    24 Dec  1 15:00 ");
    ImGui::SameLine(0, 0);
    ImGui::TextColored(cyan, "config -> ~/.config/app");

    ImGui::Spacing();

    ImGui::TextColored(green, "user@host");
    ImGui::SameLine(0, 0);
    ImGui::TextColored(fg, ":");
    ImGui::SameLine(0, 0);
    ImGui::TextColored(blue, "~/projects");
    ImGui::SameLine(0, 0);
    ImGui::TextColored(fg, "$ ");
    ImGui::SameLine(0, 0);
    ImGui::TextColored(fg, "git status");

    ImGui::TextColored(fg, "On branch ");
    ImGui::SameLine(0, 0);
    ImGui::TextColored(green, "main");
    ImGui::TextColored(fg, "Changes to be committed:");
    ImGui::TextColored(green, "  modified:   src/main.cpp");
    ImGui::TextColored(green, "  new file:   src/utils.hpp");
    ImGui::TextColored(fg, "Changes not staged:");
    ImGui::TextColored(red, "  modified:   README.md");

    ImGui::Spacing();

    ImGui::TextColored(fg, "ANSI Colors (0-7 / 8-15):");

    const float box_size = 20.0f;
    const float spacing = 4.0f;
    ImVec2 start_pos = ImGui::GetCursorScreenPos();
    ImDrawList *draw_list = ImGui::GetWindowDrawList();

    std::array<ImVec4, 8> normal_colors = {black, red, green, yellow, blue, magenta, cyan, white};
    for (size_t i = 0; i < 8; i++)
    {
        ImVec2 p0 = ImVec2(start_pos.x + i * (box_size + spacing), start_pos.y);
        ImVec2 p1 = ImVec2(p0.x + box_size, p0.y + box_size);
        draw_list->AddRectFilled(p0, p1, ImGui::ColorConvertFloat4ToU32(normal_colors[i]));
        draw_list->AddRect(p0, p1, ImGui::ColorConvertFloat4ToU32(border_col));
    }

    std::array<ImVec4, 8> bright_colors = {bright_black,  bright_red,  bright_green,
                                           bright_yellow, bright_blue, bright_magenta,
                                           bright_cyan,   bright_white};
    for (size_t i = 0; i < 8; i++)
    {
        ImVec2 p0 =
            ImVec2(start_pos.x + i * (box_size + spacing), start_pos.y + box_size + spacing);
        ImVec2 p1 = ImVec2(p0.x + box_size, p0.y + box_size);
        draw_list->AddRectFilled(p0, p1, ImGui::ColorConvertFloat4ToU32(bright_colors[i]));
        draw_list->AddRect(p0, p1, ImGui::ColorConvertFloat4ToU32(border_col));
    }

    ImGui::Dummy(ImVec2(8 * (box_size + spacing), 2 * box_size + spacing + 4));

    ImGui::PopStyleColor(2);
    ImGui::EndChild();
}

void preview_renderer::render(const clrsync::core::palette &current)
{
    if (current.colors().empty())
    {
        ImVec4 error_color = clrsync::gui::widgets::palette_color(current, "error", "accent");
        ImGui::TextColored(error_color, "Current palette is empty");
        return;
    }

    render_code_preview();
    render_terminal_preview(current);
}
