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
    {"background", 0x1e1e1eff},
    {"on_background", 0xd4d4d4ff},
    
    {"surface", 0x252526ff},
    {"on_surface", 0xe8e8e8ff},
    
    {"surface_variant", 0x2d2d30ff},
    {"on_surface_variant", 0xccccccff},
    
    {"border_focused", 0x007accff},
    {"border", 0x3e3e42ff},
    
    {"foreground", 0xccccccff},
    
    {"cursor", 0xaeafadff},
    {"accent", 0x0e639cff},
    
    {"success", 0x4ec9b0ff},
    {"info", 0x4fc1ffff},
    {"warning", 0xdcdcaaff},
    {"error", 0xf48771ff},
    
    {"on_success", 0x000000ff},
    {"on_info", 0x000000ff},
    {"on_warning", 0x000000ff},
    {"on_error", 0xffffffff},
    
    {"editor_background", 0x1e1e1eff},
    {"editor_command", 0xd7ba7dff},
    {"editor_comment", 0x6a9955ff},
    {"editor_disabled", 0x808080ff},
    {"editor_emphasis", 0x569cd6ff},
    {"editor_error", 0xf44747ff},
    {"editor_inactive", 0x858585ff},
    {"editor_line_number", 0x858585ff},
    {"editor_link", 0x3794ffff},
    {"editor_main", 0xd4d4d4ff},
    {"editor_selected", 0x264f78ff},
    {"editor_selection_inactive", 0x3a3d41ff},
    {"editor_string", 0xce9178ff},
    {"editor_success", 0x89d185ff},
    {"editor_warning", 0xcca700ff},
    
    {"base00", 0x181818ff},
    {"base01", 0x282828ff},
    {"base02", 0x383838ff},
    {"base03", 0x585858ff},
    {"base04", 0xb8b8b8ff},
    {"base05", 0xd8d8d8ff},
    {"base06", 0xe8e8e8ff},
    {"base07", 0xf8f8f8ff},
    {"base08", 0xab4642ff},
    {"base09", 0xdc9656ff},
    {"base0A", 0xf7ca88ff},
    {"base0B", 0xa1b56cff},
    {"base0C", 0x86c1b9ff},
    {"base0D", 0x7cafc2ff},
    {"base0E", 0xba8bafff},
    {"base0F", 0xa16946ff},
};
} // namespace clrsync::core
#endif
