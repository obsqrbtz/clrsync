#include "gui/controllers/theme_applier.hpp"
#include "gui/theme/app_theme.hpp"

namespace theme_applier
{

void apply_to_editor(TextEditor &editor, const clrsync::core::palette &current)
{
    using namespace clrsync::gui::theme;

    auto get_color_u32 = [&](const std::string &key) -> uint32_t {
        const auto &col = current.get_color(key);
        return color_utils::to_imgui_u32(col.hex());
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

void apply_to_imgui(const clrsync::core::palette &current)
{
    clrsync::gui::theme::set_theme(current);
}

} // namespace theme_applier
