#include "color_scheme_editor.hpp"
#include "template_editor.hpp"
#include "color_text_edit/TextEditor.h"
#include "imgui.h"
#include <iostream>
#include <ranges>

color_scheme_editor::color_scheme_editor()
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
    
    const auto &palettes = m_controller.palettes();
    
    const auto &current = m_controller.current_palette();
    
    if (!current.colors().empty())
    {
        apply_palette_to_imgui();
        apply_palette_to_editor();
    }
    else
    {
        std::cout << "WARNING: No palette loaded, skipping theme application\n";
    }
}

void color_scheme_editor::notify_palette_changed()
{
    if (m_template_editor)
    {
        m_template_editor->apply_current_palette(m_controller.current_palette());
    }
}

void color_scheme_editor::render_controls_and_colors()
{
    ImGui::Begin("Color Schemes");

    render_controls();
    ImGui::Separator();

    ImGui::BeginChild("ColorTableContent", ImVec2(0, 0), false);
    render_color_table();
    ImGui::EndChild();

    ImGui::End();
}

void color_scheme_editor::render_preview()
{
    ImGui::Begin("Color Preview");
    
    render_preview_content();
    
    ImGui::End();
}

void color_scheme_editor::render_controls()
{
    const auto &current = m_controller.current_palette();
    const auto &palettes = m_controller.palettes();

    const float avail_width = ImGui::GetContentRegionAvail().x;

    ImGui::Text("Color Scheme:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(std::min(200.0f, avail_width * 0.3f));
    if (ImGui::BeginCombo("##scheme", current.name().c_str()))
    {
        for (const auto &name : palettes | std::views::keys)
        {
            const bool selected = current.name() == name;
            if (ImGui::Selectable(name.c_str(), selected))
            {
                m_controller.select_palette(name);
                apply_palette_to_imgui();
                apply_palette_to_editor();
                notify_palette_changed();
            }
            if (selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    ImGui::SameLine();

    static char new_palette_name_buf[128] = "";
    if (ImGui::Button("New"))
    {
        new_palette_name_buf[0] = 0;
        ImGui::OpenPopup("New Palette");
    }

    if (ImGui::BeginPopupModal("New Palette", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("New palette name:");
        ImGui::InputText("##new_palette_input", new_palette_name_buf,
                         IM_ARRAYSIZE(new_palette_name_buf));

        ImGui::Separator();

        if (ImGui::Button("Create", ImVec2(120, 0)))
        {
            if (strlen(new_palette_name_buf) > 0)
            {
                m_controller.create_palette(new_palette_name_buf);
                apply_palette_to_imgui();
                apply_palette_to_editor();
                notify_palette_changed();
                new_palette_name_buf[0] = 0;
            }
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel", ImVec2(120, 0)))
        {
            new_palette_name_buf[0] = 0;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    ImGui::SameLine();
    if (ImGui::Button("Save"))
    {
        m_controller.save_current_palette();
    }

    ImGui::SameLine();
    if (ImGui::Button("Delete"))
    {
        m_controller.delete_current_palette();
    }

    ImGui::SameLine();
    if (ImGui::Button("Apply"))
    {
        m_controller.apply_current_theme();
    }
}

void color_scheme_editor::render_color_table()
{
    const auto &current = m_controller.current_palette();

    if (current.colors().empty())
    {
        ImGui::Text("No palette loaded");
        return;
    }

    ImGui::Text("Color Variables");
    ImGui::Separator();

    auto render_color_row = [&](const std::string &name) {
        const auto &colors = current.colors();
        auto it = colors.find(name);
        if (it == colors.end())
            return;

        const clrsync::core::color &col = it->second;

        ImGui::TableNextRow();

        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted(name.c_str());

        ImGui::TableSetColumnIndex(1);
        {
            std::string hex_str = col.to_hex_string();
            char buf[9];
            strncpy(buf, hex_str.c_str(), sizeof(buf));
            buf[8] = 0;

            ImGui::SetNextItemWidth(-FLT_MIN);
            if (ImGui::InputText(("##hex_" + name).c_str(), buf, sizeof(buf),
                                 ImGuiInputTextFlags_CharsUppercase))
            {
                try
                {
                    clrsync::core::color new_color;
                    new_color.from_hex_string(buf);
                    m_controller.set_color(name, new_color);
                    apply_palette_to_imgui();
                    apply_palette_to_editor();
                    notify_palette_changed();
                }
                catch (...)
                {
                }
            }
        }

        ImGui::TableSetColumnIndex(2);
        ImGui::PushID(name.c_str());
        float c[4] = {((col.hex() >> 24) & 0xFF) / 255.0f, ((col.hex() >> 16) & 0xFF) / 255.0f,
                      ((col.hex() >> 8) & 0xFF) / 255.0f, (col.hex() & 0xFF) / 255.0f};

        ImGui::SetNextItemWidth(-FLT_MIN);
        if (ImGui::ColorEdit4(("##color_" + name).c_str(), c,
                              ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel |
                                  ImGuiColorEditFlags_AlphaBar))
        {
            uint32_t r = (uint32_t)(c[0] * 255.0f);
            uint32_t g = (uint32_t)(c[1] * 255.0f);
            uint32_t b = (uint32_t)(c[2] * 255.0f);
            uint32_t a = (uint32_t)(c[3] * 255.0f);
            uint32_t hex = (r << 24) | (g << 16) | (b << 8) | a;

            m_controller.set_color(name, clrsync::core::color(hex));
            apply_palette_to_imgui();
            apply_palette_to_editor();
            notify_palette_changed();
        }

        ImGui::PopID();
    };

    auto draw_table = [&](const char *title, const std::vector<const char *> &keys) {
        ImGui::TextUnformatted(title);

        if (ImGui::BeginTable(title, 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
        {
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 160.0f);
            ImGui::TableSetupColumn("HEX", ImGuiTableColumnFlags_WidthFixed, 90.0f);
            ImGui::TableSetupColumn("Preview", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableHeadersRow();

            for (auto *k : keys)
                render_color_row(k);

            ImGui::EndTable();
        }

        ImGui::Spacing();
    };

    draw_table("General UI", {"background", "on_background", "surface", "on_surface",
                              "surface_variant", "on_surface_varuant", "foreground",
                              "cursor", "accent"});

    draw_table("Borders", {"border_focused", "border"});

    draw_table("Semantic Colors", {"success", "info", "warning", "error",
                                   "on_success", "on_info", "on_warning", "on_error"});

    draw_table("Editor", {"editor_background", "editor_command", "editor_comment",
                          "editor_disabled", "editor_emphasis", "editor_error",
                          "editor_inactive", "editor_line_number", "editor_link",
                          "editor_main", "editor_selected", "editor_selection_inactive",
                          "editor_string", "editor_success", "editor_warning"});

    draw_table("Terminal (Base16)", {"base00", "base01", "base02", "base03",
                                     "base04", "base05", "base06", "base07",
                                     "base08", "base09", "base0A", "base0B",
                                     "base0C", "base0D", "base0E", "base0F"});
}

void color_scheme_editor::render_preview_content()
{
    const auto &current = m_controller.current_palette();

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

    const float avail_height = ImGui::GetContentRegionAvail().y;
    const float code_preview_height = std::max(250.0f, avail_height * 0.55f);

    ImGui::Text("Code Editor:");

    m_editor.Render("##CodeEditor", ImVec2(0, code_preview_height), true);

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

void color_scheme_editor::apply_palette_to_editor()
{
    const auto &current = m_controller.current_palette();

    auto get_color_u32 = [&](const std::string &key, const std::string &fallback = "") -> uint32_t {
        auto it = current.colors().find(key);
        if (it == current.colors().end() && !fallback.empty())
        {
            it = current.colors().find(fallback);
        }
        
        if (it != current.colors().end())
        {
            const auto &col = it->second;
            const uint32_t hex = col.hex();
            // Convert from RRGGBBAA to AABBGGRR (ImGui format)
            const uint32_t r = (hex >> 24) & 0xFF;
            const uint32_t g = (hex >> 16) & 0xFF;
            const uint32_t b = (hex >> 8) & 0xFF;
            const uint32_t a = hex & 0xFF;
            return (a << 24) | (b << 16) | (g << 8) | r;
        }
        return 0xFFFFFFFF;
    };

    auto palette = m_editor.GetPalette();

    palette[int(TextEditor::PaletteIndex::Default)] = get_color_u32("editor_main");
    palette[int(TextEditor::PaletteIndex::Keyword)] = get_color_u32("editor_command");
    palette[int(TextEditor::PaletteIndex::Number)] = get_color_u32("editor_warning");
    palette[int(TextEditor::PaletteIndex::String)] = get_color_u32("editor_string");
    palette[int(TextEditor::PaletteIndex::CharLiteral)] = get_color_u32("editor_string");
    palette[int(TextEditor::PaletteIndex::Punctuation)] = get_color_u32("editor_main");
    palette[int(TextEditor::PaletteIndex::Preprocessor)] = get_color_u32("editor_emphasis");
    palette[int(TextEditor::PaletteIndex::Identifier)] = get_color_u32("editor_main");
    palette[int(TextEditor::PaletteIndex::KnownIdentifier)] = get_color_u32("editor_link");
    palette[int(TextEditor::PaletteIndex::PreprocIdentifier)] = get_color_u32("editor_link");

    palette[int(TextEditor::PaletteIndex::Comment)] = get_color_u32("editor_comment");
    palette[int(TextEditor::PaletteIndex::MultiLineComment)] = get_color_u32("editor_comment");

    palette[int(TextEditor::PaletteIndex::Background)] = get_color_u32("editor_background");
    palette[int(TextEditor::PaletteIndex::Cursor)] = get_color_u32("cursor");

    palette[int(TextEditor::PaletteIndex::Selection)] = get_color_u32("editor_selected");
    palette[int(TextEditor::PaletteIndex::ErrorMarker)] = get_color_u32("editor_error");
    palette[int(TextEditor::PaletteIndex::Breakpoint)] = get_color_u32("editor_error");

    palette[int(TextEditor::PaletteIndex::LineNumber)] = get_color_u32("editor_line_number");
    palette[int(TextEditor::PaletteIndex::CurrentLineFill)] = get_color_u32("surface_variant");
    palette[int(TextEditor::PaletteIndex::CurrentLineFillInactive)] = get_color_u32("surface");

    palette[int(TextEditor::PaletteIndex::CurrentLineEdge)] = get_color_u32("border_focused");

    m_editor.SetPalette(palette);
}

void color_scheme_editor::apply_palette_to_imgui() const
{
    const auto &current = m_controller.current_palette();
    
    auto getColor = [&](const std::string &key, const std::string &fallback = "") -> ImVec4 {
        auto it = current.colors().find(key);
        if (it == current.colors().end() && !fallback.empty())
        {
            it = current.colors().find(fallback);
        }
        
        if (it != current.colors().end())
        {
            const uint32_t hex = it->second.hex();
            return {((hex >> 24) & 0xFF) / 255.0f, ((hex >> 16) & 0xFF) / 255.0f,
                    ((hex >> 8) & 0xFF) / 255.0f, ((hex) & 0xFF) / 255.0f};
        }
        
        std::cout << "WARNING: Color key '" << key << "' not found!\n";
        return {1, 1, 1, 1};
    };

    ImGuiStyle &style = ImGui::GetStyle();

    const ImVec4 bg = getColor("background");
    const ImVec4 surface = getColor("surface");
    const ImVec4 surfaceVariant = getColor("surface_variant");
    const ImVec4 fg = getColor("foreground");
    const ImVec4 fgInactive = getColor("editor_inactive");
    const ImVec4 accent = getColor("accent");
    const ImVec4 border = getColor("border");

    style.Colors[ImGuiCol_WindowBg] = bg;
    style.Colors[ImGuiCol_ChildBg] = surface;
    style.Colors[ImGuiCol_PopupBg] = surface;

    style.Colors[ImGuiCol_Border] = border;
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(0, 0, 0, 0);

    style.Colors[ImGuiCol_Text] = fg;
    style.Colors[ImGuiCol_TextDisabled] = fgInactive;

    style.Colors[ImGuiCol_Header] = surfaceVariant;
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(accent.x, accent.y, accent.z, 0.8f);
    style.Colors[ImGuiCol_HeaderActive] = accent;

    style.Colors[ImGuiCol_Button] = surfaceVariant;
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(accent.x, accent.y, accent.z, 0.6f);
    style.Colors[ImGuiCol_ButtonActive] = accent;

    style.Colors[ImGuiCol_FrameBg] = surfaceVariant;
    style.Colors[ImGuiCol_FrameBgHovered] =
        ImVec4(surfaceVariant.x * 1.1f, surfaceVariant.y * 1.1f, surfaceVariant.z * 1.1f, 1.0f);
    style.Colors[ImGuiCol_FrameBgActive] =
        ImVec4(surfaceVariant.x * 1.2f, surfaceVariant.y * 1.2f, surfaceVariant.z * 1.2f, 1.0f);

    style.Colors[ImGuiCol_TitleBg] = surface;
    style.Colors[ImGuiCol_TitleBgActive] = surfaceVariant;
    style.Colors[ImGuiCol_TitleBgCollapsed] = surface;

    style.Colors[ImGuiCol_ScrollbarBg] = surface;
    style.Colors[ImGuiCol_ScrollbarGrab] = surfaceVariant;
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(accent.x, accent.y, accent.z, 0.6f);
    style.Colors[ImGuiCol_ScrollbarGrabActive] = accent;

    style.Colors[ImGuiCol_SliderGrab] = accent;
    style.Colors[ImGuiCol_SliderGrabActive] =
        ImVec4(accent.x * 1.2f, accent.y * 1.2f, accent.z * 1.2f, 1.0f);

    style.Colors[ImGuiCol_CheckMark] = accent;
    style.Colors[ImGuiCol_ResizeGrip] = surfaceVariant;
    style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(accent.x, accent.y, accent.z, 0.6f);
    style.Colors[ImGuiCol_ResizeGripActive] = accent;

    style.Colors[ImGuiCol_Tab] = surface;
    style.Colors[ImGuiCol_TabHovered] = ImVec4(accent.x, accent.y, accent.z, 0.8f);
    style.Colors[ImGuiCol_TabActive] = surfaceVariant;
    style.Colors[ImGuiCol_TabUnfocused] = surface;
    style.Colors[ImGuiCol_TabUnfocusedActive] = surfaceVariant;
    style.Colors[ImGuiCol_TabSelectedOverline] = accent;

    style.Colors[ImGuiCol_TableHeaderBg] = surfaceVariant;
    style.Colors[ImGuiCol_TableBorderStrong] = border;
    style.Colors[ImGuiCol_TableBorderLight] =
        ImVec4(border.x * 0.7f, border.y * 0.7f, border.z * 0.7f, border.w);

    style.Colors[ImGuiCol_TableRowBg] = ImVec4(0, 0, 0, 0);
    style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(fg.x, fg.y, fg.z, 0.06f);

    style.Colors[ImGuiCol_Separator] = border;
    style.Colors[ImGuiCol_SeparatorHovered] = accent;
    style.Colors[ImGuiCol_SeparatorActive] = accent;

    style.Colors[ImGuiCol_MenuBarBg] = surface;

    style.Colors[ImGuiCol_DockingPreview] = ImVec4(accent.x, accent.y, accent.z, 0.7f);
    style.Colors[ImGuiCol_DockingEmptyBg] = bg;
}