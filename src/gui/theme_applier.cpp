#include "theme_applier.hpp"
#include "imgui.h"

namespace theme_applier
{

void apply_to_editor(TextEditor& editor, const clrsync::core::palette& current)
{
    auto get_color_u32 = [&](const std::string &key) -> uint32_t {
        const auto &col = current.get_color(key);
        const uint32_t hex = col.hex();
        // Convert from RRGGBBAA to AABBGGRR (ImGui format)
        const uint32_t r = (hex >> 24) & 0xFF;
        const uint32_t g = (hex >> 16) & 0xFF;
        const uint32_t b = (hex >> 8) & 0xFF;
        const uint32_t a = hex & 0xFF;
        return (a << 24) | (b << 16) | (g << 8) | r;
    };

    auto palette = editor.GetPalette();

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

    editor.SetPalette(palette);
}

void apply_to_imgui(const clrsync::core::palette& current)
{
    auto getColor = [&](const std::string &key) -> ImVec4 {
        const auto &col = current.get_color(key);
        const uint32_t hex = col.hex();
        return {((hex >> 24) & 0xFF) / 255.0f, ((hex >> 16) & 0xFF) / 255.0f,
                ((hex >> 8) & 0xFF) / 255.0f, ((hex) & 0xFF) / 255.0f};
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

} // namespace theme_applier
