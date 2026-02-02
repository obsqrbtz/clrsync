#ifndef CLRSYNC_CORE_PALETTE_COLOR_KEYS_HPP
#define CLRSYNC_CORE_PALETTE_COLOR_KEYS_HPP
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <string>
#include <unordered_map>

namespace clrsync::core
{
constexpr const char *COLOR_KEYS[] = {
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
    "accent_secondary",

    // Semantic
    "success",
    "info",
    "warning",
    "error",

    "on_success",
    "on_info",
    "on_warning",
    "on_error",

    // Editor - Basic
    "editor_background",
    "editor_foreground",
    "editor_line_highlight",
    "editor_selection",
    "editor_selection_inactive",
    "editor_cursor",
    "editor_whitespace",

    // Editor - Gutter
    "editor_gutter_background",
    "editor_gutter_foreground",
    "editor_line_number",
    "editor_line_number_active",

    // Editor - Syntax
    "editor_comment",
    "editor_string",
    "editor_number",
    "editor_boolean",
    "editor_keyword",
    "editor_operator",
    "editor_function",
    "editor_variable",
    "editor_parameter",
    "editor_property",
    "editor_constant",
    "editor_type",
    "editor_class",
    "editor_interface",
    "editor_enum",
    "editor_namespace",
    "editor_attribute",
    "editor_decorator",
    "editor_tag",
    "editor_punctuation",
    "editor_link",
    "editor_regex",
    "editor_escape_character",

    // Editor - Diagnostics
    "editor_invalid",
    "editor_error",
    "editor_error_background",
    "editor_warning",
    "editor_warning_background",
    "editor_info",
    "editor_info_background",
    "editor_hint",
    "editor_hint_background",

    // Editor - UI Elements
    "editor_active_line_border",
    "editor_indent_guide",
    "editor_indent_guide_active",
    "editor_bracket_match",
    "editor_search_match",
    "editor_search_match_active",
    "editor_find_range_highlight",

    // Editor - Diff
    "editor_deleted",
    "editor_inserted",
    "editor_modified",
    "editor_ignored",
    "editor_folded_background",

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

inline const std::unordered_map<std::string, uint32_t> DEFAULT_COLORS = {
    {"background", 0x111111ff},
    {"on_background", 0xd4d4d4ff},

    {"surface", 0x111111ff},
    {"on_surface", 0xd4d4d4ff},

    {"surface_variant", 0x191919ff},
    {"on_surface_variant", 0xd4d4d4ff},

    {"border_focused", 0x2e2e2eff},
    {"border", 0x242424ff},

    {"foreground", 0xd2d2d2ff},

    {"cursor", 0xd2d2d2ff},
    {"accent", 0x9a8652ff},
    {"accent_secondary", 0x9a8652ff},

    {"success", 0x668a51ff},
    {"info", 0x3a898cff},
    {"warning", 0xb47837ff},
    {"error", 0xaa4e4aff},

    {"on_success", 0xd2d2d2ff},
    {"on_info", 0xd2d2d2ff},
    {"on_warning", 0xd2d2d2ff},
    {"on_error", 0xd2d2d2ff},

    // Editor - Basic
    {"editor_background", 0x111111ff},
    {"editor_foreground", 0xd2d2d2ff},
    {"editor_line_highlight", 0x191919ff},
    {"editor_selection", 0x242424ff},
    {"editor_selection_inactive", 0x1d1c1cff},
    {"editor_cursor", 0xd2d2d2ff},
    {"editor_whitespace", 0x3a3a3aff},

    // Editor - Gutter
    {"editor_gutter_background", 0x111111ff},
    {"editor_gutter_foreground", 0x849899ff},
    {"editor_line_number", 0x849899ff},
    {"editor_line_number_active", 0xd2d2d2ff},

    // Editor - Syntax
    {"editor_comment", 0x849899ff},
    {"editor_string", 0x9a8652ff},
    {"editor_number", 0xb47837ff},
    {"editor_boolean", 0xb47837ff},
    {"editor_keyword", 0x3a898cff},
    {"editor_operator", 0xd2d2d2ff},
    {"editor_function", 0xa9dc86ff},
    {"editor_variable", 0xd2d2d2ff},
    {"editor_parameter", 0xb0779eff},
    {"editor_property", 0x9a8652ff},
    {"editor_constant", 0xb47837ff},
    {"editor_type", 0x3a898cff},
    {"editor_class", 0x3a898cff},
    {"editor_interface", 0x3a898cff},
    {"editor_enum", 0x3a898cff},
    {"editor_namespace", 0xb0779eff},
    {"editor_attribute", 0xa9dc86ff},
    {"editor_decorator", 0xa9dc86ff},
    {"editor_tag", 0xaa4e4aff},
    {"editor_punctuation", 0xd2d2d2ff},
    {"editor_link", 0xb0779eff},
    {"editor_regex", 0xaa477bff},
    {"editor_escape_character", 0xb47837ff},

    // Editor - Diagnostics
    {"editor_invalid", 0xaa4e4aff},
    {"editor_error", 0xaa4e4aff},
    {"editor_error_background", 0x3a1a1aff},
    {"editor_warning", 0xb47837ff},
    {"editor_warning_background", 0x3a2a1aff},
    {"editor_info", 0x3a898cff},
    {"editor_info_background", 0x1a2a3aff},
    {"editor_hint", 0x668a51ff},
    {"editor_hint_background", 0x1a2a1aff},

    // Editor - UI Elements
    {"editor_active_line_border", 0x2e2e2eff},
    {"editor_indent_guide", 0x2a2a2aff},
    {"editor_indent_guide_active", 0x3a3a3aff},
    {"editor_bracket_match", 0x3a898cff},
    {"editor_search_match", 0x9a865280},
    {"editor_search_match_active", 0x9a8652ff},
    {"editor_find_range_highlight", 0x9a865240},

    // Editor - Diff
    {"editor_deleted", 0xaa4e4aff},
    {"editor_inserted", 0x668a51ff},
    {"editor_modified", 0x9a8652ff},
    {"editor_ignored", 0x849899ff},
    {"editor_folded_background", 0x191919ff},

    {"base00", 0x111111ff},
    {"base01", 0x668a51ff},
    {"base02", 0x9a8652ff},
    {"base03", 0xb47837ff},
    {"base04", 0x9a5552ff},
    {"base05", 0xaa477bff},
    {"base06", 0x3a898cff},
    {"base07", 0xb5b5b5ff},
    {"base08", 0xaa4e4aff},
    {"base09", 0xa9dc86ff},
    {"base0A", 0xb6ab82ff},
    {"base0B", 0xc5916bff},
    {"base0C", 0xac7676ff},
    {"base0D", 0xb0779eff},
    {"base0E", 0x849899ff},
    {"base0F", 0xd2d2d2ff},
};
} // namespace clrsync::core
#endif
