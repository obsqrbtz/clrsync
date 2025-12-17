#ifndef CLRSYNC_CORE_PALETTE_COLOR_KEYS_HPP
#define CLRSYNC_CORE_PALETTE_COLOR_KEYS_HPP
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <unordered_map>
#include <string>

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
    
    {"success", 0x668a51ff},
    {"info", 0x3a898cff},
    {"warning", 0xb47837ff},
    {"error", 0xaa4e4aff},
    
    {"on_success", 0xd2d2d2ff},
    {"on_info", 0xd2d2d2ff},
    {"on_warning", 0xd2d2d2ff},
    {"on_error", 0xd2d2d2ff},
    
    {"editor_background", 0x111111ff},
    {"editor_command", 0x3a898cff},
    {"editor_comment", 0x849899ff},
    {"editor_disabled", 0x849899ff},
    {"editor_emphasis", 0xa9dc86ff},
    {"editor_error", 0xaa4e4aff},
    {"editor_inactive", 0x849899ff},
    {"editor_line_number", 0x849899ff},
    {"editor_link", 0xb0779eff},
    {"editor_main", 0xd2d2d2ff},
    {"editor_selected", 0x242424ff},
    {"editor_selection_inactive", 0x1d1c1cff},
    {"editor_string", 0x9a8652ff},
    {"editor_success", 0x668a51ff},
    {"editor_warning", 0xb47837ff},
    
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
