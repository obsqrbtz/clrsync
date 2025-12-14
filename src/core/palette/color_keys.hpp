#ifndef CLRSYNC_CORE_PALETTE_COLOR_KEYS_HPP
#define CLRSYNC_CORE_PALETTE_COLOR_KEYS_HPP
#include <cstddef>
#include <iterator>

namespace clrsync::core
{
constexpr const char* COLOR_KEYS[] = {
    // General UI
    "background",
    "on_background",

    "surface",
    "on_surface",

    "surface_variant",
    "on_surface_variant",

    "border_focused",
    "border",

    "foreground",

    "cursor",
    "accent",

    // Semantic
    "success",
    "info",
    "warning",
    "error",

    "on_success",
    "on_info",
    "on_warning",
    "on_error",

    // Editor
    "editor_background",
    "editor_command",
    "editor_comment",
    "editor_disabled",
    "editor_emphasis",
    "editor_error",
    "editor_inactive",
    "editor_line_number",
    "editor_link",
    "editor_main",
    "editor_selected",
    "editor_selection_inactive",
    "editor_string",
    "editor_success",
    "editor_warning",

    // Terminal
    "base00",
    "base01",
    "base02",
    "base03",
    "base04",
    "base05",
    "base06",
    "base07",
    "base08",
    "base09",
    "base0A",
    "base0B",
    "base0C",
    "base0D",
    "base0E",
    "base0F",
};

constexpr size_t NUM_COLOR_KEYS = std::size(COLOR_KEYS);
} // namespace clrsync::core
#endif
