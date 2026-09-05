#ifndef CLRSYNC_CORE_PALETTE_MATUGEN_MAPPINGS_HPP
#define CLRSYNC_CORE_PALETTE_MATUGEN_MAPPINGS_HPP

namespace clrsync::core
{

struct key_color_mapping
{
    const char *palette_key;
    const char *source_key;
};

inline constexpr key_color_mapping MATUGEN_COLOR_MAPPINGS[] = {
    {"accent", "primary"},
    {"accent_secondary", "secondary"},
    {"background", "background"},
    {"foreground", "on_surface"},
    {"on_background", "on_background"},
    {"surface", "surface_container_low"},
    {"on_surface", "on_surface"},
    {"surface_variant", "surface_container"},
    {"on_surface_variant", "on_surface_variant"},
    {"surface_container", "surface_container_high"},
    {"surface_high", "surface_container_highest"},
    {"border", "outline_variant"},
    {"border_focused", "outline"},
    {"cursor", "on_surface"},
    {"success", "primary"},
    {"on_success", "on_primary"},
    {"info", "tertiary"},
    {"on_info", "on_tertiary"},
    {"warning", "secondary"},
    {"on_warning", "on_secondary"},
    {"error", "error"},
    {"on_error", "on_error"},
    // Editor - Basic
    {"editor_background", "background"},
    {"editor_foreground", "on_surface"},
    {"editor_line_highlight", "surface_container"},
    {"editor_selection", "primary_container"},
    {"editor_selection_inactive", "surface_container_low"},
    {"editor_cursor", "on_surface"},
    {"editor_whitespace", "outline_variant"},
    // Editor - Gutter
    {"editor_gutter_background", "background"},
    {"editor_gutter_foreground", "outline"},
    {"editor_line_number", "outline"},
    {"editor_line_number_active", "on_surface"},
    // Editor - Syntax
    {"editor_comment", "outline"},
    {"editor_string", "tertiary"},
    {"editor_number", "secondary"},
    {"editor_boolean", "secondary"},
    {"editor_keyword", "primary"},
    {"editor_operator", "on_surface"},
    {"editor_function", "tertiary_container"},
    {"editor_variable", "on_surface"},
    {"editor_parameter", "tertiary"},
    {"editor_property", "tertiary"},
    {"editor_constant", "secondary"},
    {"editor_type", "primary"},
    {"editor_class", "primary"},
    {"editor_interface", "primary"},
    {"editor_enum", "primary"},
    {"editor_namespace", "primary_container"},
    {"editor_attribute", "tertiary_container"},
    {"editor_decorator", "tertiary_container"},
    {"editor_tag", "error"},
    {"editor_punctuation", "on_surface"},
    {"editor_link", "primary_container"},
    {"editor_regex", "secondary_container"},
    {"editor_escape_character", "secondary"},
    // Editor - Diagnostics
    {"editor_invalid", "error"},
    {"editor_error", "error"},
    {"editor_error_background", "error_container"},
    {"editor_warning", "secondary"},
    {"editor_warning_background", "secondary_container"},
    {"editor_info", "tertiary"},
    {"editor_info_background", "tertiary_container"},
    {"editor_hint", "primary"},
    {"editor_hint_background", "primary_container"},
    // Editor - UI Elements
    {"editor_active_line_border", "outline_variant"},
    {"editor_indent_guide", "outline_variant"},
    {"editor_indent_guide_active", "outline"},
    {"editor_bracket_match", "primary"},
    {"editor_search_match", "tertiary_container"},
    {"editor_search_match_active", "tertiary"},
    {"editor_find_range_highlight", "tertiary_container"},
    // Editor - Diff
    {"editor_deleted", "error"},
    {"editor_inserted", "primary"},
    {"editor_modified", "secondary"},
    {"editor_ignored", "outline_variant"},
    {"editor_folded_background", "surface_container"},
    {"base00", "background"},
};

inline constexpr std::size_t MATUGEN_COLOR_MAPPINGS_COUNT =
    sizeof(MATUGEN_COLOR_MAPPINGS) / sizeof(MATUGEN_COLOR_MAPPINGS[0]);

} // namespace clrsync::core

#endif
