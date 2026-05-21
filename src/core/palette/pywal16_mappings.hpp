#ifndef CLRSYNC_CORE_PALETTE_PYWAL16_MAPPINGS_HPP
#define CLRSYNC_CORE_PALETTE_PYWAL16_MAPPINGS_HPP

#include "core/palette/generator_mappings.hpp"

namespace clrsync::core
{

inline constexpr index_color_mapping PYWAL16_COLOR_MAPPINGS[] = {
    // General UI
    {"background", 0},
    {"foreground", 7},
    {"cursor", 15},
    {"surface", 0},
    {"surface_variant", 8},
    {"on_background", 7},
    {"on_surface", 7},
    {"on_surface_variant", 8},
    {"border", 8},
    {"border_focused", 4},
    {"accent", 4},
    {"accent_secondary", 6},
    // Semantic
    {"success", 2},
    {"warning", 3},
    {"error", 1},
    {"info", 4},
    {"on_success", 15},
    {"on_warning", 0},
    {"on_error", 15},
    {"on_info", 15},
    // Base16
    {"base00", 0},
    {"base01", 8},
    {"base02", 8},
    {"base03", 8},
    {"base04", 8},
    {"base05", 7},
    {"base06", 15},
    {"base07", 15},
    {"base08", 1},
    {"base09", 9},
    {"base0A", 3},
    {"base0B", 2},
    {"base0C", 6},
    {"base0D", 4},
    {"base0E", 5},
    {"base0F", 11},
    // Editor - Basic
    {"editor_background", 0},
    {"editor_foreground", 7},
    {"editor_line_highlight", 8},
    {"editor_selection", 4},
    {"editor_selection_inactive", 8},
    {"editor_cursor", 15},
    {"editor_whitespace", 8},
    // Editor - Gutter
    {"editor_gutter_background", 0},
    {"editor_gutter_foreground", 8},
    {"editor_line_number", 8},
    {"editor_line_number_active", 7},
    // Editor - Syntax
    {"editor_comment", 8},
    {"editor_string", 2},
    {"editor_number", 3},
    {"editor_boolean", 3},
    {"editor_keyword", 5},
    {"editor_operator", 7},
    {"editor_function", 12},
    {"editor_variable", 7},
    {"editor_parameter", 4},
    {"editor_property", 10},
    {"editor_constant", 9},
    {"editor_type", 14},
    {"editor_class", 14},
    {"editor_interface", 14},
    {"editor_enum", 14},
    {"editor_namespace", 4},
    {"editor_attribute", 13},
    {"editor_decorator", 13},
    {"editor_tag", 1},
    {"editor_punctuation", 7},
    {"editor_link", 12},
    {"editor_regex", 5},
    {"editor_escape_character", 9},
    // Editor - Diagnostics
    {"editor_invalid", 1},
    {"editor_error", 1},
    {"editor_error_background", 0},
    {"editor_warning", 3},
    {"editor_warning_background", 0},
    {"editor_info", 4},
    {"editor_info_background", 0},
    {"editor_hint", 2},
    {"editor_hint_background", 0},
    // Editor - UI Elements
    {"editor_active_line_border", 8},
    {"editor_indent_guide", 8},
    {"editor_indent_guide_active", 7},
    {"editor_bracket_match", 6},
    {"editor_search_match", 3},
    {"editor_search_match_active", 11},
    {"editor_find_range_highlight", 3},
    // Editor - Diff
    {"editor_deleted", 1},
    {"editor_inserted", 2},
    {"editor_modified", 3},
    {"editor_ignored", 8},
    {"editor_folded_background", 8},
};

inline constexpr std::size_t PYWAL16_COLOR_MAPPINGS_COUNT =
    sizeof(PYWAL16_COLOR_MAPPINGS) / sizeof(PYWAL16_COLOR_MAPPINGS[0]);

} // namespace clrsync::core

#endif
