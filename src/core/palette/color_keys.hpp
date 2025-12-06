#ifndef CLRSYNC_CORE_PALETTE_COLOR_KEYS_HPP
#define CLRSYNC_CORE_PALETTE_COLOR_KEYS_HPP
#include <cstddef>
#include <iterator>

namespace clrsync::core
{
constexpr const char *COLOR_KEYS[] = {
    // UI / Surfaces
    "background",           // main window / editor background
    "surface",              // panels, cards
    "surface_variant",      // alternate rows, subtle panels
    "foreground",           // main text
    "foreground_secondary", // secondary text / hints
    "accent",               // buttons, highlights, selection
    "outline",              // borders, outlines
    "shadow",               // drop shadows / depth
    "cursor",               // caret / text cursor

    // Editor-specific surfaces
    "editor_background", "sidebar_background", "popup_background", "floating_window_background",
    "menu_option_background",

    // Editor text roles
    "text_main", "text_emphasis", "text_command", "text_inactive", "text_disabled",
    "text_line_number", "text_selected", "text_selection_inactive",

    // Editor / Window borders
    "border_window", "border_focused", "border_emphasized",

    // Syntax highlighting
    "syntax_function", "syntax_error", "syntax_keyword", "syntax_special_keyword",
    "syntax_operator",

    // Semantic text colors
    "text_error", "text_warning", "text_link", "text_comment", "text_string", "text_success",
    "warning_emphasis", "foreground_emphasis",

    // Extra
    "terminal_gray",

    // Semantic / Status
    "error", "warning", "success", "info",

    // Terminal colors (normal)
    "term_black", "term_red", "term_green", "term_yellow", "term_blue", "term_magenta", "term_cyan",
    "term_white",

    // Terminal colors (bright)
    "term_black_bright", "term_red_bright", "term_green_bright", "term_yellow_bright",
    "term_blue_bright", "term_magenta_bright", "term_cyan_bright", "term_white_bright"};

constexpr size_t NUM_COLOR_KEYS = std::size(COLOR_KEYS);
} // namespace clrsync::core
#endif
